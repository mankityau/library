#ifndef CPEN333_THREAD_TIMER_H
#define CPEN333_THREAD_TIMER_H

#include <functional>  // for storing std::function<void()>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "cpen333/thread/semaphore.h"

namespace cpen333 {
namespace thread {

namespace detail {
struct noop_function_t {
  void operator () () const {}
};
constexpr const noop_function_t noop_function;  // constant instance

template<typename T>
class runner {

  T func_;
  std::mutex mutex_;
  std::condition_variable cv_;
  size_t count_;   // run count
  bool terminate_; // not accepting any more
  std::unique_ptr<std::thread> thread_;

  void run() {

    // do not run just yet
    std::unique_lock<std::mutex> lock(mutex_, std::defer_lock);
    size_t cc = 0;  // number of times run
    
    while(!terminate_) {
      // wait until terminate or one more item
      lock.lock();
      {
        // reduce count by number of times we have since ran func_()
        count_ -= cc;
        cv_.wait(lock, [&]() { return terminate_ || count_ > 0; });
        cc = count_;    // current count is the # of times to run func_() now
      }
      lock.unlock();  // relinquish lock
      
      // call function cc times in a row, outside of lock so notify() will not block
      // for long-lasting functions
      for (int i=0; i<cc; ++i) {
        func_();
      }
    }
  }

 public:
  runner(T&& func) : func_{std::move(func)}, mutex_{}, cv_{}, 
                     count_{0}, terminate_{0}, thread_{} {}

  void start() {
    // spawn new thread
    thread_ = std::unique_ptr<std::thread>(new std::thread(&runner::run, this));
  }
  
  void terminate() {
    std::lock_guard<std::mutex> lock(mutex_);
    terminate_ = true;
    cv_.notify_one();
  }

  ~runner() {
    terminate();
    if (thread_.get() != nullptr) {
      thread_->join();  // need to "join", otherwise thread will refer to non-existent variables
    }
  }

  void notify() {
    { // localized lock
      std::lock_guard<std::mutex> lock(mutex_);
      ++count_;
    }
    cv_.notify_one();
  }
};

}

// time-based timer
template<typename Duration>
class timer {
 public:

  template<typename Func, typename...Args>
  timer(const Duration& time) : timer(time, detail::noop_function) {}

  template<typename Func, typename...Args>
  timer(const Duration& time, Func &&func, Args &&... args) :
    time_{time}, ring_{false}, run_{false}, terminate_{false},
    runner_{std::bind(std::forward<Func>(func), std::forward<Args>(args)...)},
    mutex_{}, cv_{},
    thread_{&timer::run, this} {
    runner_.start();  // start new thread running
  }

  /**
   * Start timer running, resets clock to zero and "test" flag
   */
  void start() {
    {
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      run_ = true;    // start your races
      ring_ = false;  // turn off last ring
    }
    cv_.notify_all();
  }

  /**
   * Stops timer running, leaves "test" flag in tact to see if it has gone off
   */
  void stop() {
    {
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      run_ = false;  // stop running
    }
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
    // let thread finish, since refers to member data
    thread_.join();
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
        std::chrono::steady_clock::now() + time_;  // start time from now

      } else {
        // timeout, run callback
        tick_ += time_;    // set up next tick immediately so no time is wasted
        ring_ = true;      // let people know timer has gone off
        cv_.notify_all();  // wake up anyone waiting for event

        // notify runner to run another instance
        runner_.notify();
      }
    }
  }

  Duration time_;
  bool ring_;
  bool run_;
  bool terminate_;         // signal to terminate
  detail::runner<std::function<void()>>   runner_;  // callback
  std::mutex mutex_;                // controls terminate
  std::condition_variable cv_;      // controls waiting
  std::thread thread_;              // timer thread
};

} // thread
} // cpen333

#endif //CPEN333_THREAD_TIMER_H
