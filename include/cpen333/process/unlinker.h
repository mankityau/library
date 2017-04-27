#ifndef CPEN333_PROCESS_UNLINKER_H
#define CPEN333_PROCESS_UNLINKER_H

namespace cpen333 {
namespace process {

template<typename T>
class unlinker {
 public:
  using type = T;

  unlinker(T& resource) : resource_{resource}{}

  ~unlinker() {
    resource_.unlink();
  }

  static bool unlink(const std::string &name) {
    return type::unlink(name);
  }

 private:
  T &resource_;
};

}
} // cpen333

#endif //CPEN333_PROCESS_UNLINKER_H
