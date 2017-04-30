#ifndef CPEN333_PROCESS_NAMED_RESOURCE_H
#define CPEN333_PROCESS_NAMED_RESOURCE_H

#include <string>
#include "cpen333/os.h"

#ifndef WINDOWS
#include "cpen333/process/impl/sha1.h"
#endif

// limit maximum resource name length to this
// Windows limit is 260, POSIX limit is 251, though OSX limit seems to be 30 :S
//     POSIX names must also begin with a / for portability if used between
//     multiple processes, otherwise behaviour is implementation defined
// Due to the 30 character limit on POSIX, we will take a sha1 hash and base64-encode
//      which will generate 28 characters.  Add a / prefix and terminating zero = 30.
#ifdef WINDOWS
#define MAX_RESOURCE_NAME 248
#else
#define MAX_RESOURCE_NAME 30
#endif

namespace cpen333 {
namespace process {

class named_resource {
 private:
  char name_[MAX_RESOURCE_NAME];

 public:
  named_resource(const std::string &name, bool raw = false) {
    if (raw) {
      size_t len = (MAX_RESOURCE_NAME-1) > name.length() ? name.length() : (MAX_RESOURCE_NAME-1);
      name.copy(&name_[0], len, 0);
      name_[len] = 0;  // terminating zero
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

  virtual bool unlink() = 0;  // abstract, force unlink to be defined

  static bool unlink(const std::string& name) {
    return false;
  }

 protected:

  /**
   *  Internal-use system name
   */
  std::string name() const {
    return name_;
  }

  const char* name_ptr() const {
    return &name_[0];
  }

#ifdef WINDOWS

  /**
  * Create a valid resource name for the platform
  * @param name original resource name
  * @param out platform-safe resource name
  */
  static void make_resource_name(const std::string &name, char out[]) {

    size_t sidx = 0;
    size_t tidx = 0;

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

#else

  /**
  * Create a valid resource name for the platform, on Linux/OSX this is
  * a leading / with sha1 base64-encoded hash of the string name, with
  * any +=/ replaced with underscores
  * @param name original resource name
  * @param out platform-safe resource name
  */
  static void make_resource_name(const std::string &name, char out[]) {
    sha1 hash = sha1(name.c_str()).finalize();

    // leading slash for pathname
    out[0] = '/';
    hash.print_base64(&out[1], true); // print starting at offset 1, with terminating zero
    // replace '/', '+', and '=' with _ for safer path name
    for (int i=1; i<MAX_RESOURCE_NAME; ++i) {
      if (out[i] == '/' || out[i] == '+' || out[i] == '=') {
        out[i] = '_';
      }
    }
  }

#endif



};

} // process
} // cpen333

#endif //CPEN333_PROCESS_NAMED_RESOURCE_H
