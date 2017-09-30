#include <cpen333/process/impl/windows/pipe.h>
#include <iostream>

int main() {

  cpen333::process::windows::pipe pipe("q4_pipes_server");
  pipe.open();
  pipe.write("Hello server!");
  pipe.close();

  std::cout << "Goodbye" << std::endl;
  cpen333::pause();

  return 0;
}