/**
 * @file
 * @brief POSIX implementation of a socket
 *
 */
#ifndef CPEN333_PROCESS_POSIX_SOCKET_H
#define CPEN333_PROCESS_POSIX_SOCKET_H

#include <string>
#include <cstdint>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "../../../util.h"

#ifndef CPEN333_SOCKET_DEFAULT_PORT
#define CPEN333_SOCKET_DEFAULT_PORT 5120
#endif

#define INVALID_SOCKET  -1
#define SOCKET_ERROR -1

namespace cpen333 {
namespace process {

namespace posix {

// forward declaration so client can friend it
class socket_server;

class socket_client {
 private:
  std::string server_;
  int port_;
  int socket_;

  bool open_;
  bool connected_;

  friend class socket_server;

  void __initialize(const std::string& server, int port,
                    int socket, bool open, bool connected) {
    server_ = server;
    port_ = port;
    socket_ = socket;
    open_ = open;
    connected_ = connected;
  }

 public:
  socket_client() : server_("localhost"), port_(CPEN333_SOCKET_DEFAULT_PORT),
                    socket_(INVALID_SOCKET), open_(false), connected_(false) {}

  socket_client(const std::string& server, int port) :
      server_(server), port_(port),
      socket_(INVALID_SOCKET), open_(false), connected_(false) {}

  ~socket_client() {
    close();
  }

  bool open() {

    // don't open if already opened
    if (open_) {
      return false;
    }

    // don't open if illegal port
    if (port_ <= 0) {
      return false;
    }

    /* Obtain address(es) matching host/port */
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    struct addrinfo *addrresult;
    std::string strport = std::to_string(port_);
    int status = getaddrinfo(server_.c_str(), strport.c_str(),
                             &hints, &addrresult);
    if (status != 0) {
      cpen333::error(std::string("getaddrinfo(...) failed with error: ")
                         + std::to_string(status));
      return false;
    }

    // Attempt to connect to an address until one succeeds
    for (struct addrinfo* ptr = addrresult; ptr != NULL; ptr = ptr->ai_next) {

      // Create a SOCKET for connecting to server
      socket_ = socket(ptr->ai_family, ptr->ai_socktype,
                             ptr->ai_protocol);

      if (socket_ == INVALID_SOCKET) {
        continue;
      }

      // Connect to server.
      status = ::connect(socket_, ptr->ai_addr, ptr->ai_addrlen);
      if (status != SOCKET_ERROR) {
        break;
      }

      // failed to connect, close socket
      ::close(socket_);
      socket_ = INVALID_SOCKET;
    }
    freeaddrinfo(addrresult);

    if (socket_ == INVALID_SOCKET) {
      cpen333::error(std::string("Unable to connect to server: ")
                         + server_ + std::string(":") + strport);
      return false;
    }

    open_ = true;
    connected_ = true;
    return true;
  }

  bool send(const std::string str) {
    return send(str.c_str(), str.length());
  }

  bool send(const char* buff, size_t len) {

    if (!connected_) {
      return false;
    }

    // write all contents
    ssize_t nwrite = 0;
    do {
      ssize_t lwrite = write(socket_, &buff[nwrite], len-nwrite);
      if (lwrite == -1) {
        cpen333::perror(std::string("write(...) to socket failed"));
        return false;
      }
      nwrite += lwrite;
    } while ((size_t)nwrite != len);

    return true;
  }

  int receive(char* buff, int len) {

    if (!open_) {
      return -1;
    }

    ssize_t nread = read(socket_, buff, len);
    if (nread == -1) {
      cpen333::perror("write(...) to socket failed");
    }
    return nread;
  }

  bool disconnect() {

    if (!connected_) {
      return false;
    }

    // shutdown the connection since no more data will be sent
    int status = ::shutdown(socket_, SHUT_WR);
    if (status != 0) {
      cpen333::perror("shutdown(...) failed");
      return false;
    }
    connected_ = false;
    return true;
  }

  bool close() {

    if (!open_) {
      return false;
    }
    if (connected_) {
      disconnect();
    }

    // Receive and discard until the peer closes the connection
    static const int recvbuflen = 1024;
    char recvbuf[recvbuflen];
    ssize_t nread = 0;
    do {
      nread = read(socket_, recvbuf, recvbuflen);
      if (nread < 0) {
        cpen333::perror("socket read(...) failed");
      }
    } while( nread > 0 );

    // cleanup
    ::close(socket_);
    socket_ = INVALID_SOCKET;
    open_ = false;
    return true;
  }

};


class socket_server {
  int port_;
  int socket_;
  bool open_;

 public:
  socket_server() : port_(CPEN333_SOCKET_DEFAULT_PORT),
                    socket_(INVALID_SOCKET), open_(false) {}
  socket_server(int port) : port_(port), socket_(INVALID_SOCKET),
                            open_(false) {}

  ~socket_server() {
    close();
  }

  bool start() {

    if (open_){
      return false;
    }

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    struct addrinfo *addrresult = NULL;
    std::string strport = std::to_string(port_);
    int status = getaddrinfo(NULL, strport.c_str(), &hints, &addrresult);
    if ( status != 0 ) {
      cpen333::error(std::string("getaddrinfo(...) failed with error: ")
                         + std::to_string(status));
      return false;
    }

    // Create a SOCKET for connecting to server
    socket_ = socket(addrresult->ai_family, addrresult->ai_socktype,
                     addrresult->ai_protocol);
    if (socket_ == INVALID_SOCKET) {
      cpen333::perror("socket(...) failed");
      freeaddrinfo(addrresult);
      return false;
    }

    // Setup the TCP listening socket
    status = bind( socket_, addrresult->ai_addr, (int)addrresult->ai_addrlen);
    if (status == SOCKET_ERROR) {
      cpen333::perror("bind(...) failed");
      freeaddrinfo(addrresult);
      ::close(socket_);
      socket_ = INVALID_SOCKET;
      return false;
    }
    freeaddrinfo(addrresult);

    if (port_ == 0) {
      struct sockaddr_in sin;
      socklen_t addrlen = sizeof(sin);
      status = getsockname(socket_, (struct sockaddr *)&sin, &addrlen);
      if(status == 0 ) {
        port_ = ntohs(sin.sin_port);
      } else {
        cpen333::error(std::string("getsockname(...) failed with error: ")
                           + std::to_string(status));
      }
    }

    status = listen(socket_, SOMAXCONN);
    if (status == SOCKET_ERROR) {
      cpen333::perror("listen(...) failed");
      ::close(socket_);
      socket_ = INVALID_SOCKET;
      return false;
    }

    open_ = true;
    return true;
  }

  bool accept(socket_client& client) {
    if (!open_) {
      return false;
    }

    int client_socket = INVALID_SOCKET;

    // Accept a client socket
    client_socket = ::accept(socket_, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
      cpen333::perror("accept(...) failed");
      return false;
    }

    client.close();
    client.__initialize("", -1, client_socket, true, true);

    return true;
  }

  bool close() {
    if (!open_) {
      return false;
    }

    ::close(socket_);
    socket_ = INVALID_SOCKET;
    open_ = false;
    return true;
  }

  int get_port() {
    return port_;
  }

};

} // posix

typedef posix::socket_client socket_client;
typedef posix::socket_server socket_server;


} // process
} // cpen333

#endif