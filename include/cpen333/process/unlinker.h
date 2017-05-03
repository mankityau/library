#ifndef CPEN333_PROCESS_UNLINKER_H
#define CPEN333_PROCESS_UNLINKER_H

#include "cpen333/os.h" // identify OS

#ifdef WINDOWS
#include "cpen333/process/impl/windows/unlinker.h"
#else
#include "cpen333/process/impl/posix/unlinker.h"
#endif

#endif //CPEN333_PROCESS_UNLINKER_H
