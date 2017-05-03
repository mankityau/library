#include "cpen333/os.h"

#include <sys/mman.h>
#include <iostream>
#include <cerrno>    // error numbers


#ifdef POSIX
int main(int argc, char* argv[]) {

  if (argc < 2) {
    std::cout << "Usage:" << std::endl;
    std::cout << argv[0] << " <list of shared memory IDs...>" << std::endl;
    std::cout << "\t\tShared memory IDs can be found on OSX by using the command" << std::endl;
    std::cout << "\t\t\tlsof | grep PSXSHM | awk '{print $7}'" << std::endl;
  }

  // try to unlink all semaphores provided as arguments
  for (int i=1; i<argc; ++i) {
    int status = shm_unlink(argv[i]);
    if (status != 0 && errno == EACCES) {
      std::cerr << "Failed to unlink shared memory " << argv[i] << ": ACCESS DENIED" << std::endl;
    }
  }
  return 0;
}
#else
int main(int argc, char* argv[]) {
  std::cout << "Cannot unlink shared memory on Windows" << std::endl;
  return 0;
}
#endif
