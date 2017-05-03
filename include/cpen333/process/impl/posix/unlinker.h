#ifndef CPEN333_PROCESS_POSIX_UNLINKER_H
#define CPEN333_PROCESS_POSIX_UNLINKER_H

#include <string>
#include <unordered_set>
#include <mutex>
#include <signal.h>

#include "cpen333/process/named_resource.h"

namespace cpen333 {
namespace process {
namespace posix {

namespace detail {

/**
 * Static class for handling interrupt/kill signals to make sure
 * resources get cleaned up.  Otherwise we could be leaking resources
 * and preventing our programs from running the next time they are called.
 * @tparam Dummy
 */
template<typename Dummy>
class unlink_signal_handler {
  // stores list of registered unlink functions

  static struct sigaction old_int_action_;
  static struct sigaction old_hup_action_;
  static struct sigaction old_term_action_;
  
  static std::mutex mutex_;
  static bool init_;
  static std::unordered_set<cpen333::process::named_resource *> unlinkset_;

 private:
  static void init() {
    if (!init_) {
      // add a signal handler
      struct sigaction new_action;

      new_action.sa_handler = unlink_signal_handler<void>::handler;
      sigemptyset (&new_action.sa_mask);
      new_action.sa_flags = 0;

      // set new action
      sigaction (SIGINT, &new_action, &old_int_action_);
      sigaction (SIGHUP, &new_action, &old_hup_action_);
      sigaction (SIGTERM, &new_action, &old_term_action_);

      init_ = true;
    }
  }

 public:

  template<typename T>
  static void remove_unlink(T &obj) {
    // protect memory access
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    // find and remove iterator entry
    unlinkset_.erase(&obj);
  }

  template<typename T>
  static void add_unlink(T &obj) {
    init();  // make sure we are initialized
    // protect memory access
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    unlinkset_.insert(&obj);
  }

  static void handler(int signum) {
    {
      std::cout << "Caught signal " << signum << ", cleaning up resources..." << std::endl;
      // go through and unlink everyone
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      for (auto &nr : unlinkset_) {
        nr->unlink();
      }
      unlinkset_.clear();
    }

    // chain on old handler
    struct sigaction *old;
    if (signum == SIGINT) {
      old = &old_int_action_;
    } else if (signum == SIGHUP) {
      old = &old_hup_action_;
    } else if (signum == SIGTERM) {
      old = &old_term_action_;
    }

    // check for default handler, otherwise call
    if (old->sa_handler == SIG_DFL) {
      // call old default action
      struct sigaction new_action;
      sigemptyset (&new_action.sa_mask);
      new_action.sa_flags = 0;
      new_action.sa_handler = SIG_DFL;
      sigaction(signum, &new_action, NULL);
      raise(signum);
    } else if (old->sa_handler == SIG_IGN) {
      // nothing
    } else {
      // call old action directly
      old->sa_handler(signum);
    }
  }
};

// member initailization
template<typename Dummy>
std::mutex unlink_signal_handler<Dummy>::mutex_{};

template<typename Dummy>
struct sigaction unlink_signal_handler<Dummy>::old_int_action_{};

template<typename Dummy>
struct sigaction unlink_signal_handler<Dummy>::old_hup_action_{};

template<typename Dummy>
struct sigaction unlink_signal_handler<Dummy>::old_term_action_{};

template<typename Dummy>
bool unlink_signal_handler<Dummy>::init_ = false;

template<typename Dummy>
std::unordered_set<
    cpen333::process::named_resource *> unlink_signal_handler<Dummy>::unlinkset_{};

} // detail

/**
 * Actual unlinker class, responsible for cleaning up resources
 * @tparam T type of named resource, should extend cpen333::process::named_resource
 */
template<typename T>
class unlinker {
 public:
  using type = T;

  unlinker(T &resource) : resource_{resource} {
    shandler_.add_unlink(resource_);  // add unlink to signal handler
  }

  ~unlinker() {
    // remove from static handler
    shandler_.remove_unlink(resource_);  // remove unlink from signal handler
    resource_.unlink();
  }

  static bool unlink(const std::string &name) {
    return type::unlink(name);
  }

 private:
  T &resource_;
  static detail::unlink_signal_handler<void> shandler_;
};

// initialize signal handler
template<typename T>
detail::unlink_signal_handler<void> unlinker<T>::shandler_{};

} // posix

template<typename T>
using unlinker = posix::unlinker<T>;

} // process
} // cpen333


#endif //CPEN333_PROCESS_POSIX_UNLINKER_H
