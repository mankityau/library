/**
 * @file
 * @brief Inter-process shared-access mutex implementations, allowing for multi-read/write access
 */
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

/**
 * @brief Default shared mutex that uses fair priority
 */
using shared_mutex = impl::shared_mutex_fair;

/**
 * @brief Default shared timed mutex uses fair priority
 */
using shared_timed_mutex = impl::shared_mutex_fair;

/**
 * @class cpen333::process::shared_mutex
 * @brief An inter-process mutual exclusion synchronization primitive allowing for shared access
 *
 * Used to protect access to a resource shared by multiple processes, allowing shared
 * `read' access, mirroring std::shared_mutex in c++17.  This is an alias to
 * cpen333::process::impl::shared_mutex_fair.
 */

/**
 * @class cpen333::process::shared_timed_mutex
 * @brief An inter-process mutual exclusion synchronization primitive allowing for shared access with timeouts
 *
 * Used to protect access to a resource shared by multiple processes, allowing shared
 * `read' access, mirroring std::shared_timed_mutex in c++14.  This is an alias to
 * cpen333::process::impl::shared_mutex_fair.
 */

/**
 * @brief Shared lock guard, similar to std::lock_guard but for shared locks
 * @tparam SharedMutex  shared mutex type
 */
template<typename SharedMutex>
class shared_lock_guard {
  SharedMutex& mutex_;
 public:

  /**
   * @brief Construct the shared lock guard
   * @param mutex  mutex to lock on constructions
   */
  shared_lock_guard(SharedMutex& mutex) : mutex_{mutex} {
    mutex_.lock_shared();
  }

  // disable copy/move constructors
  shared_lock_guard(const shared_lock_guard&) = delete;
  shared_lock_guard(shared_lock_guard&&) = delete;
  shared_lock_guard& operator=(const shared_lock_guard&) = delete;
  shared_lock_guard& operator=(shared_lock_guard&&) = delete;

  /**
   * @brief Destructor, unlock shared mutex
   */
  ~shared_lock_guard() {
    mutex_.unlock_shared();
  }
};

} // process
} // cpen333

#endif //CPEN333_PROCESS_SHARED_MUTEX_H
