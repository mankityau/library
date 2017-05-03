#ifndef CPEN333_PROCESS_WINDOWS_SHARED_MEMORY_H
#define CPEN333_PROCESS_WINDOWS_SHARED_MEMORY_H

// suffix to append to shared memory names for uniqueness
#define SHARED_MEMORY_NAME_SUFFIX "_shm"

#include <string>
#include <cstdint>
#include <windows.h>

#include "cpen333/util.h"
#include "cpen333/process/impl/named_resource_base.h"

namespace cpen333 {
namespace process {
namespace windows {

class shared_memory : public impl::named_resource_base {
 public:
  using native_handle_type = HANDLE;

  shared_memory(const std::string &name, size_t size, bool readonly = false ) :
      impl::named_resource_base{name+std::string(SHARED_MEMORY_NAME_SUFFIX)},
      handle_{NULL},
      data_{nullptr},
      size_{size} {

    // Clear thread error, create mapping, then check if already exists
    SetLastError(0);
    handle_ = CreateFileMapping(INVALID_HANDLE_VALUE,  // create in paging file
                                NULL,
                                PAGE_READWRITE,        // always read-write so we can initialize if we need to
                                0,
                                size,
                                name_ptr() );
    if (handle_ == INVALID_HANDLE_VALUE) {
      cpen333::perror(std::string("Cannot create shared memory ") + this->name());
      return;
    }
    // see if we need to initialize??
    // bool initialize = GetLastError() != ERROR_ALREADY_EXISTS;

    // map (if not already mapped
    int flags = (readonly ? FILE_MAP_READ : FILE_MAP_WRITE);
    data_ = MapViewOfFile(
        handle_,            // file-mapping object to map into address space
        flags,              // read-write
        0, 0,               // offset
        size                // number of bytes to map, 0 means all
    );

    if (data_ == NULL) {
      cpen333::perror(std::string("Cannot map shared memory ") + this->name());
      return;
    }
  }

  ~shared_memory() {
    // unmap memory
    if (data_ != nullptr) {
      bool success = UnmapViewOfFile(data_);
      if (!success) {
        cpen333::perror(std::string("Cannot unmap shared memory ") + name());
      }
    }

    if (handle_ != NULL && !CloseHandle(handle_)) {
      cpen333::perror(std::string("Cannot close shared memory handle ") + name());
    }
  }

  void* operator->() {
    return data_;
  }

  void* get(size_t offset = 0) {
    return (void*)((uint8_t*)data_ + offset);
  }

  uint8_t& operator[](size_t offset) {
    return *((uint8_t*)get(offset));
  }

  template<typename T>
  void* get(size_t offset) {
    return (T*)get(offset);
  }

  template<typename T>
  T* get() {
    return (T*)data_;
  }

  native_handle_type native_handle() {
    return handle_;
  }

  bool unlink() {
    return false;
  }

  static bool unlink(const std::string& name) {
    return false;
  }

 private:
  native_handle_type handle_;
  void* data_;       // pointer to shared data
  size_t size_;      // size of memory block

};

} // native implementation

using shared_memory = windows::shared_memory;

} // process
} // cpen333

#endif //CPEN333_PROCESS_WINDOWS_SHARED_MEMORY_H
