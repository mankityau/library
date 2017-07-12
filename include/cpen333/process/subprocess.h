/**
 * @file
 * @brief Child process implementation
 */
#ifndef CPEN333_PROCESS_SUBPROCESS_H
#define CPEN333_PROCESS_SUBPROCESS_H

// Platform-dependent includes
#include "cpen333/os.h"

#ifdef WINDOWS
#include "cpen333/process/impl/windows/subprocess.h"
#else
#include "cpen333/process/impl/posix/subprocess.h"
#endif

/**
 * @class cpen333::process::subprocess
 * @brief A child process implementation
 *
 * Used to create and run a child process.  This is an alias to either
 * cpen333::process::posix::subprocess or cpen333::process::windows::subprocess depending on your platform.
 */

#endif //CPEN333_SUBPROCESS_H
