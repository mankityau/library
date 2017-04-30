#ifndef CPEN333_THREAD_CONDITION_H
#define CPEN333_THREAD_CONDITION_H

#include <mutex>
#include <condition_variable>

namespace cpen333 {
namespace thread {

// condition class using a condition_variable
class condition {

 public:
  // start with gate closed
  condition(bool open = false) : open_{open}, cv_{}, mutex_{} {}

  void wait() {
    // wait on condition variable until gate is open
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    cv_.wait(lock, [&](){return open_;});
  }

  template<class Rep, class Period>
  bool wait_for(const std::chrono::duration<Rep, Period>& rel_time) {
    return wait_until(std::chrono::steady_clock::now()+rel_time);
  }

  template< class Clock, class Duration >
  bool wait_until( const std::chrono::time_point<Clock, Duration>& timeout_time ) {
    std::unique_lock<decltype(mutex_)> lock(mutex_, std::defer_lock);
    if (!lock.try_lock_until(timeout_time)) {
      return false;
    }
    return cv_.wait_until(lock, timeout_time, [&](){return open_;});
  }

  void notify() {
    // protect data and open gate
    {
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      open_ = true;
    }
    // notify all that gate is now open
    cv_.notify_all();
  }

  // close gate
  void reset() {
    // protect data and close gate
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    open_ = false;
  }

 private:
  bool open_;  // gate
  std::condition_variable_any cv_;
  std::timed_mutex mutex_;
};

} // thread
} // cpen333

#endif //CPEN333_THREAD_CONDITION_H
