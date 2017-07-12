/**
 * @file
 * @brief Inter-process shared semaphore implementation
 */
#ifndef CPEN333_PROCESS_SEMAPHORE_H
#define CPEN333_PROCESS_SEMAPHORE_H

#include "cpen333/os.h" // identify OS

#ifdef WINDOWS
#include "cpen333/process/impl/windows/semaphore.h"
#else
#ifdef APPLE
#include "cpen333/process/impl/osx/sem_timedwait.h" // missing sem_timedwait functionality
#endif
#include "cpen333/process/impl/posix/semaphore.h"
#endif

/**
 * @class cpen333::process::semaphore
 * @brief An inter-process semaphore synchronization primitive
 *
 * Used to protect access to a counted resource shared by multiple processes.  This is an alias to either
 * cpen333::process::posix::semaphore or cpen333::process::windows::semaphore depending on your platform.
 */

#include "cpen333/process/impl/semaphore_guard.h"

#endif //CPEN333_PROCESS_SEMAPHORE_H
