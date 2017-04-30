#ifndef CPEN333_PROCESS_EVENT_H
#define CPEN333_PROCESS_EVENT_H

// suffix to append to mutex names for uniqueness
#define EVENT_NAME_SUFFIX "ev"

#include <string>
#include <chrono>
#include <condition_variable>

#include "cpen333/process/impl/condition_base.h"
#include "cpen333/process/mutex.h"

namespace cpen333 {
namespace process {

class event : private condition_base {
 public:

  event(const std::string &name) :
      condition_base{name + std::string(EVENT_NAME_SUFFIX)},
      mutex_{name + std::string(EVENT_NAME_SUFFIX)} {}

  // disable copy/move constructors
  event(const event&) = delete;
  event(event&&) = delete;
  event& operator=(const event&) = delete;
  event& operator=(event&&) = delete;

  void wait() {
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    condition_base::wait(lock);
  }

  template<class Rep, class Period>
  std::cv_status wait_for(const std::chrono::duration<Rep, Period>& rel_time) {
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    return condition_base::wait_for(lock, rel_time);
  }

  template< class Clock, class Duration >
  std::cv_status wait_until( const std::chrono::time_point<Clock, Duration>& timeout_time ) {
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    return condition_base::wait_until(lock, timeout_time);
  }

  void notify_one() {
    condition_base::notify_one();
  }

  void notify_all() {
    condition_base::notify_all();
  }

  virtual bool unlink() {
    bool b1 = condition_base::unlink();
    bool b2 = mutex_.unlink();
    return (b1 && b2);
  }

  static bool unlink(const std::string& name) {
    bool b1 = cpen333::process::condition_base::unlink(name + std::string(EVENT_NAME_SUFFIX));
    bool b2 = cpen333::process::mutex::unlink(name + std::string(EVENT_NAME_SUFFIX));
    return b1 && b2;
  }

 private:
  cpen333::process::mutex mutex_;

};

} // process
} // cpen333

#endif //CPEN333_PROCESS_EVENT_H
