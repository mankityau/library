#ifndef CPEN333_THREAD_SUSPENDED_THREAD_H
#define CPEN333_THREAD_SUSPENDED_THREAD_H

#include <thread>
#include <functional>
#include <condition_variable>
#include <mutex>

namespace cpen333 {
namespace thread {

class suspended_thread : public std::thread {
 private:
  std::function<void()> func_;  // delayed function

  // internal for suspending
  std::mutex mutex_;
  std::condition_variable cv_;
  bool start_;                  // signal to start thread

  // wrapper to call func_ once initialized
  void run_() {
    // wait for start_ to be set to true
    std::unique_lock<std::mutex> lk(mutex_);
    cv_.wait(lk, [this]{ return start_; });

    // call delayed function
    func_();
  }

 public:
  template<typename Func, typename ... Args>
  suspended_thread(Func &&func, Args &&... args) :
    std::thread(&suspended_thread::run_, this), func_ (std::bind(std::forward<Func>(func), std::forward<Args>(args)...)),
    mutex_(), cv_(), start_(false){}

  // move constructor, required due to unmovable mutex and condition_variable
  suspended_thread(suspended_thread&& mv) : func_(std::move(mv.func_)), mutex_(), cv_(), start_(false){}

  void start() {
    std::unique_lock<std::mutex> lk(mutex_);
    start_ = true;
    cv_.notify_one();
  }

};

} // thread
} // cpen333

#endif // CPEN333_THREAD_SUSPENDED_THREAD_H