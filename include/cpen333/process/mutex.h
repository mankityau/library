#ifndef CPEN333_PROCESS_MUTEX_H
#define CPEN333_PROCESS_MUTEX_H

#include <mutex>  // for locks

#include "cpen333/os.h"
#ifdef WINDOWS
#include "cpen333/process/impl/windows/mutex.h"
#else
#include "cpen333/process/impl/posix/mutex.h"
#endif

#endif //CPEN333_PROCESS_MUTEX_H
