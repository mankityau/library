#ifndef CPEN333_PROCESS_POSIX_SEMAPHORE_H
#define CPEN333_PROCESS_POSIX_SEMAPHORE_H

// maximum possible size of a semaphore
#define MAX_SEMAPHORE_SIZE LONG_MAX

// suffix appended to semaphore names for uniqueness
#define SEMAPHORE_NAME_SUFFIX "__semaphore__"

#include <string>
#include <chrono>
#include <thread>       // for yield
#include <fcntl.h>      /* For O_* constants */
#include <sys/stat.h>   /* For mode constants */
#include <semaphore.h>

#include "cpen333/util.h"
#include "cpen333/process/impl/named_resource.h"

namespace cpen333 {
namespace process {
namespace posix {

class semaphore : public named_resource {
 public:
  using native_handle_type = sem_t*;

  semaphore(const std::string& name, size_t value = 1) :
      named_resource{name+std::string(SEMAPHORE_NAME_SUFFIX)}, handle_{nullptr} {
    // create named semaphore
    handle_ = sem_open(name_ptr(), O_CREAT | O_RDWR, S_IRWXU | S_IRWXG, value);
    if (handle_ == nullptr) {
      cpen333::perror(std::string("Cannot create semaphore ")+this->name());
    }
  }

  ~semaphore() {
    // release the semaphore
    if (sem_close(handle_) != 0) {
        cpen333::perror(std::string("Cannot destroy semaphore ")+name());
    }
  }

  size_t value() {
    int val = 0;
#ifdef __APPLE__
    int success = -1;
    errno = ENOSYS; // not supported on OSX
#else
    int success = sem_getvalue(handle_, &val);
#endif
    if (success != 0) {
      cpen333::perror(std::string("Failed to get semaphore value ")+name());
    }
    return val;
  }

  void wait() {
    int success = 0;
    // continuously loop until we have the lock
    do {
      success = sem_wait(handle_);
      std::this_thread::yield();   // yield to prevent from hogging resources
    } while (success == -1 && errno == EINTR);
    if (success != 0) {
      cpen333::perror(std::string("Failed to wait on semaphore ")+name());
    }
  }

  bool try_wait() {
    int success = sem_trywait(handle_);
    if (errno == EINVAL) {
      cpen333::perror(std::string("Failed to wait on semaphore ")+name());
    }
    return (success == 0);
  }

  void notify() {
    // std::cout << "notifying sem " << name() << std::endl;
    int success = sem_post(handle_);
    if (success != 0) {
      cpen333::perror(std::string("Failed to post semaphore ")+name());
    }
  }

  template< class Rep, class Period >
  bool wait_for( const std::chrono::duration<Rep,Period>& timeout_duration ) {
    return wait_until(std::chrono::steady_clock::now()+timeout_duration);
  }

  template< class Clock, class Duration >
  bool wait_until( const std::chrono::time_point<Clock,Duration>& timeout_time ) {
    auto duration = timeout_time.time_since_epoch();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(duration);
    timespec ts;
    ts.tv_sec = sec.count();
    ts.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(duration-sec).count();
    int success = sem_timedwait(handle_, &ts);
    if (errno == EINVAL) {
      cpen333::perror(std::string("Failed to wait on semaphore ")+name());
    }
    return (success == 0);
  }

  native_handle_type native_handle() const {
    return handle_;
  }

  bool unlink() {
    int status = sem_unlink(name_ptr());
    if (status != 0) {
      cpen333::perror(std::string("Failed to unlink semaphore ")+name());
    }
    return (status == 0);
  }

  static bool unlink(const std::string& name) {
    char nm[MAX_RESOURCE_NAME];
    named_resource::make_resource_name(name+std::string(SEMAPHORE_NAME_SUFFIX), nm);
    int status = sem_unlink(&nm[0]);
    if (status != 0) {
      cpen333::perror(std::string("Failed to unlink semaphore ")+std::string(nm));
    }
    return (status == 0);
  }

 private:
  native_handle_type handle_;

};

} // native implementation

using semaphore = posix::semaphore;

} // process
} // cpen333


#endif //CPEN333_PROCESS_POSIX_SEMAPHORE_H
