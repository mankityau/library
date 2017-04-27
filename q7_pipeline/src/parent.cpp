#include <iostream>

#include "common.h"
#include "cpen333/process/pipe.h"
#include "cpen333/process/unlinker.h"
#include "cpen333/process/subprocess.h"

int main() {

  std::cout << "Parent Process Creating the Pipeline....." << std::endl;
  cpen333::process::pipe pipe(PIPE_NAME, 1024);                 // create a shared pipe
  cpen333::process::unlinker<decltype(pipe)> unlinker(pipe);    // unlink pipe when runs out of scope

  std::cout << "Parent Process Creating Child Process to Read from Pipeline....." << std::endl;
  cpen333::process::subprocess p1({"./child"}, true, true);

  // write i to pipe
  int i = 20;
  std::cout << "Hit RETURN to Write the integer " << i << " to the pipeline.....";
  std::cin.get();   // get line
  pipe.write(i);

  // write array to pipe
  int array[10] = {1,2,3,4,5,6,7,8,9,0};
  std::cout << "Hit RETURN to Write the integer array 1 2 3 4 5 6 7 8 9 0 to the pipeline....." << std::endl;
  std::cin.get();
  pipe.write(&array[0], sizeof(array)); // write the array of integers' to the pipe

  // write string to pipe
  std::string name = "Snuffaluffagus";
  std::cout << "Hit RETURN to Write the string \"" << name << "\" to the pipeline....." << std::endl;
  std::cin.get();
  pipe.write(name.c_str(), name.size()+1);  // write the terminating 0 as well

  // write struct
  example ex = {10, 2.3456f};
  std::cout << "Hit RETURN to Write the structure [" << ex.x << ", " << ex.y << "] to the pipeline....." << std::endl;
  std::cin.get();
  pipe.write(ex);

  p1.join();

  return 0;
}
