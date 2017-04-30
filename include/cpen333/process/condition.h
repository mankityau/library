#ifndef CPEN333_PROCESS_CONDITION_H
#define CPEN333_PROCESS_CONDITION_H

// suffix to append to mutex names for uniqueness
#define CONDITION_NAME_SUFFIX "__condition__"

// magic number of testing initialization
#define CONDITION_INITIALIZED 0x87621232

#include <string>
#include <chrono>

#include "cpen333/process/impl/condition_base.h"

namespace cpen333 {
namespace process {


// an event with optional predicate to control wait
class condition : private condition_base {
 public:

  condition(const std::string &name, bool value = false) :
      condition_base{name + std::string(CONDITION_NAME_SUFFIX)},
      storage_{name + std::string(CONDITION_NAME_SUFFIX)},
      mutex_{name + std::string(CONDITION_NAME_SUFFIX)}{

    // initialize data if we need to
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    if (storage_->initialized != CONDITION_INITIALIZED) {
      storage_->value = value;
      storage_->initialized = CONDITION_INITIALIZED;
    }

  }

  void wait() {
    std::unique_lock<cpen333::process::mutex> lock(mutex_);
    while (!storage_->value) {
      condition_base::wait(lock, false, std::chrono::steady_clock::now());
    }
  }

  template<class Rep, class Period>
  bool wait_for(const std::chrono::duration<Rep, Period>& rel_time) {
    return wait_until(std::chrono::steady_clock::now()+rel_time);
  }

  template< class Clock, class Duration >
  bool wait_until( const std::chrono::time_point<Clock, Duration>& timeout_time ) {
    std::unique_lock<cpen333::process::mutex> lock(mutex_, std::defer_lock);
    if (!lock.try_lock_until(timeout_time)) {
      return false;
    }
    while (!storage_->value) {
      if (condition_base::wait_until(lock, timeout_time)) {
        return storage_->value;
      }
      if (std::chrono::steady_clock::now() > timeout_time) {
        return false;
      }
    }
    return true;
  }

  void notify() {
    // open gate
    {
      std::lock_guard<cpen333::process::mutex> lock(mutex_);
      storage_->value = true;
    }
    // wake everyone up to check the new value
    condition_base::notify(true);
  }

  void reset() {
    // reset the value
    std::lock_guard<cpen333::process::mutex> lock(mutex_);
    storage_->value = false;
  }

  virtual bool unlink() {
    bool b1 = condition_base::unlink();
    bool b2 = storage_.unlink();
    bool b3 = mutex_.unlink();
    return b1 && b2 && b3;
  }

  static bool unlink(const std::string& name) {
    bool b1 = cpen333::process::condition_base::unlink(name + std::string(CONDITION_NAME_SUFFIX));
    bool b2 = cpen333::process::shared_object<shared_data>::unlink(name + std::string(CONDITION_NAME_SUFFIX));
    bool b3 = cpen333::process::mutex::unlink(name + std::string(CONDITION_NAME_SUFFIX));
    return b1 && b2 && b3;
  }

 private:
  struct shared_data {
    bool value;
    size_t initialized;
  };
  cpen333::process::shared_object<shared_data> storage_;
  cpen333::process::mutex mutex_;

};

} // process
} // cpen333

#endif //CPEN333_PROCESS_CONDITION_H
