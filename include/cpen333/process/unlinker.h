#ifndef CPEN333_PROCESS_UNLINKER_H
#define CPEN333_PROCESS_UNLINKER_H

#include <string>

#include <typeinfo>     // info about a type
#include <typeindex>    // for converting info to an index
#include <unordered_map>
#include <vector>
#include <mutex>

#ifdef POSIX
#include <signal.h>
#endif

namespace cpen333 {
namespace process {

//namespace detail {
//
//template<typename Dummy>
//class unlink_signal_handler {
//  // stores list of registered unlink functions
//
//  // static struct sigaction *old_handler_;
//  static std::mutex mutex_;
//  static std::unordered_map<std::string,std::vector<std::type_index>> map_;
//
// public:
//
//  template <typename T>
//  static void remove_unlink(const std::string& name) {
//    std::lock_guard<decltype(mutex_)> lock(mutex_);
//
//  }
//  static void add_unlink(const std::string& name) {
//    std::lock_guard<decltype(mutex_)> lock(mutex_);
//    typeinfo(T);
//  }
//
//  static void handler(int signum) {
//  }
//};
//
//// member initailization
//template<typename Dummy>
//unlink_signal_handler::mutex_;
//
//template<typename Dummy>
//unlink_signal_handler::old_handler_ = nullptr;
//
//} // detail

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
