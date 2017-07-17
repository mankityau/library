/**
 * @file
 * @brief Named-resource wrapper for RAII-style unlinking of the resource name
 */
#ifndef CPEN333_PROCESS_UNLINKER_H
#define CPEN333_PROCESS_UNLINKER_H

#include "../os.h" // identify OS

#ifdef WINDOWS
#include "impl/windows/unlinker.h"
#else
#include "impl/posix/unlinker.h"
#endif

/**
 * @class cpen333::process::unlinker
 * @brief A named-resource wrapper that provides a convenient RAII-style unlinking of the resource name
 *
 * Used to ensure that a resource's name is unlinked when the unlinker object drops out of scope.  On POSIX systems,
 * this can help guarantee that the lifetime of a resource will not persist beyond the lifetime of the controlling process.
 * This is an alias to either cpen333::process::posix::unlinker or cpen333::process::windows::unlinker
 * depending on your platform.
 */

#endif //CPEN333_PROCESS_UNLINKER_H
