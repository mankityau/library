/**
 * @file
 * @brief Utility functions
 *
 * Contains functions for formatting strings, testing for data on standard input, and printing
 * system library errors.
 */

#ifndef CPEN333_UTIL_H
#define CPEN333_UTIL_H

#include "os.h"
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
#include <vector>

namespace cpen333 {

/**
 * @brief Formats a string according to standard C printf rules.
 *
 * Writes the string described by `format` to a new output string.
 * If `format` includes format specifiers (subsequences beginning with @%), the additional arguments following
 * `format` are formatted and inserted in the resulting string replacing their respective specifiers.
 *
 * @tparam Args variadic template types of additional arguments
 * @param format format specifier string
 * @param args additional arguments
 * @return new formatted string
 */
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args ) {
  // compute size of necessary string
  size_t size = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1;  // added 1 for terminating \0
  // create a new character buffer
  std::unique_ptr<char[]> buf( new char[size] );              // unique_ptr will delete memory when done
  std::snprintf( buf.get(), size, format.c_str(), args ... ); // actually do the snprintf
  return std::string( buf.get(), buf.get() + size - 1 );      // create output string
}

template <typename T, size_t N>
inline std::vector<T> to_vector(T (&array) [N]) {
  std::vector<T> out;
  out.reserve(N);
  for (int i=0; i<N; ++i) {
    out.push_back(array[i]);
  }
  return out;
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

/**
 * @brief Tests for keyboard input
 *
 * Wrapper around platform-specific code for detecting the presence of data available on the
 * standard input stream `stdin`.  This call returns immediately, without blocking.
 *
 * @return non-zero value if there is content
 */
inline int test_stdin() {
#ifdef WINDOWS
  return _kbhit();
#else
  return detail::_kbhit<void>();
#endif
}

/**
 * @brief Print error message
 *
 * Attempts to detect the last system error that occured, interprets the corresponding error message,
 * and prints it to sterr (the standard error output stream, usually the console), preceding it with the
 * custom message specified in msg.
 *
 * On Windows, the error corresponds to that obtained by GetLastError().  On Linux and OSX, it is the
 * build-in `errno`.  The error message produced by perror is platform-depend.
 *
 * perror should be called right after the error was produced, otherwise it may be overwritten by calls
 * to other system functions.
 *
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

/**
 * @brief Pause for input
 *
 * Mirrors the Windows system("pause") command in a cross-platform way, waiting for
 * keyboard input.
 */
void pause() {
#ifdef WINDOWS
  system("pause");
#else
  std::cin.clear();  // flush input
  std::cout << "Press ENTER to continue . . .";
  std::string line;
  std::getline(std::cin, line);
#endif
}

}// cpen333

#endif //CPEN333_UTIL_H
