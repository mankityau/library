#ifndef CPEN333_PROCESS_POSIX_SHARED_MEMORY_H
#define CPEN333_PROCESS_POSIX_SHARED_MEMORY_H

// suffix to append to shared memory names for uniqueness
#define SHARED_MEMORY_NAME_SUFFIX "_shm_"

#include <string>
#include <cstring>  // for memcpy

#include "cpen333/util.h"
#include "cpen333/process/impl/named_resource.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>        // for mode constants
#include <fcntl.h>           // for O constants

namespace cpen333 {
namespace process {
namespace posix {

class shared_memory : public named_resource {
 public:
  using native_handle_type = int;

  shared_memory(const std::string &name, size_t size, bool readonly = false) :
    named_resource{name+std::string(SHARED_MEMORY_NAME_SUFFIX)}, fid_{-1},
    data_{nullptr}, size_{size} {

    // try opening new
    bool initialize = true;
    int mode = S_IRWXU | S_IRWXG; // user/group +rw permissions
    errno = 0;
    fid_ = shm_open(name_ptr(), O_RDWR | O_CREAT | O_EXCL, mode);
    if (fid_ < 0 && errno == EEXIST) {
      // create for open
      initialize = false;
      fid_ = shm_open(name_ptr(), readonly ? O_RDONLY : O_RDWR, mode);
    }

    if (fid_ < 0) {
      cpen333::perror(std::string("Cannot create shared memory ") + this->name());
      return;
    }


    // truncate and initialize
    if (initialize) {
      int resize = ftruncate(fid_, size_);
      if (resize < 0) {
        cpen333::perror(std::string("Cannot allocate shared memory ") + this->name());
        return;
      }
    }

    int flags = readonly ? PROT_READ : PROT_WRITE;
    data_ = mmap(nullptr, size_, flags, MAP_SHARED, fid_, 0);
    if (data_ == (void*) -1) {
      data_ = nullptr;
      cpen333::perror(std::string("Cannot map shared memory ") + this->name());
      return;
    }
  }

  ~shared_memory() {
    // unmap
    if (data_ != nullptr) {
      if (munmap(data_, size_) != 0) {
        cpen333::perror(std::string("Cannot unmap shared memory ") + name());
      }
    }

    // close
    if (fid_ != -1) {
      if (close(fid_) != 0) {
        cpen333::perror(std::string("Cannot close shared memory ") + name());
      }
    }
  }

  // ONLY UNLINK ON PURPOSE!
  bool unlink() {
    errno = 0;
    int status = shm_unlink(name_ptr());
    if (status != 0) {
      cpen333::perror(std::string("Failed to unlink shared memory ") + name());
    }
    return status == 0;
  }

  static bool unlink(const std::string& name) {
    char nm[MAX_RESOURCE_NAME];
    named_resource::make_resource_name(name+std::string(SHARED_MEMORY_NAME_SUFFIX), nm);
    int status = shm_unlink(&nm[0]);
    if (status != 0) {
      cpen333::perror(std::string("Failed to unlink shared memory ") + std::string(nm));
    }
    return status == 0;
  }

  void* operator->() {
    return data_;
  }

  void* get(size_t offset = 0) {
    return (void*)((char*)data_ + offset);
  }

  uint8_t& operator[](size_t offset) {
    return *((uint8_t*)get(offset));
  }

  template<typename T>
  T* get(size_t offset) {
    return (T*)get(offset);
  }

  template<typename T>
  T* get() {
    return (T*)data_;
  }

  native_handle_type native_handle() {
    return fid_;
  }

  private:
    native_handle_type fid_;
    void* data_;
    size_t size_;
};


} // native implementation

using shared_memory = posix::shared_memory;

} // process
} // cpen333

#endif //CPEN333_PROCESS_POSIX_SHARED_MEMORY_H
