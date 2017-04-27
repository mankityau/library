#ifndef CPEN333_THREAD_THREAD_OBJECT_H
#define CPEN333_THREAD_THREAD_OBJECT_H

#include <thread>
#include "cpen333/thread/suspended_thread.h"

namespace cpen333 {
namespace thread {

class thread_object : public std::thread {
 public:
  // pass "this.run()" to thread
  thread_object() : std::thread{&thread_object::run, this} {}

 protected:
  // pure virtual method
  virtual void run() = 0;
};

class suspended_thread_object : public suspended_thread {
 public:

  // pass "this.run()" to thread
  suspended_thread_object() : suspended_thread{&suspended_thread_object::run, this} {}

 protected:
  // pure virtual method
  virtual void run() = 0;
};

} // thread
} // cpen333

#endif // CPEN333_THREAD_THREAD_OBJECT_H