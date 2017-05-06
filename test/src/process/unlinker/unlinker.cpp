#include <iostream>
#include <thread>
#include <signal.h>

#include "cpen333/os.h"
#include "cpen333/process/semaphore.h"
#include "cpen333/process/unlinker.h"

#ifdef POSIX

int main() {

  // set signal ignore handler
  signal(SIGINT, SIG_IGN);

  // test semaphore
  cpen333::process::semaphore sem("test_semaphore");
  {
    cpen333::process::unlinker<decltype(sem)> usem(sem);
  }
  // check if unlinked
  if (sem.unlink()) {
    std::cout << "semaphore should have already been unlinked" << std::endl;
    return -1;
  }

  {
    // Test cleanup after signal SIGINT catch
    cpen333::process::semaphore sem2("other semaphore");
    cpen333::process::unlinker<decltype(sem)> usem(sem2); // register with unlinker
    raise(SIGINT);
    std::this_thread::yield(); // allow handling of signal

    if (sem2.unlink()) {
      std::cout << "semaphore should have already been unlinked due to signal handler" << std::endl;
      return -2;
    }

  }

  std::cout << "test successful" << std::endl;
  return 0;
}

#else

int main() {
  std::cout << "Unlinker has no affect on Windows" << std::endl;
  return 0;
}

#endif