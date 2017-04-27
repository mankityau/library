#include <iostream>
#include "cpen333/process/shared_memory.h"
#include "cpen333/process/subprocess.h"

// Let's suppose the data we want to put in the datapool is complex, i.e not just a simple
// int or char. The easiest way to store complex data is to introduce a structure template
// describing the names and types of the data we wish to group together as shown below
struct DataPoolData {       // start of structure template
  int floor;                // floor corresponding to lifts current position
  int direction;            // direction of travel of lift
  int floors[10];           // an array representing the floors and whether requests are set
};                          // end of structure template

int main() {

//	Start by making a datapool called 'car1'. This is actual name of the global datapool
//	we are creating, which can then be shared by all other processes in the system.
//	Note that we do not have to worry about which of the many processes using the datapool will
//	actually make it, because when a request is made to make a datapool, the operating system
//	checks to see if it exists or not. If not, then the pool will be made, otherwise the process
//	will simply 'use' the pool if it finds it already exists.
//
//	Note we also have to specify the size of the datapool we are making and because we want it
//	to hold the data in the structure template above, we make it that big

  std::cout << "Parent attempting to create/use the datapool....." << std::endl;
  cpen333::process::shared_memory datapool("car1", sizeof(DataPoolData));

// In order to access the data pool, we need a pointer to its location in memory, returned via datapool.get().
// Note that because the operating system doesn't know the purpose of our datapool, i.e. what data
// we are going to put in it, we have to 'cast' or 'convert' the actual pointer returned into a pointer
// to the data we want to store inside the datapool, i.e. convert it to a pointer to the structure
// DataPoolData. Hence the pointer *data below is a pointer to a structure that we want to map onto the datapool

  DataPoolData *data = (DataPoolData*)datapool.get();

  std::cout << "Parent linked to datapool at address ....." << data << std::endl;

// Now that we have the pointer to the datapool, we can put data into it
  std::cout << "Parent Writing value '10' to floor variable....." << std::endl;
  data->floor = 10;        // store 55 into the variable 'floor' in the datapool

  std::cout << "Parent Writing value '1' to direction variable....." << std::endl;
  data->direction = 1;     // store 1 into the variable 'direction' in the datapool

  std::cout << "Parent Writing value '0 1 2 3 4 5 6 7 8 9' to floors array....." << std::endl;
  for (int i = 0; i < 10; i++) {
    data->floors[i] = i;
  }

//	Now that we have created the data pool and have stored data in it, it is safe to create
//	a child process that can access the data
  std::cout << "Press Enter to launch external process" << std::endl;
  std::cin.get();

  // spawn child process
  // start it running, and detached
  cpen333::process::subprocess p1({"./q3child1"}, true, true);
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