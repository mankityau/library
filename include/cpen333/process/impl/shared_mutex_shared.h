#ifndef CPEN333_PROCESS_SHARED_MUTEX_SHARED_H
#define CPEN333_PROCESS_SHARED_MUTEX_SHARED_H

#define SHARED_MUTEX_SHARED_NAME_SUFFIX "ms"
#define SHARED_MUTEX_SHARED_INITIALIZED 0x98271238

#include "cpen333/process/mutex.h"
#include "cpen333/process/semaphore.h"
#include "cpen333/process/shared_memory.h"

namespace cpen333 {
namespace process {

namespace impl {

// Read-preferring
/**
 * Shared-mutex implementation based on the mutex/semaphore pattern
 * See https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock for details
 */
class shared_mutex_shared : named_resource {
 protected:

  struct shared_data {
    size_t shared;
    size_t initialized;
  };

  cpen333::process::mutex shared_;     // mutex for shared access
  cpen333::process::semaphore global_; // global semaphore
  cpen333::process::shared_object<shared_data> count_;     // shared counter object

 public:
  shared_mutex_shared(const std::string &name) :
      named_resource{name + std::string(SHARED_MUTEX_SHARED_NAME_SUFFIX)},
      shared_{name + std::string(SHARED_MUTEX_SHARED_NAME_SUFFIX)},
      global_{name + std::string(SHARED_MUTEX_SHARED_NAME_SUFFIX), 1},
      count_{name + std::string(SHARED_MUTEX_SHARED_NAME_SUFFIX)} {

    // initialize storage
    std::lock_guard<decltype(shared_)> lock(shared_);
    if (count_->initialized != SHARED_MUTEX_SHARED_INITIALIZED) {
      count_->shared = 0;
      count_->initialized = SHARED_MUTEX_SHARED_INITIALIZED;
    }
  }

  // disable copy/move constructors
  shared_mutex_shared(const shared_mutex_shared &) = delete;
  shared_mutex_shared(shared_mutex_shared &&) = delete;
  shared_mutex_shared &operator=(const shared_mutex_shared &) = delete;
  shared_mutex_shared &operator=(shared_mutex_shared &&) = delete;

  void lock_shared() {

    // may hold both shared_ and global_ until writes are complete
    std::lock_guard <cpen333::process::mutex> lock(shared_);
    if (++(count_->shared) == 1) {
      global_.wait();  // "lock" semaphore preventing write access
    }
  }

  bool try_lock_shared() {
    std::unique_lock <cpen333::process::mutex> lock(shared_, std::defer_lock); // do not try yet
    if (!lock.try_lock()) {
      return false;
    }

    if (count_->shared == 0) {
      bool success = global_.try_wait();  // "lock" semaphore preventing writes
      // only increment if successful
      if (!success) {
        return false;
      }
      count_->shared = 1;
    } else {
      ++(count_->shared);
    }
    return true;
  }

  void unlock_shared() {
    std::lock_guard <cpen333::process::mutex> lock(shared_);
    if (--(count_->shared) == 0) {
      global_.notify(); // "unlock" semaphore allowing writes
    }
  }

  void lock() {
    global_.wait(); // lock semaphore
  }

  bool try_lock() {
    return global_.try_wait();
  }

  void unlock() {
    global_.notify(); // unlock semaphore
  }

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable for the specified timeout duration
   * @tparam Rep duration representation
   * @tparam Period duration period
   * @param timeout_duration timeout
   * @return true if locked successfully
   */
  template<class Rep, class Period>
  bool try_lock_for(const std::chrono::duration <Rep, Period> &timeout_duration) {
    return try_lock_until(std::chrono::steady_clock::now() + timeout_duration);
  }

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable until specified time point has been reached
   * @tparam Clock clock representation
   * @tparam Duration time
   * @param timeout_time time of timeout
   * @return true if locked successfully
   */
  template<class Clock, class Duration>
  bool try_lock_until(const std::chrono::time_point <Clock, Duration> &timeout_time) {
    return global_.wait_until(timeout_time);
  }

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable for the specified timeout duration
   * @tparam Rep duration representation
   * @tparam Period duration period
   * @param timeout_duration timeout
   * @return true if locked successfully
   */
  template<class Rep, class Period>
  bool try_lock_shared_for(const std::chrono::duration <Rep, Period> &timeout_duration) {
    return try_lock_shared_until(std::chrono::steady_clock::now() + timeout_duration);
  }

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable until specified time point has been reached
   * @tparam Clock clock representation
   * @tparam Duration time
   * @param timeout_time time of timeout
   * @return true if locked successfully
   */
  template<class Clock, class Duration>
  bool try_lock_shared_until(const std::chrono::time_point <Clock, Duration> &timeout_time) {
    std::unique_lock <cpen333::process::mutex> lock(shared_, std::defer_lock); // do not try yet
    if (!lock.try_lock_until(timeout_time)) {
      return false;
    }

    // locked, so safe to read/write to count
    if (count_->shared == 0) {
      bool success = global_.wait_until(timeout_time);  // "lock" semaphore
      if (!success) {
        return false;
      }
      count_->shared = 1;
    } else {
      ++(count_->shared);
    }
    return true;
  }

  bool unlink() {
    bool b1 = shared_.unlink();
    bool b2 = global_.unlink();
    bool b3 = count_.unlink();
    return (b1 && b2 && b3);
  }

  static bool unlink(const std::string& name) {
    bool b1 = cpen333::process::mutex::unlink(name + std::string(SHARED_MUTEX_SHARED_NAME_SUFFIX));
    bool b2 = cpen333::process::semaphore::unlink(name + std::string(SHARED_MUTEX_SHARED_NAME_SUFFIX));
    bool b3 = cpen333::process::shared_object<shared_data>::unlink(name + std::string(SHARED_MUTEX_SHARED_NAME_SUFFIX));
    return (b1 && b2 && b3);
  }

};

} // implementation

using shared_mutex_shared = impl::shared_mutex_shared;
using shared_timed_mutex_shared = impl::shared_mutex_shared;

} // process
} // cpen333


#endif //CPEN333_PROCESS_SHARED_MUTEX_SHARED_H
