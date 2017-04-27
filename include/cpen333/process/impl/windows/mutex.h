#ifndef CPEN333_PROCESS_MUTEX_WINDOWS_H
#define CPEN333_PROCESS_MUTEX_WINDOWS_H

// suffix to append to mutex names for uniqueness
#define MUTEX_NAME_SUFFIX "__mutex__"

#include <string>
#include <chrono>

#include <windows.h>

#include "cpen333/util.h"
#include "cpen333/process/impl/named_resource.h"

namespace cpen333 {
namespace process {
namespace windows {

class mutex : public named_resource {
 public:
  using native_handle_type = HANDLE;

  mutex(const std::string& name) :
    named_resource{name + std::string(MUTEX_NAME_SUFFIX)} {
    handle_  = CreateMutex(NULL, false, name_ptr()) ;
    if (handle_ == INVALID_HANDLE_VALUE) {
      cpen333::perror(std::string("Cannot create mutex ")+this->name());
    }
  }

  ~mutex() {
    if (!CloseHandle(handle_)) {
      cpen333::perror(std::string("Cannot destroy mutex ")+name());
    }
  }

  void lock() {
    UINT result;
    // wait on handle_ until we are successful (ignore spurious wakes)
    do {
      result = WaitForSingleObject(handle_, INFINITE) ;
    } while (result != WAIT_OBJECT_0 && result != WAIT_FAILED);
    if (result == WAIT_FAILED) {
      cpen333::perror(std::string("Failed to lock mutex "+name()));
    }
  }

  bool try_lock() {
    // try locking with a 0 timeout
    UINT result = WaitForSingleObject(handle_, 0) ;
    if (result == WAIT_FAILED) {
      cpen333::perror(std::string("Failed to lock mutex "+name()));
    }

    // if we were actually signaled, return true
    return (result == WAIT_OBJECT_0);
  }

  template< class Rep, class Period >
  bool try_lock_for( const std::chrono::duration<Rep,Period>& timeout_duration ) {
    DWORD time = std::chrono::duration_cast<std::chrono::milliseconds>(timeout_duration).count();
    UINT result = WaitForSingleObject(handle_, time) ;
    if (result == WAIT_FAILED) {
      cpen333::perror(std::string("Failed to lock mutex "+name()));
    }
    // if we were actually signaled, return true
    return (result == WAIT_OBJECT_0);
  };

  template< class Clock, class Duration >
  bool try_lock_until( const std::chrono::time_point<Clock,Duration>& timeout_time ) {
    auto duration = timeout_time - std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

    if (ms.count() < 0) {
      ms = std::chrono::milliseconds(0);
    }
    return try_lock_for(ms);
  };

  bool unlock() {
    bool success = ReleaseMutex(handle_) ;  // FALSE on failure, TRUE on success
    if (!success) {
      cpen333::perror(std::string("Failed to unlock mutex "+name()));
    }
    return success;
  }

  native_handle_type native_handle() {
    return handle_;
  }

  bool unlink() {
    return false;
  }

  static bool unlink(const std::string& name) {
    return false;
  }

 private:
  native_handle_type handle_;

};

} // native implementation

using mutex = windows::mutex;
using timed_mutex = windows::mutex;

} // process
} // cpen333

#endif //CPEN333_PROCESS_MUTEX_WINDOWS_H
