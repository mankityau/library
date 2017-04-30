#ifndef CPEN333_PROCESS_CONDITION_VARIABLE_H
#define CPEN333_PROCESS_CONDITION_VARIABLE_H

// suffix to append to mutex names for uniqueness
#define CONDITION_VARIABLE_NAME_SUFFIX "cv"

#include <string>
#include <chrono>

#include "cpen333/process/impl/condition_base.h"

namespace cpen333 {
namespace process {

// a condition with optional predicate to control wait
class condition_variable : public condition_base {

 public:
  condition_variable(const std::string &name) :
      condition_base{name + std::string(CONDITION_VARIABLE_NAME_SUFFIX)} {}

  void wait(std::unique_lock<cpen333::process::mutex>& lock) {
    return condition_base::wait(lock);
  }

  template<class Rep, class Period>
  std::cv_status wait_for( std::unique_lock<cpen333::process::mutex>& lock,
                           const std::chrono::duration<Rep, Period>& rel_time) {
    return condition_base::wait_for(lock, rel_time);
  }

  template<class Clock, class Duration >
  bool wait_until( std::unique_lock<cpen333::process::mutex>& lock,
                   const std::chrono::time_point<Clock, Duration>& timeout_time ) {
    return condition_base::wait(lock, timeout_time);
  }

  template<typename Predicate>
  void wait(std::unique_lock<cpen333::process::mutex>& lock, Predicate pred) {
    while (!pred()) {
      condition_base::wait(lock, false, std::chrono::steady_clock::now());
    }
  }

  template<class Rep, class Period, class Predicate>
  bool wait_for( std::unique_lock<cpen333::process::mutex>& lock,
                 const std::chrono::duration<Rep, Period>& rel_time,
                 Predicate pred) {
    return wait_until(lock, std::chrono::steady_clock::now()+rel_time, pred);
  }

  template< class Clock, class Duration, class Predicate >
  bool wait_until( std::unique_lock<cpen333::process::mutex>& lock,
                   const std::chrono::time_point<Clock, Duration>& timeout_time,
                   Predicate pred ) {
    while (!pred()) {
      if (condition_base::wait_until(lock, timeout_time)) {
        return pred();
      }
    }
    return true;
  }

  bool unlink() {
    return condition_base::unlink();
  }

  static bool unlink(const std::string& name) {
    return cpen333::process::condition_base::unlink(name + std::string(CONDITION_VARIABLE_NAME_SUFFIX));
  }

};

} // process
} // cpen333

#endif //CPEN333_PROCESS_CONDITION_VARIABLE_H
