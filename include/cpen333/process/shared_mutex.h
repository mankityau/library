#ifndef CPEN333_PROCESS_SHARED_MUTEX_H
#define CPEN333_PROCESS_SHARED_MUTEX_H

#include <shared_mutex>
#include "cpen333/process/mutex.h"
#include "cpen333/process/semaphore.h"
#include "cpen333/process/shared_memory.h"
#include "cpen333/process/impl/shared_mutex_exclusive.h"
#include "cpen333/process/impl/shared_mutex_fair.h"
#include "cpen333/process/impl/shared_mutex_shared.h"

namespace cpen333 {
namespace process {


using shared_mutex = impl::shared_mutex_fair;
using shared_timed_mutex = impl::shared_mutex_fair;

template<typename SharedMutex>
class shared_lock_guard {
  SharedMutex& mutex_;
 public:
  shared_lock_guard(SharedMutex& mutex) : mutex_{mutex} {
    mutex_.lock_shared();
  }

  // disable copy/move constructors
  shared_lock_guard(const shared_lock_guard&) = delete;
  shared_lock_guard(shared_lock_guard&&) = delete;
  shared_lock_guard& operator=(const shared_lock_guard&) = delete;
  shared_lock_guard& operator=(shared_lock_guard&&) = delete;
  
  ~shared_lock_guard() {
    mutex_.unlock_shared();
  }
};

} // process
} // cpen333

#endif //CPEN333_PROCESS_SHARED_MUTEX_H
