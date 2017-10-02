#include <cpen333/process/pipe.h>
#include <iostream>

int main() {

  cpen333::process::pipe pipe("q4_pipes_server");
  pipe.open();
  pipe.write("Hello server!");
  pipe.write("What's up?");

  cpen333::pause();

  std::cout << "Goodbye" << std::endl;
  pipe.close();

  return 0;
}