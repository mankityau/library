#ifndef CPEN333_THREAD_TIMER_H
#define CPEN333_THREAD_TIMER_H

#include <functional>
#include <chrono>
#include <thread>
#include <condition_variable>

namespace cpen333 {
namespace thread {

namespace detail {

struct void_function_t {
  operator () () const {}
};

constexpr const void_function_t void_function;  // constant instance

}

// time-based timer
template<typename Duration>
class timer {
 public:

  template<typename Func, typename...Args>
  timer(const Duration& time) : timer(time, detail::void_function) {}

  template<typename Func, typename...Args>
  timer(const Duration& time, Func &&func, Args &&... args) :
    time_{time}, ring_{false}, run_{false}, terminate_{false},
    callback_{std::bind(std::forward<Func>(func), std::forward<Args>(args)...)},
    mutex_{}, cv_{},
    thread_{&timer::run, this} {
  }

  /**
   * Start timer running, resets clock to zero and "test" flag
   */
  void start() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    run_ = true;    // start your races
    ring_ = false;  // turn off last ring
    cv_.notify_all();
  }

  /**
   * Stops timer running, leaves "test" flag in tact to see if it has gone off
   */
  void stop() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    run_ = false;  // stop running
    cv_.notify_all();
  }

  /**
   * Checks if timer is running
   * @return true if running
   */
  bool running() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    return run_;
  }
  
  void wait() {
    // wait until next event
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    cv_.wait(lock);
  }

  /**
   * Tests if timer has gone off since last reset
   * @return true if timer has gone off
   */
  bool test() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    return ring_;
  }

  /**
   * Test if timer has gone off since last call, resets flag
   * @return true if timer has gone off since last call
   */
  bool test_and_reset() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    if (ring_) {
      ring_ = false;
      return true;
    }
    return false;
  }

  ~timer() {
    // signal thread to terminate
    {
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      terminate_ = true;
      cv_.notify_all();
    }
    // detach thread to let it die
    thread_.detach();
  }

 private:

  void run() {

    // wait for first start or until terminated
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [&](){return run_ || terminate_;});

    auto tick_ = std::chrono::steady_clock::now() + time_;
    while(!terminate_) {
      // wait until awoken on purpose, or time-out
      if (cv_.wait_until(lock, tick_,
                     [&](){ return !run_ || terminate_; })) {

        // we've hit stop, wait again until start is hit
        cv_.wait(lock, [&](){return run_ || terminate_;});
        std::chrono::steady_clock::now() + time_;
      } else {
        // timeout, run callback
        tick_ += time_;   // set up next tick immediately
        callback_();      // call function
        ring_ = true;
        cv_.notify_all();  // let everyone know condition went off
      }
    }

  }

  Duration time_;
  bool ring_;
  bool run_;
  bool terminate_;         // signal to terminate
  std::function<void()> callback_;  // callback
  std::mutex mutex_;                // controls terminate
  std::condition_variable cv_;      // controls waiting
  std::thread thread_;              // timer thread
};

} // thread
} // cpen333

#endif //CPEN333_THREAD_TIMER_H
