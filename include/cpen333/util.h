//
// Created by Antonio on 2017-04-15.
//

#ifndef CPEN333_UTIL_H
#define CPEN333_UTIL_H

#include "cpen333/os.h"
#ifdef WINDOWS
#include <windows.h>
#include <conio.h>
#else
// for _khbit()
#include <stdio.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
// #include <stropts.h>
#endif
#include <cstdio>
#include <iostream>
#include <string>
#include <memory>

namespace cpen333 {

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args ) {
  // compute size of necessary string
  size_t size = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1;  // added 1 for terminating \0
  // create a new character buffer
  std::unique_ptr<char[]> buf( new char[size] );              // unique_ptr will delete memory when done
  std::snprintf( buf.get(), size, format.c_str(), args ... ); // actually do the snprintf
  return std::string( buf.get(), buf.get() + size - 1 );      // create output string
}

#ifndef WINDOWS
namespace detail {
template<typename Dummy>
/**
 Linux (POSIX) implementation of _kbhit().
 Morgan McGuire, morgan@cs.brown.edu
 */
int _kbhit() {
  static const int STDIN = 0;
  static bool initialized = false;

  if (!initialized) {
    // Use termios to turn off line buffering
    termios term;
    tcgetattr(STDIN, &term);
    term.c_lflag &= ~ICANON;
    tcsetattr(STDIN, TCSANOW, &term);
    setbuf(stdin, NULL);
    initialized = true;
  }

  int bytesWaiting;
  ioctl(STDIN, FIONREAD, &bytesWaiting);
  return bytesWaiting;
}
}
#endif

inline int test_stdin() {
#ifdef WINDOWS
  return _kbhit();
#else
  return detail::_kbhit<void>();
#endif
}

/**
 * Print error message
 * @param msg custom message
 */
inline void perror(const std::string& msg) {
#ifdef WINDOWS
  UINT LastError = GetLastError();
  char buff[512];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, LastError,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buff, 1024, NULL);
  std::cerr << msg << ": " << buff << std::endl;
#else
  ::perror(msg.c_str());
#endif
}

}// cpen333
#endif //CPEN333_UTIL_H
