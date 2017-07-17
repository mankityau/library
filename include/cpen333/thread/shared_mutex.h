/**
 * @file
 * @brief Shared-access mutex implementations, allowing for multi-read/write access
 */
#ifndef CPEN333_THREAD_SHARED_MUTEX_H
#define CPEN333_THREAD_SHARED_MUTEX_H

#include <shared_mutex>
#include "impl/shared_mutex_shared.h"
#include "impl/shared_mutex_exclusive.h"

namespace cpen333 {
namespace thread {

/**
 * @brief Default shared mutex that uses fair priority
 */
using shared_mutex_fair = std::shared_timed_mutex;

/**
 * @brief Default shared timed mutex that uses fair priority
 */
using shared_timed_mutex_fair = std::shared_timed_mutex;

/**
 * @brief Default shared mutex that uses fair priority
 */
using shared_mutex = std::shared_timed_mutex;

/**
 * @brief Default shared timed mutex that uses fair priority
 */
using shared_timed_mutex = std::shared_timed_mutex;

}  // thread
}  // cpen333

#endif //CPEN333_THREAD_SHARED_MUTEX_H
