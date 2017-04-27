#include <iostream>
#include "cpen333/process/shared_memory.h"
#include "cpen333/process/subprocess.h"

// datapool data
struct DataPoolData {       // start of structure template
  int floor;                // floor corresponding to lifts current position
  int direction;            // direction of travel of lift
  int floors[10];           // an array representing the floors and whether requests are set
};                          // end of structure template

int main() {

  // To simplify handling of structured data mapped to a datapool, we have created a templated class shared_object
  // This allows us to assign a type to the block of shared memory, and use the datapool like a pointer to acces
  // its members
  std::cout << "Parent attempting to create/use the datapool....." << std::endl;
  cpen333::process::shared_object<DataPoolData> datapool("car1");

  // we can still get a raw pointer to the data
  std::cout << "Parent linked to datapool at address ....." << datapool.get() << std::endl;

  // The -> operator of the datapool has been overridden to access members of the underlying data directly
  std::cout << "Parent Writing value '10' to floor variable....." << std::endl;
  datapool->floor = 10;        // store 55 into the variable 'floor' in the datapool

  std::cout << "Parent Writing value '1' to direction variable....." << std::endl;
  datapool->direction = 1;     // store 1 into the variable 'direction' in the datapool

  std::cout << "Parent Writing value '0 1 2 3 4 5 6 7 8 9' to floors array....." << std::endl;
  for (int i = 0; i < 10; i++) {
    datapool->floors[i] = i;
  }

  //	Now that we have created the data pool and have stored data in it, it is safe to create
  //	a child process that can access the data
  std::cout << "Press Enter to launch external process" << std::endl;
  std::cin.get();

  // spawn child process
  // start it running, and detached
  cpen333::process::subprocess p1({"./q3child2"}, true, true);
  p1.join();        // wait for the child process to Terminate


  // Note that our data pool still exists at this point.  On Windows, the shared memory will be freed when the parent
  // process and all child processes using the datapool terminate.  On POSIX systems, however (like Linux or OSX),
  // the datapool will continue to exist until the next reboot.  This means if you run this program multiple times,
  // it will remember what was written last time.  To prevent this behaviour, we need to explicitly disconnect the
  // datapool from its name.  We call this "unlinking" it.
  datapool.unlink();  // release datapool name so it can be freed when process terminates

  std::cout << "Done." << std::endl;

  return 0;
}