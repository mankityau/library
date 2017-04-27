//
// Created by Antonio on 2017-04-15.
//

#ifndef CPEN333_THREAD_SEMAPHORE_H
#define CPEN333_THREAD_SEMAPHORE_H

#include <mutex>
#include <condition_variable>
#include <chrono>

namespace cpen333 {
namespace thread {

/**
 * Simple implementation of a local semaphore
 * adapted from http://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
 * @tparam Mutex mutex type
 * @tparam CondVar condition variable type
 */
template <typename Mutex, typename CondVar>
class basic_semaphore {
 public:
  using native_handle_type = typename CondVar::native_handle_type;

  /**
   * Simple constructor that allows setting the initial count
   * @param count resource count (default 1)
   */
  explicit basic_semaphore(size_t count = 1) : mutex_{}, cv_{}, count_{count} {}

  // do not allow copying or moving
  basic_semaphore(const basic_semaphore&) = delete;
  basic_semaphore(basic_semaphore&&) = delete;
  basic_semaphore& operator=(const basic_semaphore&) = delete;
  basic_semaphore& operator=(basic_semaphore&&) = delete;

  /**
   * Unblocks one resource count
   */
  void notify() {
    std::lock_guard<Mutex> lock{mutex_};
    ++count_;
    cv_.notify_one();
  }

  /**
   * Waits for one resource count to be free
   */
  void wait() {
    std::unique_lock<Mutex> lock{mutex_};
    cv_.wait(lock, [&]{ return count_ > 0; });
    --count_;
  }

  /**
   * Tests if a resource is available, and if so, acquires
   * @return true if resource acquired (i.e. was available), false otherwise
   */
  bool try_wait() {
    std::lock_guard<Mutex> lock{mutex_};
    if (count_ > 0) {
      --count_;
      return true;
    }
    return false;
  }

  /**
   * Attempt to acquire resource, but only block for a specified period of time
   * @tparam Rep duration representation
   * @tparam Period duration period
   * @param d maximum time to wait (blocking) for resource
   * @return true if resource acquired, false if specified time elapsed and unable to acquire resource
   */
  template<class Rep, class Period>
  bool wait_for(const std::chrono::duration<Rep, Period>& d) {
    std::unique_lock<Mutex> lock{mutex_};
    bool finished = cv_.wait_for(lock, d, [&]{ return count_ > 0; });
    if (finished) {
      --count_;
    }

    return finished;
  };

  /**
   * Attempt to acquire a resource until a specified time
   * @tparam Clock clock representation
   * @tparam Duration duration type
   * @param t latest time until which to wait for a resource
   * @return true if resource acquired, false if specified time is reached without acquiring resource
   */
  template<class Clock, class Duration>
  bool wait_until(const std::chrono::time_point<Clock, Duration>& t) {
    std::unique_lock<Mutex> lock{mutex_};
    auto finished = cv_.wait_until(lock, t, [&]{ return count_ > 0; });
    if (finished) {
      --count_;
    }
    return finished;
  };

  native_handle_type native_handle() {
    return cv_.native_handle();
  }

 private:
  Mutex   mutex_;
  CondVar cv_;
  size_t  count_;
};

/**
 * Default semaphore implementation
 */
using semaphore = basic_semaphore<std::mutex, std::condition_variable>;

template <typename SemaphoreType>
class semaphore_guard {
  SemaphoreType& sem_;
 public:
  semaphore_guard(SemaphoreType& sem) : sem_{sem} {
    sem_.wait();
  }

  ~semaphore_guard() {
    sem_.notify();
  }

  // do not allow copying or moving
  semaphore_guard(const semaphore_guard&) = delete;
  semaphore_guard(semaphore_guard&&) = delete;
  semaphore_guard& operator=(const semaphore_guard&) = delete;
  semaphore_guard& operator=(semaphore_guard&&) = delete;
  
};

} // thread
} // cpen333

#endif //CPEN333_THREAD_SEMAPHORE_H
