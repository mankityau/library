#ifndef CPEN333_THREAD_THREAD_OBJECT_H
#define CPEN333_THREAD_THREAD_OBJECT_H

#include <thread>

namespace cpen333 {
namespace thread {

class thread_object {
 private:
  std::thread* thread_;        // underlying thread object
  volatile int result_;

 public:
  // pass "this.run()" to thread
  thread_object() : thread_{nullptr}, result_{0} {}

  ~thread_object() {
    // free up thread
    if (thread_ != nullptr) {
      delete thread_;
    }
  }

  /**
   * Start thread execution
   */
  void start() {
    if (thread_ == nullptr) {
      thread_ = new std::thread(&thread_object::__run, this);
    }
  }

  /**
   * Waits for thread to finish executing.  If thread hasn't been started yet, then it is started here.
   * @return the result of main()
   */
  int join() {
    if (thread_ == nullptr) {
      start();
    }
    if (thread_->joinable()) {
      thread_->join();            // wait for parent thread to join, ensures result_ is set
    }
    return result_;
  }

 protected:

  // pure virtual method
  virtual int main() = 0;

  // private non-virtual internal method that calls main
  void __run() {
    result_ = main();
  }

 private:
  void run() {
    result_ = main();
  }
};

} // thread
} // cpen333

#endif // CPEN333_THREAD_THREAD_OBJECT_H