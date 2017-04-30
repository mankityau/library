#ifndef CPEN333_PROCESS_WINDOWS_SEMAPHORE_H
#define CPEN333_PROCESS_WINDOWS_SEMAPHORE_H

// maximum possible size of a semaphore
#define MAX_SEMAPHORE_SIZE LONG_MAX

// suffix appended to semaphore names for uniqueness
#define SEMAPHORE_NAME_SUFFIX "_sem_"

#include <string>
#include <chrono>

#include <windows.h>

#include "cpen333/util.h"
#include "cpen333/process/impl/named_resource.h"

namespace cpen333 {
namespace process {
namespace windows {

class semaphore : public named_resource {
 public:
  using native_handle_type = HANDLE;

  semaphore(const std::string& name, size_t value = 1) :
      named_resource{name+std::string(SEMAPHORE_NAME_SUFFIX)}, handle_{NULL} {

    // create named semaphore
    handle_ = CreateSemaphore(NULL, value, MAX_SEMAPHORE_SIZE, name_ptr());
    if (handle_ == INVALID_HANDLE_VALUE) {
       cpen333::perror(std::string("Cannot create semaphore ")+this->name());
    }
  }

  ~semaphore() {
    // close the semaphore
    if (!CloseHandle(handle_)) {
      cpen333::perror(std::string("Cannot destroy semaphore ")+name());
    }
  }

  size_t value() {
    LONG val = 0;
    // check if one available, if so grab it, otherwise count was zero
    if( WAIT_OBJECT_0 == WaitForSingleObject(handle_,0L)) {
      // Semaphores count is at least one, determine previous value then release it
      if (!ReleaseSemaphore(handle_, 1, &val)) {
        cpen333::perror(std::string("Cannot get semaphore value ") + name());
      }
      ++val;  // reduce count by 1 to account for artificial wait
    }
    return val;
  }

  void wait() {
    UINT result;
    // wait on handle_ until we are successful
    do {
      result = WaitForSingleObject(handle_, INFINITE) ;
    } while (result != WAIT_OBJECT_0 && result != WAIT_FAILED);
    // check if we had an error
    if (result == WAIT_FAILED) {
      cpen333::perror(std::string("Failed to wait on semaphore ")+name());
    }
  }

  bool try_wait() {
    // try locking with a 0 timeout
    UINT result = WaitForSingleObject(handle_, 0) ;
    if (result == WAIT_FAILED) {
      cpen333::perror(std::string("Failed to wait on semaphore ")+name());
    }
    // if we were actually signaled, return true
    return (result == WAIT_OBJECT_0);
  }

  template< class Rep, class Period >
  bool wait_for( const std::chrono::duration<Rep,Period>& timeout_duration ) {
    DWORD time = std::chrono::duration_cast<std::chrono::milliseconds>(timeout_duration).count();
    if (time < 0) {
      time = 0;
    }
    UINT result = WaitForSingleObject(handle_, time) ;
    if (result == WAIT_FAILED) {
      cpen333::perror(std::string("Failed to wait for semaphore ")+name());
    }

    return (result == WAIT_OBJECT_0);
  }

  template< class Clock, class Duration >
  bool wait_until( const std::chrono::time_point<Clock,Duration>& timeout_time ) {
    auto duration = timeout_time - std::chrono::steady_clock::now();
    return wait_for(duration);
  }

  void notify() {
    bool success = ReleaseSemaphore(handle_, 1, NULL) ;  // FALSE on failure, TRUE on success
    if (!success) {
      cpen333::perror(std::string("Failed to post semaphore ")+name());
    }
  }

  native_handle_type native_handle() {
    return handle_;
  }

  /**
   * Windows does not support unlinking of named objects
   * @return false
   */
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

using semaphore = windows::semaphore;

} // process
} // cpen333

#endif //CPEN333_PROCESS_WINDOWS_SEMAPHORE_H
