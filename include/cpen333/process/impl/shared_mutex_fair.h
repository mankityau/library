#ifndef CPEN333_PROCESS_SHARED_MUTEX_FAIR_H
#define CPEN333_PROCESS_SHARED_MUTEX_FAIR_H

#define SHARED_MUTEX_FAIR_NAME_SUFFIX "_smf"
#define SHARED_MUTEX_FAIR_INITIALIZED 0x91271238

#include "cpen333/process/mutex.h"
#include "cpen333/process/condition_variable.h"
#include "cpen333/process/shared_memory.h"
#include "cpen333/process/named_resource.h"

namespace cpen333 {
namespace process {

namespace impl {

// A more fair shared mutex, access is granted in batches: 1 writer, batch of readers, 1 writer, batch of readers
// Based on the alternating method described here:
//     http://www.tools-of-computing.com/tc/CS/Monitors/AlternatingRW.htm
class shared_mutex_fair : public virtual named_resource {
 protected:

  struct shared_data {
    size_t shared[2];   // readers or queued
    char this_batch;    // index within shared of current batch sharing access
    char next_batch;    // index within shared of next batch to push, 1-this_batch
    char exclusive;     // # exclusive access, 0 or 1
    size_t etotal;      // waiting and exclusive acces
    size_t initialized;
  };

  cpen333::process::mutex mutex_;               // mutex for state access
  cpen333::process::condition_variable econd_;  // exclusive condition
  cpen333::process::shared_object<shared_data> state_;     // shared counter object

 public:
  shared_mutex_fair(const std::string &name) :
      mutex_{name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX)},
      econd_{name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX)},
      state_{name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX)} {

    // initialize storage
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    if (state_->initialized != SHARED_MUTEX_FAIR_INITIALIZED) {
      state_->shared[0] = 0;
      state_->shared[1] = 0;
      state_->this_batch = 0;
      state_->next_batch = 1;
      state_->exclusive = 0;
      state_->etotal = 0;
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
    if (state_->etotal == 0) {
      // let him pass
      ++(state_->shared[(size_t)(state_->this_batch)]);
    } else {
      // wait until I am told to go by end of exclusive lock
      size_t batch = 1-state_->this_batch;  // next batch
      ++(state_->shared[batch]);
      econd_.wait(lock, [&](){ return (size_t)(state_->this_batch) == batch; });
    }
  }

  bool try_lock_shared() {
    std::unique_lock<cpen333::process::mutex> lock(mutex_, std::defer_lock);
    if (!lock.try_lock()) {
      return false;
    }

    if (state_->exclusive > 0) {
      return false;
    }
    return true;
  }

  void unlock_shared() {
    std::unique_lock<cpen333::process::mutex> lock(mutex_);
    if (--(state_->shared[(size_t)(state_->this_batch)]) == 0) {
      econd_.notify_all();     // tell others to check their conditions
    }
  }

  void lock() {
    std::unique_lock<cpen333::process::mutex> lock(mutex_);
    // one more waiting
    ++(state_->etotal);
    // wait until there are no readers or writers
    econd_.wait(lock, [&](){ return state_->exclusive == 0 && state_->shared[(size_t)(state_->this_batch)] == 0; });
    state_->exclusive = 1;  // exclusive lock on
  }

  bool try_lock() {
    std::unique_lock<cpen333::process::mutex> lock(mutex_, std::defer_lock);
    if (!lock.try_lock()) {
      return false;
    }

    // check if we can go immediately
    if (state_->shared[(size_t)(state_->this_batch)]+state_->exclusive > 0) {
      return false;
    }

    // we got through
    ++(state_->etotal);
    state_->exclusive = 1;  // exclusive lock on
    return true;
  }

  void unlock() {
    std::lock_guard<cpen333::process::mutex> lock(mutex_);
    state_->exclusive = 0;
    --state_->etotal;
    // send through next batch and notify
    state_->this_batch = 1-state_->this_batch;
    econd_.notify_all();
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
  }

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

    // one more waiting
    ++(state_->etotal);
    // wait until there are no readers or writers
    if (!econd_.wait_until(lock, timeout_time,
                           [&](){ return state_->shared[(size_t)(state_->this_batch)]+state_->exclusive == 0; })) {
      // timed out, undo wait
      --(state_->etotal);
      return false;
    }

    state_->exclusive = 1;  // exclusive lock on
    return true;
  }

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
  }

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

    if (state_->etotal == 0) {
      // let him pass
      ++(state_->shared[(size_t)(state_->this_batch)]);
    } else {
      // wait until I am told to go by end of exclusive lock
      size_t batch = 1-state_->this_batch;  // next batch
      ++(state_->shared[batch]);
      if (!econd_.wait_until(lock, timeout_time,
                             [&](){ return state_->this_batch == batch; })) {
        // undo queued reader
        --(state_->shared[batch]);
        return false;
      }
    }
    return true;
  }

  bool unlink() {
    bool b1 = mutex_.unlink();
    bool b2 = econd_.unlink();
    bool b3 = state_.unlink();
    return (b1 && b2 && b3);
  }

  static bool unlink(const std::string& name) {
    bool b1 = cpen333::process::mutex::unlink(name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX));
    bool b2 = cpen333::process::condition_variable::unlink(name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX));
    bool b3 = cpen333::process::shared_object<shared_data>::unlink(name + std::string(SHARED_MUTEX_FAIR_NAME_SUFFIX));
    return (b1 && b2 && b3);
  }

};

} // impl

using shared_mutex_fair = impl::shared_mutex_fair;
using shared_timed_mutex_fair = impl::shared_mutex_fair;

} // process
} // cpen333

#endif //CPEN333_PROCESS_SHARED_MUTEX_FAIR_H
