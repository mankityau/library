//
// Created by Antonio on 2017-04-15.
//

#ifndef CPEN333_PROCESS_SEMAPHORE_GUARD_H
#define CPEN333_PROCESS_SEMAPHORE_GUARD_H

namespace cpen333 {
namespace process {

/**
 * Protects a semaphore's wait/notify using RAII to ensure all resources
 * are returned to the system
  * @tparam Semaphore basic semaphore that supports wait() and notify()
  */
template<typename Semaphore>
class semaphore_guard {
  Semaphore& sem_;
 public:
  semaphore_guard(Semaphore& sem) : sem_(sem) {
    sem_.wait();
  }

  // disable copy/move constructors
  semaphore_guard(const semaphore_guard&) = delete;
  semaphore_guard(semaphore_guard&&) = delete;
  semaphore_guard& operator=(const semaphore_guard&) = delete;
  semaphore_guard& operator=(semaphore_guard&&) = delete;

  ~semaphore_guard() {
    sem_.notify();
  }
};

} // process
} // cpen333

#endif //CPEN333_PROCESS_SEMAPHORE_GUARD_SEMAPHORE_H
