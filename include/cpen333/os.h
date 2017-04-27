//
// Created by Antonio on 2017-04-15.
//

#ifndef CPEN333_OS_H
#define CPEN333_OS_H

// Platform-dependent defines
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(_WIN64) || defined(WIN64) || defined(_WIN64)
#define WINDOWS
#elif defined(__APPLE__)
#define APPLE
#define POSIX
#else
#define LINUX
#define POSIX
#endif

#endif //CPEN333_OS_H
