#ifndef CPEN333_PROCESS_SHARED_MEMORY_H
#define CPEN333_PROCESS_SHARED_MEMORY_H

#include "cpen333/os.h"           // identify OS

#ifdef WINDOWS
#include "cpen333/process/impl/windows/shared_memory.h"
#else
#include "cpen333/process/impl/posix/shared_memory.h"
#endif

namespace cpen333 {
namespace process {

template<typename T>
class shared_object : private shared_memory, public virtual named_resource {

 public:
  shared_object(const std::string &name, bool readonly = false) :
      shared_memory(name, sizeof(T), readonly) {}

  T& operator*() {
    return *shared_memory::get<T>();
  }

  T *operator->() {
    return shared_memory::get<T>();
  }

  T *get() {
    return shared_memory::get<T>();
  }

  bool unlink() {
    return shared_memory::unlink();
  }

  static bool unlink(const std::string &name) {
    return shared_memory::unlink(name);
  }

  std::string name() const {
    return shared_memory::name();
  }
};

} // process
} // cpen333

#endif //CPEN333_PROCESS_SHARED_MEMORY_H
