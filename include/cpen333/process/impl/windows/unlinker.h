#ifndef CPEN333_PROCESS_WINDOWS_UNLINKER_H
#define CPEN333_PROCESS_WINDOWS_UNLINKER_H

#include <string>

namespace cpen333 {
namespace process {
namespace windows {

template<typename T>
class unlinker {
 public:
  using type = T;

  unlinker(T &resource) : resource_{resource} {}

  ~unlinker() {
    resource_.unlink();
  }

  static bool unlink(const std::string &name) {
    return type::unlink(name);
  }

 private:
  T &resource_;
};

} // windows

template<typename T>
using unlinker = windows::unlinker<T>;

} // process
} // cpen333


#endif //CPEN333_PROCESS_WINDOWS_UNLINKER_H
