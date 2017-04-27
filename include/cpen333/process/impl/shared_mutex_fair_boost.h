#ifndef CPEN333_PROCESS_SHARED_MUTEX_FAIR_H
#define CPEN333_PROCESS_SHARED_MUTEX_FAIR_H

#define SHARED_MUTEX_FAIR_NAME_SUFFIX "__shared_mutex_fair__"
#define SHARED_MUTEX_FAIR_SHARED_SUFFIX "__shared_mutex_fair_s__"
#define SHARED_MUTEX_FAIR_EXCLUSIVE_SUFFIX "__shared_mutex_fair_e__"
#define SHARED_MUTEX_FAIR_INITIALIZED 0x91271238

#include "cpen333/process/mutex.h"
#include "cpen333/process/condition_variable.h"
#include "cpen333/process/shared_memory.h"

namespace cpen333 {
namespace process {

namespace impl {

// A more fair shared mutex, access is granted approximately in order of arrival
// Based on boost's implementation found here:
// http://www.boost.org/doc/libs/1_63_0/boost/thread/pthread/shared_mutex.hpp
class shared_mutex_fair : named_resource {
 protected:

  struct shared_data {
    size_t shared;
    bool exclusive;
    bool blocked;
    int initialized;

    bool can_lock() const {
      return (shared == 0) && !exclusive;
    }

    void lock() {
      exclusive = true;
    }

    void unlock() {
      exclusive = false;
      blocked = false;
    }

    void block(bool set) {
      blocked = set;
    }

    bool can_lock_shared() const {
      return !exclusive && !blocked;
    }

    bool has_shared() const {
      return shared > 0;
    }

    size_t lock_shared() {
      return ++shared;
    }

    size_t unlock_shared() {
      return --shared;
    }

  };

  cpen333::process::mutex mutex_;               // mutex for state access
  cpen333::process::condition_variable scond_;  // shared condition
  cpen333::process::condition_variable econd_;  // exclusive condition
  cpen333::process::shared_object<shared_data> state_;     // shared counter object

 public:
  shared_mutex_fair(const std::string &name) :
      named_resource{name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX)},
      mutex_{name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX)},
      scond_{name + std::string(SHARED_MUTEX_FAIR_SHARED_SUFFIX)},
      econd_{name + std::string(SHARED_MUTEX_FAIR_EXCLUSIVE_SUFFIX)},
      state_{name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX)} {

    // initialize storage
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    if (state_->initialized != SHARED_MUTEX_FAIR_INITIALIZED) {
      state_->shared = 0;
      state_->exclusive = false;
      state_->blocked = false;
      state_->initialized = SHARED_MUTEX_FAIR_INITIALIZED;
    }
  }

  // disable copy/move constructors
  shared_mutex_fair(const shared_mutex_fair &) = delete;
  shared_mutex_fair(shared_mutex_fair &&) = delete;
  shared_mutex_fair &operator=(const shared_mutex_fair &) = delete;
  shared_mutex_fair &operator=(shared_mutex_fair &&) = delete;

  void lock_shared() {
    std::unique_lock<cpen333::process::mutex> lock(mutex_);
    while (!state_->can_lock_shared()) {
      scond_.wait(lock);                  // gate 1
    }
    state_->lock_shared();
  }

  bool try_lock_shared() {
    std::unique_lock<cpen333::process::mutex> lock(mutex_, std::defer_lock); // do not try yet
    if (!lock.try_lock()) {
      return false;
    }

    if (!state_->can_lock_shared()) {
      return false;
    }
    state_->lock_shared();
    return true;
  }

  void unlock_shared() {
    std::unique_lock<cpen333::process::mutex> lock(mutex_);
    state_->unlock_shared();
    if (!state_->has_shared()) {
      state_->block(false);
      econd_.notify_one();
      scond_.notify_all();
    }
  }

  void lock() {
    std::unique_lock<cpen333::process::mutex> lock(mutex_); // prevent readers from passing me, since they need this
    while (!state_->can_lock()) {
      state_->block(true);
      econd_.wait(lock);
    }
    state_->lock();
  }

