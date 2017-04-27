#include "common.h"
#include "cpen333/process/pipe.h"

int main() {

  std::cout << "Child Process Opening Pipeline....." << std::endl;
  cpen333::process::pipe pipe(PIPE_NAME, 1024);   // create pipe

  // read an integer from the pipe
  int x;
  pipe.read(&x);
  std::cout << "Child Process read integer = " << x << " from Pipeline....." << std::endl;

  // read an array from the pipe
  int array[10];
  pipe.read(&array[0], sizeof(array)) ;

  std::cout << "Child Process read array = ";
  for(int i = 0; i < sizeof(array)/sizeof(array[0]); ++i) {
    std::cout << " " << array[i];
  }
  std::cout << std::endl;

  // read a string until we get a terminating zero
  std::string name;
  char c;
  while ((c = pipe.read()) != 0) {
    name.push_back(c);
  }
  std::cout << "Child Process read string = \"" << name << "\" from Pipeline....." << std::endl;

  // read struct
  example ex;
  pipe.read(&ex) ;
  std::cout << "Child Process read struct ex = [" << ex.x  << ", " << ex.y << "] from Pipeline....." << std::endl;

  std::cout << "Press ENTER to exit" << std::endl;
  std::cin.get();
  
  return 0;
}