#ifndef CPEN333_THREAD_SHARED_MUTEX_SHARED_H
#define CPEN333_THREAD_SHARED_MUTEX_SHARED_H

#include <mutex>
#include "cpen333/thread/semaphore.h"

namespace cpen333 {
namespace thread {

namespace impl {

// Read-preferring
/**
 * Shared-mutex implementation based on the mutex/semaphore pattern
 * See https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock for details
 */
class shared_mutex_shared {
 protected:

  std::mutex mutex_;     // mutex for shared access
  cpen333::thread::semaphore global_; // global semaphore
  size_t shared_;          // shared counter object

 public:
  shared_mutex_shared() :  mutex_{}, global_{1}, shared_{0} { }

  // disable copy/move constructors
  shared_mutex_shared(const shared_mutex_shared &) = delete;
  shared_mutex_shared(shared_mutex_shared &&) = delete;
  shared_mutex_shared &operator=(const shared_mutex_shared &) = delete;
  shared_mutex_shared &operator=(shared_mutex_shared &&) = delete;

  void lock_shared() {
    // may hold both mutex_ and global_ until writes are complete
    std::lock_guard <std::mutex> lock(mutex_);
    if (++shared_ == 1) {
      global_.wait();  // "lock" semaphore preventing write access
    }
  }

  bool try_lock_shared() {
    std::unique_lock <std::mutex> lock(mutex_, std::defer_lock); // do not try yet
    if (!lock.try_lock()) {
      return false;
    }

    if (shared_ == 0) {
      bool success = global_.try_wait();  // "lock" semaphore preventing writes
      // only increment if successful
      if (!success) {
        return false;
      }
      shared_ = 1;
    } else {
      ++shared_;
    }
    return true;
  }

  void unlock_shared() {
    std::lock_guard <std::mutex> lock(mutex_);
    if (--shared_ == 0) {
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
  };

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
  };

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
  };

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable until specified time point has been reached
   * @tparam Clock clock representation
   * @tparam Duration time
   * @param timeout_time time of timeout
   * @return true if locked successfully
   */
  template<class Clock, class Duration>
  bool try_lock_shared_until(const std::chrono::time_point <Clock, Duration> &timeout_time) {
    std::unique_lock <std::mutex> lock(mutex_, std::defer_lock); // do not try yet
    if (!lock.try_lock_until(timeout_time)) {
      return false;
    }

    // locked, so safe to read/write to count
    if (shared_ == 0) {
      bool success = global_.wait_until(timeout_time);  // "lock" semaphore
      if (!success) {
        return false;
      }
      shared_ = 1;
    } else {
      ++shared_;
    }
    return true;
  };
};

} // implementation

using shared_mutex_shared = impl::shared_mutex_shared;
using shared_timed_mutex_shared = impl::shared_mutex_shared;

} // thread
} // cpen333

#endif //CPEN333_THREAD_SHARED_MUTEX_SHARED_H