  bool try_lock() {
    // prevent readers from passing me, since they need this
    std::unique_lock<cpen333::process::mutex> lock(mutex_, std::defer_lock);
    if (!lock.try_lock()) {
      return false;
    }

    if (!state_->can_lock()) {
      return false;
    }

    state_->lock();
    return true;
  }

  void unlock() {
    std::lock_guard<cpen333::process::mutex> lock(mutex_);
    state_->unlock();
    econd_.notify_one();
    scond_.notify_all();
  }

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable for the specified timeout duration
   * @tparam Rep duration representation
   * @tparam Period duration period
   * @param timeout_duration timeout
   * @return true if locked successfully
   */
  template<class Rep, class Period>
  bool try_lock_for(const std::chrono::duration<Rep, Period> &timeout_duration) {
    return try_lock_until(std::chrono::steady_clock::now() + timeout_duration);
  };

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable until specified time point has been reached
   * @tparam Clock clock representation
   * @tparam Duration time
   * @param timeout_time time of timeout
   * @return true if locked successfully
   */
  template<class Clock, class Duration>
  bool try_lock_until(const std::chrono::time_point<Clock, Duration> &timeout_time) {
    // prevent readers from passing me, since they need this
    std::unique_lock<cpen333::process::mutex> lock(mutex_, std::defer_lock);
    if (!lock.try_lock_until(timeout_time)) {
      return false;
    }

    while (!state_->can_lock()) {
      state_->block(true);
      if (!econd_.wait_until(lock, timeout_time)) {
        if (!state_->can_lock()) {
          state_->block(false);
          econd_.notify_one();
          scond_.notify_all();
          return false;
        }
        break;
      }
    }

    state_->lock();
    return true;
  };

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable for the specified timeout duration
   * @tparam Rep duration representation
   * @tparam Period duration period
   * @param timeout_duration timeout
   * @return true if locked successfully
   */
  template<class Rep, class Period>
  bool try_lock_shared_for(const std::chrono::duration<Rep, Period> &timeout_duration) {
    return try_lock_shared_until(std::chrono::steady_clock::now() + timeout_duration);
  };

  /**
   * tries to lock the mutex, returns if the mutex has been unavailable until specified time point has been reached
   * @tparam Clock clock representation
   * @tparam Duration time
   * @param timeout_time time of timeout
   * @return true if locked successfully
   */
  template<class Clock, class Duration>
  bool try_lock_shared_until(const std::chrono::time_point<Clock, Duration> &timeout_time) {
    std::unique_lock<cpen333::process::mutex> lock(mutex_, std::defer_lock); // do not try yet
    if (!lock.try_lock_until(timeout_time)) {
      return false;
    }

    // locked, so safe to read/write to count
    while (!state_->can_lock_shared()) {
      if (!scond_.wait_until(lock, timeout_time)) {
        if (!state_->can_lock_shared()) {
          return false;
        }
        break;
      }
    }
    state_->lock_shared();
    return true;
  };

  bool unlink() {
    bool b1 = mutex_.unlink();
    bool b2 = econd_.unlink();
    bool b3 = scond_.unlink();
    bool b4 = state_.unlink();
    return (b1 && b2 && b3 && b4);
  }

  static bool unlink(const std::string& name) {
    bool b1 = cpen333::process::mutex::unlink(name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX));
    bool b2 = cpen333::process::condition_variable::unlink(name + std::string(SHARED_MUTEX_FAIR_SHARED_SUFFIX));
    bool b3 = cpen333::process::condition_variable::unlink(name + std::string(SHARED_MUTEX_FAIR_EXCLUSIVE_SUFFIX));
    bool b4 = cpen333::process::shared_object<shared_data>::unlink(name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX));
    return (b1 && b2 && b3 && b4);
  }

};

} // impl

using shared_mutex_fair = impl::shared_mutex_fair;
using shared_timed_mutex_fair = impl::shared_mutex_fair;

} // process
} // cpen333

#endif //CPEN333_PROCESS_SHARED_MUTEX_FAIR_H
