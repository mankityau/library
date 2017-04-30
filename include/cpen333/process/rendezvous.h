#ifndef CPEN333_PROCESS_RENDEZVOUS_H
#define CPEN333_PROCESS_RENDEZVOUS_H

#define RENDEZVOUS_NAME_SUFFIX "rv"
#define RENDEZVOUS_INITIALIZED 0x38973823

#include "cpen333/process/impl/named_resource.h"
#include "cpen333/process/shared_memory.h"
#include "cpen333/process/semaphore.h"
#include "cpen333/process/mutex.h"

namespace cpen333 {
namespace process {

class rendezvous : public named_resource {

 public:
  rendezvous(const std::string &name, size_t size) :
      named_resource{name + std::string(RENDEZVOUS_NAME_SUFFIX)},
      shared_{name + std::string(RENDEZVOUS_NAME_SUFFIX)},
      semaphore_{name + std::string(RENDEZVOUS_NAME_SUFFIX), {0}},
      mutex_{name + std::string(RENDEZVOUS_NAME_SUFFIX)}{

    // initialize data
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    if (shared_->initialized != RENDEZVOUS_INITIALIZED) {
      shared_->size = size;
      shared_->count = size;
      shared_->initialized = RENDEZVOUS_INITIALIZED;
    }

  }

  void wait() {
    // lock shared data to prevent concurrent modification
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    if (shared_->count == 0) {
      return;  // no need to wait
    }

    if (--(shared_->count) == 0) {
      // reset count
      shared_->count = shared_->size;
      // release size-1 (since we are the last to arrive)
      int release = (shared_->size)-1;
      for (int i=0; i<release; ++i) {
        semaphore_.notify();
      }
    } else {
      // unlock shared data and wait
      lock.unlock();
      semaphore_.wait();
    }
  }

  bool unlink() {
    bool b1 = shared_.unlink();
    bool b2 = semaphore_.unlink();
    bool b3 = mutex_.unlink();
    return b1 && b2 && b3;
  }

  static bool unlink(const std::string& name) {

    bool b1 = cpen333::process::shared_object<shared_data>::unlink(name + std::string(RENDEZVOUS_NAME_SUFFIX));
    bool b2 = cpen333::process::semaphore::unlink(name + std::string(RENDEZVOUS_NAME_SUFFIX));
    bool b3 = cpen333::process::mutex::unlink(name + std::string(RENDEZVOUS_NAME_SUFFIX));

    return b1 && b2 && b3;
  }

 private:

  struct shared_data {
    size_t size;
    size_t count;
    int initialized;
  };
  cpen333::process::shared_object<shared_data> shared_;
  cpen333::process::semaphore semaphore_;
  cpen333::process::mutex mutex_;

};

} // process
} // cpen333

#endif //CPEN333_PROCESS_RENDEZVOUS_H
