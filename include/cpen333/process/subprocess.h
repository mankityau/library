#ifndef CPEN333_PROCESS_SUBPROCESS_H
#define CPEN333_PROCESS_SUBPROCESS_H

// Platform-dependent includes
#include "cpen333/os.h"

#ifdef WINDOWS
#include "cpen333/process/impl/windows/subprocess.h"
#else
#include "cpen333/process/impl/posix/subprocess.h"
#endif

#endif //CPEN333_SUBPROCESS_H
