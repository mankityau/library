#ifndef CPEN333_PROCESS_NAMED_RESOURCE_H
#define CPEN333_PROCESS_NAMED_RESOURCE_H

#include <string>
#include "cpen333/os.h"

// limit maximum resource name length to this
// Windows limit is 260, POSIX limit is 251, 248 is smallest divisible by 4 and 8 bytes (32/64-bit word size)
#define MAX_RESOURCE_NAME 248

namespace cpen333 {
namespace process {

class named_resource {
 private:
  char name_[MAX_RESOURCE_NAME];

 public:
  named_resource(const std::string &name, bool raw = false) {
    if (raw) {
      for (size_t i=0; i<MAX_RESOURCE_NAME-1 && i <name.size(); ++i) {
        name_[i] = name[i];
      }
      for (int i=name.size(); i<MAX_RESOURCE_NAME; ++i) {
        name_[i] = 0;
      }
    } else {
      make_resource_name(name, name_);
    }
  }

  // do not allow copying or moving
  named_resource(const named_resource&) = delete;
  named_resource(named_resource&&) = delete;
  named_resource& operator=(const named_resource&) = delete;
  named_resource& operator=(named_resource&&) = delete;

  virtual ~named_resource() {
    // clean-up
  }

  std::string name() const {
    return name_;
  }

  virtual bool unlink() = 0;  // abstract, force unlink

  static bool unlink(const std::string& name) {
    return false;
  }

 protected:
  const char* name_ptr() const {
    return &name_[0];
  }

  /**
  * Create a valid resource name for the platform
  * @param prefix text to prepend, unmodified
  * @param name original resource name
  * @param out platform-safe resource name
  */
  static void make_resource_name(const std::string &name, char out[]) {

    size_t sidx = 0;
    size_t tidx = 0;

#ifdef WINDOWS
    // start with Global\ or Local\, otherwise cannot contain backslash
  if (name.compare(0, 7, "Global\\") == 0) {
    for (;sidx<7; ++sidx, ++tidx) {
      out[tidx] = name[sidx];
    }
  } else if (name.compare(0, 6, "Local\\") == 0) {
    for (;sidx<6; ++sidx, ++tidx) {
      out[tidx] = name[sidx];
    }
  }
#else
    // start with /
    if (name.size() == 0 || name[0] != '/') {
      out[tidx] = '/';
      ++tidx;
    } else if (name[0] == '/') {
      out[tidx] = name[0];
      ++tidx; ++sidx;
    }
#endif
    // append rest of name
    for (; sidx < name.size() && tidx < MAX_RESOURCE_NAME - 1; ++sidx, ++tidx) {
      if (name[sidx] < 31 || name[sidx] == '/' || name[sidx] == '\\' || name[sidx] == ' ') {
        out[tidx] = '_';
      } else {
        out[tidx] = name[sidx];
      }
    }

    // fill remainder with zeros
    for (; tidx < MAX_RESOURCE_NAME; ++tidx) {
      out[tidx] = 0;
    }
  }

};

} // process
} // cpen333

#endif //CPEN333_PROCESS_NAMED_RESOURCE_H
