/**
 * @file
 * @brief Named-resource wrapper for RAII-style unlinking of the resource name on POSIX systems
 */

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
 * @brief Special signal handler for cleaning up resources on signals
 *
 * Static class for handling interrupt/kill signals to make sure
 * resources get cleaned up.  Otherwise we could be leaking resources
 * and preventing our programs from running the next time they are called.
 * @tparam Dummy an unused template parameter used to allow in-header static initialization
 */
template<typename Dummy>
class unlink_signal_handler {
  // stores list of registered unlink functions
 private:
  /**
   * Old SIGINT interrupt action
   */
  static struct sigaction old_int_action_;

/**
   * Old SIGHUP hangup action
   */
  static struct sigaction old_hup_action_;

  /**
   * Old SIGTERM termination action
   */
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

  /**
   * @brief Remove a named-resource from internal list of resources to unlink on signal catch
   *
   * This should be called by an unlinker object in its destructor, since the unlinker itself will
   * handle the clean-up.
   *
   * @tparam T named-resource type
   * @param obj named-resource reference
   */
  template<typename T>
  static void remove_unlink(T &obj) {
    // protect memory access
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    // find and remove iterator entry
    unlinkset_.erase(&obj);
  }

  /**
   * @brief Add a named-resource to internal list of resources to unlink on signal catch
   *
   * This should be called by an unlinker object in its constructor, registering a named-resource
   * in case a signal is caught and the program terminates without the unlinker's destructor being
   * called.
   *
   * @tparam T named-resource type
   * @param obj named-resource reference
   */
  template<typename T>
  static void add_unlink(T &obj) {
    init();  // make sure we are initialized
    // protect memory access
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    unlinkset_.insert(&obj);
  }

  /**
   * @brief Signal handler used for resource unlinking when signal is caught
   *
   * Will loop through all registered named resources and unlink their names from the resources, allowing
   * the resources to be freed when they are no longer used.
   *
   * @param signum signal id number
   */
  static void handler(int signum) {
    {
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

      init_ = false;  // allow handler to be re-initialized
      raise(signum);
    } else if (old->sa_handler == SIG_IGN) {
      // nothing
    } else {
      // call old action directly
      old->sa_handler(signum);
    }
  }
};

/**
 * @cond
 *
 * start of hiding static initialization code from doxygen
 */

/**
 * Initialize the old SIGINT handler
 * @tparam Dummy unused template parameter to allow in-header static initialization
 */
template<typename Dummy>
struct sigaction unlink_signal_handler<Dummy>::old_int_action_{};

/**
 * Initialize the old SIGHUP handler
 * @tparam Dummy unused template parameter to allow in-header static initialization
 */
template<typename Dummy>
struct sigaction unlink_signal_handler<Dummy>::old_hup_action_{};

/**
 * Initialize the old SIGTERM handler
 * @tparam Dummy unused template parameter to allow in-header static initialization
 */
template<typename Dummy>
struct sigaction unlink_signal_handler<Dummy>::old_term_action_{};

/**
 * @var unlink_signal_handler<Dummy>::mutex_
 * @brief Initialize the static unlink signal handler's mutex
 * @tparam Dummy unused template parameter to allow in-header static initialization
 */
template<typename Dummy>
std::mutex unlink_signal_handler<Dummy>::mutex_{};

/**
 * Initialize the old SIGINT handler
 * @tparam Dummy unused template parameter to allow in-header static initialization
 */
template<typename Dummy>
bool unlink_signal_handler<Dummy>::init_ = false;

template<typename Dummy>
std::unordered_set<
    cpen333::process::named_resource *> unlink_signal_handler<Dummy>::unlinkset_{};

/**
 * end of hiding code from doxygen
 * @endcond
 */

} // detail

/**
 * @brief A named-resource wrapper that provides a convenient RAII-style unlinking of the resource name
 *
 * Used to ensure that a resource's name is unlinked when the unlinker object drops out of scope.  This should
 * prevent resource leaking in case of exceptions.
 *
 * This POSIX implementation additionally registers each named resource with a master list of resources to
 * unlink in case of signals such as SIGINT, SIGHUP and SIGTERM.  This is to allow processes to be manually
 * killed without leaking a resource name which may interfere with the next run of the program.
 *
 * @tparam T named-resource type, must extend cpen333::process::named_resource and support `bool unlink()` and
 *         `static bool unlink(std::string&)`
 */
template<typename T>
class unlinker {
 public:
  /**
   * @brief named_resource type
   */
  using type = T;

  /**
   * @brief Constructs the object, wrapping the provided resource
   *
   * @param resource resource to unlink when unlinker drops out of scope
   */
  unlinker(T &resource) : resource_{resource} {
    shandler_.add_unlink(resource_);  // add unlink to signal handler
  }

  /**
   * @brief Destructor, calls the `unlink()` function of the wrapped resource
   */
  ~unlinker() {
    // remove from static handler
    shandler_.remove_unlink(resource_);  // remove unlink from signal handler
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
  static detail::unlink_signal_handler<void> shandler_;
};

/**
 * Static signal handler for unlinking of names on signal catch
 * @tparam T named_resource type
 */
template<typename T>
detail::unlink_signal_handler<void> unlinker<T>::shandler_{};

} // posix

/**
 * @brief Alias to the POSIX implementation of an unlinker
 * @tparam T named-resource type
 */
template<typename T>
using unlinker = posix::unlinker<T>;

} // process
} // cpen333


#endif //CPEN333_PROCESS_POSIX_UNLINKER_H
