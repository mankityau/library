#ifndef CPEN333_PROCESS_NAMED_RESOURCE_H
#define CPEN333_PROCESS_NAMED_RESOURCE_H

#include <string>

namespace cpen333 {
namespace process {

// pure abstract class for named resource
class named_resource {
 public:

  /**
   * Virtual destructor
   */
  virtual ~named_resource() {};

  /**
   * Unlink name from named resource, allowing it to be freed (POSIX)
   * @return true if unlink successful, false if there was an error
   */
  virtual bool unlink() = 0;

  /**
   * Implementers should also provide a static method for unlinking
   * @param name desired resource name
   * @return true if unlink successful, false otherwise
   */
  static bool unlink(const std::string& name) {
    return false;
  }

};

}
} // cpen333

#endif //CPEN333_PROCESS_NAMED_RESOURCE_H
