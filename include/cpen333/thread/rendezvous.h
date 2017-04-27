#ifndef CPEN333_THREAD_RENDEZVOUS_H
#define CPEN333_THREAD_RENDEZVOUS_H

#include <mutex>
#include <condition_variable>

namespace cpen333 {
namespace thread {

// Inter-thread rendezvous class
class rendezvous {
  std::mutex mutex_;
  std::condition_variable cv_;
  size_t countdown_;
  size_t countup_;
  size_t size_;

 public:
  rendezvous(size_t size) : mutex_{}, countdown_{size}, countup_{0}, size_{size} {}

  // Cause threads to wait until all arrive
  void wait() {
    // lock to protect access to data
    std::unique_lock<std::mutex> lock(mutex_);

    //check if we are done
    if (countdown_ <= 1) {
      // release all threads
      countdown_ = 0;
      cv_.notify_all();
    } else {
      --countdown_;

      // wait until everybody has arrived
      // We use a simple lambda to check condition
      cv_.wait(lock, [&](){return countdown_ == 0;});
    }

    // reset count upwards
    ++countup_;

    // when last thread exists method, reset countdown_
    if (countup_ == size_) {
      countdown_ = size_;
      countup_ = 0;
    }
  }
};

} // thread
} // cpen333

#endif //CPEN333_THREAD_RENDEZVOUS_H
