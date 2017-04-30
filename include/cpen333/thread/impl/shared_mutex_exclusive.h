#ifndef CPEN333_THREAD_SHARED_MUTEX_EXCLUSIVE_H
#define CPEN333_THREAD_SHARED_MUTEX_EXCLUSIVE_H

#include <mutex>
#include <condition_variable>
#include "cpen333/thread/semaphore.h"
#include "cpen333/thread/condition.h"

namespace cpen333 {
namespace thread {

namespace impl {

// Write-preferring
/**
 * Shared-mutex implementation based on the mutex/semaphore pattern
 * See https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock for details
 */
class shared_mutex_exclusive {
 protected:

  std::mutex mutex_;                        // mutex for shared access
  cpen333::thread::semaphore global_;        // global semaphore
  size_t shared_count_;
  size_t exclusive_count_;
  std::mutex exclusive_;                     // protect writes to the "exclusive" field, CANNOT WAIT HERE
  cpen333::thread::condition cond_;          // condition of no writers

 public:
  shared_mutex_exclusive() :
      mutex_{},
      global_{1},   // gate opened
      shared_count_{0},
      exclusive_count_{0},
      exclusive_{},
      cond_{true}  // gate starts opened
  {}

  // disable copy/move constructors
  shared_mutex_exclusive(const shared_mutex_exclusive &) = delete;
  shared_mutex_exclusive(shared_mutex_exclusive &&) = delete;
  shared_mutex_exclusive &operator=(const shared_mutex_exclusive &) = delete;
  shared_mutex_exclusive &operator=(shared_mutex_exclusive &&) = delete;

  void lock_shared() {

    cond_.wait();           // wait until no exclusive access

    // increment number of waiting readers
    std::lock_guard<std::mutex> lock(mutex_);
    if (++shared_count_ == 1) {
      global_.wait();       // "lock" semaphore preventing write access, could be waiting here for writer to finish
    }
  }

  bool try_lock_shared() {
    if (!cond_.wait_for(std::chrono::milliseconds(0))) {
      return false;
    }

    std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
    if (!lock.try_lock()) {
      return false;
    }

    if (shared_count_ == 0) {
      bool success = global_.try_wait();  // "lock" semaphore preventing writes
      // only increment if successful
      if (!success) {
        return false;
      }
      shared_count_ = 1;
    } else {
      ++shared_count_;
    }
    return true;
  }

  void unlock_shared() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (--shared_count_ == 0) {
      global_.notify(); // "unlock" semaphore
    }
  }

  void lock() {
    // next in line
    {
      std::lock_guard<std::mutex> lock(mutex_);
      // if only one writer, block all future readers
      std::lock_guard<std::mutex> ex(exclusive_);
      if (++exclusive_count_ == 1) {
        cond_.reset();
      }
    }
    global_.wait(); // lock semaphore
  }

  bool try_lock() {

    // next in line
    {
      std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
      if (!lock.try_lock()) {
        return false; // someone is currently locked
      }

      // try holding both locks at once immediately
      if (!global_.try_wait()) {
        return false;
      }

      // I now hold both locks, which means there are no current readers or writers
      std::lock_guard<std::mutex> ex(exclusive_);
      exclusive_count_ = 1;
      cond_.reset();   // prevent future readers
    }

    return true;
  }

  void unlock() {

    global_.notify(); // unlock semaphore  // allow next reader/writer waiting

    std::lock_guard<std::mutex> lock(mutex_);   // access to data
    std::lock_guard<std::mutex> ex(exclusive_);
    if (--exclusive_count_ == 0) {
      cond_.notify();  // open gate for readers
    }
  }

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable for the specified timeout duration
   * @tparam Rep duration representation
   * @tparam Period duration period
   * @param timeout_duration timeout
   * @return true if locked successfully
   */
  template<class Rep, class Period>
  bool try_lock_for(const std::chrono::duration<Rep, Period> &timeout_duration) {
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
  bool try_lock_until(const std::chrono::time_point<Clock, Duration> &timeout_time) {

    // next in line
    {
      std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
      if (!lock.try_lock_until(timeout_time)) {
        return false;
      }

      // notify the line that there is an exclusive lock waiting
      std::lock_guard<std::mutex> ex(exclusive_);  // fast lock, doesn't protect any waits
      if (++exclusive_count_ == 1) {
        cond_.reset();
      }
    }

    if (!global_.wait_until(timeout_time)) {
      // quickly remove exclusive waiter, allowing others to proceed
      std::lock_guard<std::mutex> ex(exclusive_);
      if (--exclusive_count_ == 0) {
        cond_.notify();  // open gate for readers
      }
    }
    return true;
  }

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable for the specified timeout duration
   * @tparam Rep duration representation
   * @tparam Period duration period
   * @param timeout_duration timeout
   * @return true if locked successfully
   */
  template<class Rep, class Period>
  bool try_lock_shared_for(const std::chrono::duration<Rep, Period> &timeout_duration) {
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
  bool try_lock_shared_until(const std::chrono::time_point<Clock, Duration> &timeout_time) {

    if (!cond_.wait_until(timeout_time)) {
      return false;
    }
    // no current exclusives waiting

    std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
    if (!lock.try_lock_until(timeout_time)) {
      return false;
    }

    if (shared_count_ == 0) {
      bool success = global_.wait_until(timeout_time);  // "lock" semaphore preventing writes
      // only increment if successful
      if (!success) {
        return false;
      }
      shared_count_ = 1;
    } else {
      ++shared_count_;
    }
    return true;
  }
};

} // impl

using shared_mutex_exclusive = impl::shared_mutex_exclusive;
using shared_timed_mutex_exclusive = impl::shared_mutex_exclusive;

} // thread
} // cpen333

#endif //CPEN333_THREAD_SHARED_MUTEX_EXCLUSIVE_H
