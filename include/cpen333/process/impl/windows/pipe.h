/**
 * @file
 * @brief Windows implementation of a pipe
 */
#ifndef CPEN333_PROCESS_WINDOWS_PIPE_H
#define CPEN333_PROCESS_WINDOWS_PIPE_H

// prevent windows max macro
#undef NOMINMAX
#define NOMINMAX 1
#include <windows.h>
#include <mutex>
#include <thread>
#include <chrono>

#include "mutex.h"
#include "../named_resource_base.h"
#include "../../../util.h"

#define PIPE_BUFF_SIZE 1024
#define WINDOWS_PIPE_PREFIX "\\\\.\\pipe\\"

namespace cpen333 {
namespace process {

namespace windows {

class pipe_server;

/**
 * @brief Interprocess pipe
 */
class pipe : private impl::named_resource_base {
 private:
  HANDLE pipe_;
  bool open_;

  friend class pipe_server;

  /**
   * @brief Initialize a pipe with provided info
   *
   * For use by the pipe server when creating pipe
   *
   * @param name pipe name
   * @param pipe_ pipe handle
   * @param open whether the pipe is open
   */
  void __initialize(const char* name_ptr, HANDLE pipeh, bool open) {
    set_name_raw(name_ptr);
    pipe_ = pipeh;
    open_ = open;
  }

 public:
  /**
   * @brief Default constructor, for use with a server
   */
  pipe() : impl::named_resource_base(""), pipe_(INVALID_HANDLE_VALUE), open_(false) {}

  /**
   * @brief Main constructor, creates a named pipe
   * @param name identifier for connecting to an existing inter-process pipe
   */
  pipe(const std::string& name) : impl::named_resource_base(name), pipe_(INVALID_HANDLE_VALUE), open_(false) {}

  pipe(const pipe &) DELETE_METHOD;
  pipe &operator=(const pipe &) DELETE_METHOD;

  pipe(pipe&& other) : impl::named_resource_base(""), pipe_(INVALID_HANDLE_VALUE), open_(false) {
    *this = std::move(other);
  }
  pipe &operator=(pipe&& other) {
    __initialize(other.name_ptr(), other.pipe_, other.open_);
    other.set_name_raw("");
    other.pipe_ = INVALID_HANDLE_VALUE;
    other.open_ = false;
    return *this;
  }
  
  /**
   * @brief Destructor, closes pipe if not already closed
   */
  ~pipe() {
    close();
  }

  /**
   * @brief Opens pipe if not already open, attempts to connect
   * to pipe server.
   *
   * @return true if connection established, false otherwise
   */
  bool open() {

    // don't open if already opened
    if (open_) {
      return false;
    }

    // try to create a pipe
    std::string pipename = WINDOWS_PIPE_PREFIX;
    pipename.append(name());

    auto started = std::chrono::system_clock::now();
    while(true) {
      // Wait for pending pipe connection
      if (WaitNamedPipe(pipename.c_str(), NMPWAIT_USE_DEFAULT_WAIT) == 0) {
        DWORD err = GetLastError();
        if (err == ERROR_SEM_TIMEOUT) {
          cpen333::perror("Pipe failed to wait for server");
          return false;
        }
        auto now = std::chrono::system_clock::now();
        auto tdiff = std::chrono::duration_cast<std::chrono::milliseconds>(now-started);
        if (tdiff.count() > 10000) {
          cpen333::perror("Pipe failed to wait for server");
          return false;
        }
        std::this_thread::yield();
        continue;
      }

      SetLastError(0);
      std::string pipename = WINDOWS_PIPE_PREFIX;
      pipename.append(name());
      pipe_ = CreateFile(
          pipename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL
      );

      if (pipe_ != INVALID_HANDLE_VALUE) {
        break;
      }

      // check for any error other than busy
      if (GetLastError() != ERROR_PIPE_BUSY) {
        cpen333::perror("Pipe open() failed");
        if (pipe_ != INVALID_HANDLE_VALUE) {
          CloseHandle(pipe_);
          pipe_ = INVALID_HANDLE_VALUE;
        }
        return false;
      }
    }

    open_ = true;
    return true;
  }

  /**
   * @brief Writes a string to the pipe, including the terminating zero
   * @param str string to send
   * @return true if send successful, false otherwise
   */
  bool write(const std::string& str) {
    return write(str.c_str(), str.length()+1);
  }

