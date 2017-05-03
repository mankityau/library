#include <iostream>
#include <thread>

#include "cpen333/os.h"
#include "cpen333/process/semaphore.h"
#include "cpen333/process/unlinker.h"

#ifdef POSIX

int main() {

  // test semaphore
  cpen333::process::semaphore sem("test_semaphore");
  {
    cpen333::process::unlinker<decltype(sem)> usem(sem);
  }
  // check if unlinked
  if (sem.unlink()) {
    std::cout << "semaphore should have already been unlinked" << std::endl;
    return -1;
  } else {
    std::cout << "test successful" << std::endl;
  }

  cpen333::process::semaphore sem2("other semaphore");
  {
    // register with unlinker
    cpen333::process::unlinker<decltype(sem)> usem(sem2);
    std::this_thread::sleep_for(std::chrono::seconds(30));
  }

  return 0;
}

#else

int main() {
  std::cout << "Unlinker has no affect on Windows" << std::endl;
  return 0;
}

#endif