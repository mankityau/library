/**
 * @file
 * @brief Named-resource wrapper for RAII-style unlinking of the resource name on Windows systems
 */
#ifndef CPEN333_PROCESS_WINDOWS_UNLINKER_H
#define CPEN333_PROCESS_WINDOWS_UNLINKER_H

#include <string>

namespace cpen333 {
namespace process {
// namespace windows {  // remove namespace due to issue with VS2012

/**
 * @brief A named-resource wrapper that provides a convenient RAII-style unlinking of the resource name
 *
 * Used to ensure that a resource's name is unlinked when the unlinker object drops out of scope.  On Windows,
 * this doesn't really do anything, since unlinking of resource names is not a supported operation.
 *
 * @tparam T named-resource type,  should extend cpen333::process::named_resource, must support `bool unlink()` and
 *         `static bool unlink(std::string&)`
 */
template<typename T>
class unlinker {
 public:
  /**
   * Alias to the named-resource type
   */
  typedef T type;

  /**
   * @brief Constructs the object, wrapping the provided resource
   *
   * @param resource resource to unlink when unlinker drops out of scope
   */
  unlinker(T &resource) : resource_(resource) {}

  /**
   * @brief Destructor, calls the `unlink()` function of the wrapped resource
   */
  ~unlinker() {
    resource_.unlink();
  }

  /**
   * @brief Statically calls the `unlink(name)` function for the underlying type
   * @param name name of resource to unlink
   * @return always `false`, since unlinking is not supported on Windows
   */
  static bool unlink(const std::string &name) {
    return type::unlink(name);
  }

 private:
  T &resource_;
};

// } // windows

// Template Alias doesn't work in VS2012
// /**
//  * @brief Alias to the Windows implementation of an unlinker
//  * @tparam T named-resource type
//  */
// template<typename T>
// using unlinker = windows::unlinker<T>;

} // process
} // cpen333


#endif //CPEN333_PROCESS_WINDOWS_UNLINKER_H
