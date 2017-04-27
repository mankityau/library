#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {

//  std::cout << "argc = " << argc << std::endl;
//  for(int i = 0; i < argc; ++i ) {
//    std::cout << "arg[" << i << "] = " << argv[i] << std::endl;
//  }

  // pause for user input
  std::cout << std::endl << "Press Enter to continue" << std::endl;
  std::cin.get();  // get enter and ignore

  for( int i = 0; i < 300; ++i) {
    std::cout << "Hello from child process 3...." << std::endl;
    // sleep 50 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  return 0;

}