  /**
   * @brief Writes bytes to the pipe
   *
   * @param buff pointer to data buffer to send
   * @param len number of bytes to send
   * @return true if send successful, false otherwise
   */
  bool write(const void* buff, size_t len) {

    if (!open_) {
      return false;
    }

    size_t nwrite;
    while (nwrite < len) {
      DWORD lwrite;
      int success = WriteFile(
          pipe_,
          buff,
          len,
          &lwrite,
          NULL
      );
      if (!success) {
        cpen333::perror("Failed to write to pipe");
        return false;
      }
      nwrite += lwrite;
    }

    return true;
  }

  /**
   * @brief Reads bytes of data from a pipe
   * @param buff pointer to data buffer to populate
   * @param len size of buffer
   * @return number of bytes read, or -1 if error
   */
  ssize_t read(void* buff, size_t len) {

    if (!open_) {
      return -1;
    }

    DWORD nread = 0;
    int success = ReadFile(
        pipe_,    // pipe handle
        buff,     // buffer to receive reply
        len,      // size of buffer
        &nread,   // number of bytes read
        NULL);    // not overlapped

    if ( !success ) {
      DWORD err = GetLastError();
      if (err == ERROR_BROKEN_PIPE) {
        close();
        return -1;
      } else if ( err != ERROR_MORE_DATA ) {
        cpen333::perror("Pipe read(...) failed");
        return -1;
      }
    }

    return (ssize_t)nread;
  }

  /**
   * @brief Closes the pipe
   * @return true if successful, false otherwise
   */
  bool close() {

    if (!open_) {
      return false;
    }

    // cleanup
    int success = CloseHandle(pipe_);
    if (success == 0) {
      cpen333::perror("Failed to close pipe");
    }
    pipe_ = INVALID_HANDLE_VALUE;
    open_ = false;

    return (success != 0);
  }

  bool unlink() {
    return false;
  }

  static bool unlink(const std::string& name) {
    UNUSED(name);
    return false;
  }
};


/**
 * @brief Pipe server
 *
 * Implementation of a named pipe server that listens
 * for connections.  The server is NOT started automatically.
 * To start listening for connections, call the start() function.
 */
class pipe_server : private impl::named_resource_base {
  bool open_;

 public:
  /**
   * @brief Default constructor, creates a pipe server that listens for new connections
   */
  pipe_server(const std::string& name) : impl::named_resource_base(name), open_(false) { }

  pipe_server(const pipe_server &) DELETE_METHOD;
  pipe_server(pipe_server &&) DELETE_METHOD;
  pipe_server &operator=(const pipe_server &) DELETE_METHOD;
  pipe_server &operator=(pipe_server &&) DELETE_METHOD;

  /**
   * @brief Destructor, closes the pipe server
   */
  ~pipe_server() {
    close();
  }

  /**
   * @brief Starts listening for connections.
   * @return true if successful, false otherwise.
   */
  bool start() {

    if (open_){
      return false;
    }

    // nothing really to do...

    open_ = true;
    return true;
  }

  /**
   * @brief Accepts a client connection
   *
   * This method will block until a client connects to the pipe server.
   * Upon connection, the provide client is populated and automatically
   * opened.
   *
   * @param client connected pipe client
   * @return true if successful, false otherwise
   */
  bool accept(pipe& client) {
    if (!open_) {
      return false;
    }

    std::string pipename = WINDOWS_PIPE_PREFIX;
    pipename.append(name());

    HANDLE pipe = CreateNamedPipe( pipename.c_str(), PIPE_ACCESS_DUPLEX,
                                   PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                   PIPE_UNLIMITED_INSTANCES,
                                   PIPE_BUFF_SIZE, PIPE_BUFF_SIZE,
                                   0, NULL);

    if (pipe == INVALID_HANDLE_VALUE) {
      cpen333::perror("Failed to create named pipe");
      return false;
    }

    // Accept a client pipe
    // wait for client to connect
    DWORD success = ConnectNamedPipe(pipe, NULL) ?
                    TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!success) {
      cpen333::perror("Failed to connect pipe");
      CloseHandle(pipe);
      return false;
    }

    client.close();
    client.__initialize(name_ptr(),pipe,true);

    return true;
  }

  /**
   * @brief Close the server socket
   * @return true if successful, false otherwise
   */
  bool close() {
    if (!open_) {
      return false;
    }
    open_ = false;
    return true;
  }

  bool unlink() {
    return false;
  }

  static bool unlink(const std::string& name) {
    UNUSED(name);
    return false;
  }
};

} // windows

/**
 * @brief Windows implementation of a pipe client
 */
typedef windows::pipe pipe;

/**
 * @brief Windows implementation of a pipe server
 */
typedef windows::pipe_server pipe_server;

} // process
} // cpen333

#endif