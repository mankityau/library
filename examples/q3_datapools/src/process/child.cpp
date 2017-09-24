#include <iostream>
#include <string>
#include "cpen333/process/shared_memory.h"

//
//	It's important to that all processes accessing the same datapool must describe exactly the same datapool layout.
//  This is usually best accomplished with a shared header.
struct SharedData {
  char movie[20];    // title
  long length;       // in seconds
  double rating;     // on a 5* scale
};


int main(int argc, char* argv[]) {

  // grab name from first argument (note: argument 0 is the program name)
  std::string name = "child";
  if (argc > 1) {
    name = argv[1];
  }

  // Attempt to make the datapool "movie1". This process will not know whether the datapool exists or not.  If it
  // does already exist, it will simply use it. If it doesn't, then it will be created
  // Here we choose to use a "shared_object" rather than "shared_memory".  The shared object is the same as
  // shared_memory, except it is template-based and will automatically determine the necessary size of the datapool
  // and will do any necessary casting of pointers.  We have also overridden the -> operator so it can be tried like
  // a pointer to the underlying data type directly.
  std::cout << "Child attempting to create/use the datapool....." << std::endl;
  cpen333::process::shared_object<SharedData> datapool("movie1") ;

  std::cout << "Child linked to datapool at address " << datapool.get() << std::endl;

/// print out the data in the datapool that was stored there by the parent
  std::cout << "Child " << name << " read value for Movie = " << datapool->movie << std::endl;
  std::cout << "Child " << name << " read value for length = " << datapool->length << std::endl;
  std::cout << "Child " << name << " read values for rating = " << datapool->rating << std::endl << std::endl;

  std::cout << "Press Enter to quit child " << name << std::endl;
  std::cin.get();  // pause so you can view output before child dies

  // We don't 'unlink' the datapool here in case another child thread wishes to read the data.  Unlinking will
  // be handled by the parent process only.
  // If nobody unlinks the datapool, then on POSIX systems, it will persist until the next reboot (or someone else
  // manually unlinks it)

  return 0 ;
}