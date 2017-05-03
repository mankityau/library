#ifndef CPEN333_PROCESS_IMPL_NAMED_RESOURCE_H
#define CPEN333_PROCESS_IMPL_NAMED_RESOURCE_H

#include <string>
#include "cpen333/os.h"
#include "cpen333/process/named_resource.h"
#include "cpen333/process/impl/sha1.h"

// limit maximum resource name length to this
// Windows limit is 260, POSIX limit is 251, though OSX limit seems to be 30 :S
//     Windows names cannot contain a backslash '\'
//     POSIX names must begin with a / for portability if used between
//     multiple processes, otherwise behaviour is implementation defined,
//     and cannot contain a / anywhere else.
// Due to the 30 character limit on POSIX, we will take a sha1 hash and base64-encode
//      which will generate 28 characters.  Add a / prefix and terminating zero = 30.
//      We will also replace an / with a _ in the resulting hash
#define MAX_RESOURCE_NAME 30

namespace cpen333 {
namespace process {

// implementation details
namespace impl {

class named_resource_base : public virtual named_resource {
 private:
  char name_[MAX_RESOURCE_NAME];

 public:
  named_resource_base(const std::string &name) {
    make_resource_name(name, name_);
  }

  // do not allow copying or moving
  named_resource_base(const named_resource_base &) = delete;
  named_resource_base(named_resource_base &&) = delete;
  named_resource_base &operator=(const named_resource_base &) = delete;
  named_resource_base &operator=(named_resource_base &&) = delete;

  virtual ~named_resource_base() {
    // clean-up
  }

  virtual bool unlink() = 0;  // abstract, force unlink to be defined

  static bool unlink(const std::string &name) {
    return false;
  }

 protected:

  /**
   *  Internal-use system name
   */
  std::string name() const {
    return name_;
  }

  const char *name_ptr() const {
    return &name_[0];
  }

  /**
  * Create a valid resource name for the platform, on Linux/OSX this is
  * a leading / with sha1 base64-encoded hash of the string name.  We
  * will also replace any other / with _
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
      if (out[i] == '/') {
        out[i] = '_';
      }
    }
  }

};

} // impl

} // process
} // cpen333

#endif //CPEN333_PROCESS_IMPL_NAMED_RESOURCE_H
