#ifndef CPEN333_PROCESS_POSIX_SUBPROCESS_H
#define CPEN333_PROCESS_POSIX_SUBPROCESS_H

#include <string>
#include <vector>
#include <chrono>
#include <cstdlib> // for std::quick_exit
#include <thread>  // for yield

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#include "cpen333/util.h"
#include "cpen333/process/impl/named_resource.h"

namespace cpen333 {
namespace process {
namespace posix {

class subprocess {
 private:
  pid_t pid_;
  std::vector<std::string> exec_;
  bool detached_;
  bool started_;
  bool terminated_;

 public:
  subprocess(const std::vector<std::string> &exec, bool start = true, bool detached = false) :
      pid_{-1}, exec_{exec}, detached_{detached}, started_{false}, terminated_{false} {
    if (start) {
      this->start();
    }
  }

  // XXX results in ambiguity for common use-cases
//  subprocess(const std::string &cmd, bool start = true, bool detached = false) :
//      pid_{-1}, exec_{}, detached_{detached}, started_{false}, terminated_{false} {
//
//    wordexp_t p;
//    char **w;
//    int i;
//
//    wordexp("[a-c]*.c", &p, 0);
//    w = p.we_wordv;
//    for (i = 0; i < p.we_wordc; i++) {
//      printf("%s\n", w[i]);
//      exec_.push_back(w[i]);
//    }
//    wordfree(&p);
//
//    if (start) {
//      this->start();
//    }
//  }

  bool start() {

    if (started_) {
      return false;
    }

    // fork/exec
    pid_ = fork();
    if (pid_ == 0) {
      // we are in child
      std::vector<char*> c;
      for(size_t i = 0; i < exec_.size(); ++i){
        char *str = (char*)(&exec_[i][0]);
        c.push_back(str);
      }
      c.push_back(nullptr);  // null-terminated array of strings for execvp

      if (detached_) {
        // pid_t sid =
        setsid(); // detach process
      }
      int status = execvp(&(exec_[0][0]), c.data());
      cpen333::perror("Cannot create subprocess ");
      //std::quick_exit(status); // execvp failed, terminate child
      std::_Exit(status); // OSX doesn't seem to have quick_exit() defined
    }

    bool success = (pid_ >= 0);
    if (!success) {
      cpen333::perror("Failed to create process ");
    } else {
      started_ = true;
    }
    return success;
  }

  bool join() {
    if (pid_ < 0 || terminated_) {
      return false;
    }

    int status = 0;
    errno = 0;
    // loop while pid still running or interrupted
    while (waitpid(pid_, &status, 0) != -1 || errno == EINTR) {
    };

    if ( !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      cpen333::perror(std::string("Failed to wait for process "));
      return false;
    }
    terminated_ = true;
    return true;
  }

  /**
   * Waits for the process to terminate, waiting for a certain amount of time.
   * @tparam Rep  duration representation
   * @tparam Period  duration tick period
   * @param duration time to wait for
   * @return true if process terminated (even if state was previously queried)
   */
  template<typename Rep, typename Period>
  bool wait_for(const std::chrono::duration<Rep,Period>& duration) {

    // There is self-pipe trick to try to catch SIGCHLD, but here we will just poll
    // http://stackoverflow.com/questions/282176/waitpid-equivalent-with-timeout

    // already terminated
    if (terminated_) {
      return true;
    }

    auto timeout = std::chrono::steady_clock::now()+duration;
    int status = 0;
    int r = 0;
    do {
      errno = 0;
      r = waitpid(pid_,&status, WNOHANG);
      if (r < 0) {
        // error
        cpen333::perror(std::string("Failed to wait for process "));
        return false;
      }

      std::this_thread::yield();  // yield to other threads to prevent excessive polling
    } while (r != pid_ && std::chrono::steady_clock::now() < timeout);

    if ( r == pid_ && WIFEXITED(status) ) {
      terminated_ = true;
      int exstat = WEXITSTATUS(status);
      if (exstat != 0) {
        cpen333::perror(std::string("Process terminated with exit code ") + std::to_string(exstat));
      }
    }

    return terminated_;

  }

  bool terminated() {
    if (terminated_) {
      return true;
    }
    // check wait
    return wait_for(std::chrono::milliseconds(0));
  }

};

} // native implementation

using subprocess = posix::subprocess;

} // process
} // cpen333

#endif //CPEN333_PROCESS_POSIX_SUBPROCESS_H
