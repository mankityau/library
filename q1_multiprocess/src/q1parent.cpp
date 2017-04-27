#include <iostream>

#include "cpen333/process/subprocess.h"

int main(int argc, char* argv[]) {

  std::cout << "Creating Child Processes....." << std::endl;

  // create process objects p1, p2 and p3
  // to run programs found in the current directory, use the prefix "./"
  cpen333::process::subprocess p1(
      {"./q1child1"}, // child program and arguments, separated by comments
      true,         // whether to start running process immediately
      true          // "detached" mode, if true, process will continue even if THIS program terminates
  );

  cpen333::process::subprocess p2(
      {"./q1child2"},
      true,
      true
  );

  cpen333::process::subprocess p3(
      {"./q1child3", "fred", "1.2", "child 3"},  // pass arguments to child 3's argv[]
      true,
      true
  );

  std::cout << "Child Processes Activated....." << std::endl;

  p1.start();
  std::cout << "Waiting For Child1 to Terminate....." << std::endl;
  p1.join();

  p2.start();
  std::cout << "Waiting For Child2 to Terminate....." << std::endl;
  p2.join();

  p3.start();
  std::cout << "Waiting For Child3 to Terminate....." << std::endl;
  p3.join();

  return 0;
}