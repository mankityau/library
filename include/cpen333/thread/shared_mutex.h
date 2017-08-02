/**
 * @file
 * @brief Shared-access mutex implementations, allowing for multi-read/write access
 */
#ifndef CPEN333_THREAD_SHARED_MUTEX_H
#define CPEN333_THREAD_SHARED_MUTEX_H

#include "impl/shared_mutex_shared.h"
#include "impl/shared_mutex_exclusive.h"

#if __cplusplus >= 201402L
#include <shared_mutex>
#else
#include "impl/shared_mutex_fair.h"
#include "impl/shared_lock.h"
#endif

namespace cpen333 {
namespace thread {

/**
 * @brief Default shared mutex that uses fair priority
 */
typedef std::shared_timed_mutex shared_mutex_fair;

/**
 * @brief Default shared timed mutex that uses fair priority
 */
typedef std::shared_timed_mutex shared_timed_mutex_fair;

/**
 * @brief Default shared mutex that uses fair priority
 */
typedef shared_mutex_fair shared_mutex;

/**
 * @brief Default shared timed mutex that uses fair priority
 */
typedef shared_mutex_fair shared_timed_mutex;

}  // thread
}  // cpen333

#endif //CPEN333_THREAD_SHARED_MUTEX_H
