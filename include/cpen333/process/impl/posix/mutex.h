#ifndef CPEN333_PROCESS_MUTEX_POSIX_H
#define CPEN333_PROCESS_MUTEX_POSIX_H

// suffix to append to mutex names for uniqueness
#define MUTEX_NAME_SUFFIX "_mux"

#include <string>
#include <chrono>
#include <thread>

#include "cpen333/util.h"
#include "cpen333/process/semaphore.h"

namespace cpen333 {
namespace process {
namespace posix {

/**
 * Based on a named semaphore, but tracks the thread id
 */
class mutex : public named_resource {
 public:
  using native_handle_type = semaphore::native_handle_type;

  mutex(const std::string& name) :
    named_resource{name + std::string(MUTEX_NAME_SUFFIX)},
    semaphore_{name + std::string(MUTEX_NAME_SUFFIX), 1}
    // thread_{}
  {}

  ~mutex() {}

  void lock() {
    semaphore_.wait();
    // thread_ = std::this_thread::get_id();  // set id
  }

  bool try_lock() {
    bool success = semaphore_.try_wait();
    //    if (success) {
    //      thread_ = std::this_thread::get_id();
    //    }
    return success;
  }

  template< class Rep, class Period >
  bool try_lock_for( const std::chrono::duration<Rep,Period>& timeout_duration ) {
    bool success = semaphore_.wait_for(timeout_duration);
    //    if (success) {
    //      thread_ = std::this_thread::get_id();
    //    }
    return success;
  }

  template< class Clock, class Duration >
  bool try_lock_until( const std::chrono::time_point<Clock,Duration>& timeout_time ) {
    bool success = semaphore_.wait_until(timeout_time);
    //    if (success) {
    //      thread_ = std::this_thread::get_id();
    //    }
    return success;
  }

  void unlock() {
    //    if (thread_ != std::this_thread::get_id()) {
    //      cpen333::perror(std::string("Cannot unlock mutex locked by different thread, ") + name());
    //      return;
    //    }
    semaphore_.notify();
    //  thread_ = {};  // clear thread id
  }

  native_handle_type native_handle() {
    return semaphore_.native_handle();
  }

  bool unlink() {
    return semaphore_.unlink();
  }

  static bool unlink(const std::string& name) {
   return cpen333::process::posix::semaphore::unlink(name + std::string(MUTEX_NAME_SUFFIX));
  }

 private:
  cpen333::process::posix::semaphore semaphore_;  // binary semaphore
  // XXX Tracking ID fails if multiple threads are actually using this mutex simultaneously
  //     We *could* prevent this by using yet another mutex, but that would make this much slower
  // std::thread::id thread_;  // for tracking thread id

};

} // native implementation

using mutex = posix::mutex;
using timed_mutex = posix::mutex;

} // process
} // cpen333

#endif //CPEN333_PROCESS_MUTEX_POSIX_H
