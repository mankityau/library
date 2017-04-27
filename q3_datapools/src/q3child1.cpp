#include <iostream>
#include "cpen333/process/shared_memory.h"

//
//	It's important to realise that all processes accessing the same datapool must
//	describe exactly the same datapool or structure template, otherwise corruption
//	of data will occur. Ideally, the structure template would be put into a single header file
//	which would be '#include'd' into each program that used that datapool. However, for
//	simplicity, we have recreated the same template in each process
//
struct 	DataPoolData {  // start of structure template
  int floor;            // floor corresponding to lifts current position
  int direction;        // direction of travel of lift
  int floors[10];       // an array representing the floors and whether requests are set
};                      // end of structure template


int main() {

// 	Attempt to make the datapool 'car1'. This process will not know
//	whether the datapool exists or not yet (we know that it does because we only
//	set the process running after the parent program had created it and stored data
//	in it) but that shouldn't matter, since the program can just attempt to make it. If it already exists
//	then it will simply use it. If it doesn't exist, then it will be created below

  std::cout << "Child attempting to create/use the datapool....." << std::endl;
  cpen333::process::shared_memory datapool("car1", sizeof(DataPoolData)) ;

// In order to access the data pool, we need a pointer to its location in memory.
  DataPoolData *data = (DataPoolData*)datapool.get();

  std::cout << "Child linked to datapool at address " << data << std::endl;

// print out the data in the datapool that was stored there by the parent
  std::cout << "Child Read value for Floor = " << data->floor << std::endl;
  std::cout << "Child Read value for Direction = " << data->direction << std::endl;

  std::cout << "Child Read values for Floors array = ";
  for(int i=0; i < 10; ++i) {
    std::cout << data->floors[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Press Enter to quit child process" << std::endl;
  std::cin.get();  // pause so programmer can view output before child dies

  // we don't 'unlink' the datapool here in case another child thread wishes to read the data
  // If nobody unlinks the datapool, then on POSIX systems, it will persist until the next reboot (or someone manually
  // unlinks it)

  return 0 ;
}