#include <iostream>

//
//  Process are like threads, but they have an entire environment along with them (complete with environment variables)
//  Unlike threads, they do not share the same block of memory, so protocols need to be established for
//  processes to communicate (look up the term: Inter-Process Communication, or IPC)
//  They can also be run completely independently from each other (if the main process dies, the others may still
//  run on).
//

// custom subprocess library
#include "cpen333/process/subprocess.h"
#include "cpen333/util.h"

int main(int argc, char* argv[]) {

  std::cout << "Creating Child Processes....." << std::endl;

  // create process objects p1, p2 and p3
  // to run programs found in the current directory, use the prefix "./"
  //
  //  Note: for cross-platform compatibility, use / as a path-separator, and prefer to use relative paths when possible.
  //  the path "./" refers to the current directory you are running the program from, ".." is one directory up, "../../"
  //  is two directories up, "../../../" three directories up, etc...
  cpen333::process::subprocess p1(
      {"./child1"},   // child program and arguments, separated by commas, argv[]-style
      true,           // whether to start running process immediately
      true            // "detached" mode, if true, process will continue even if THIS program terminates
  );

  cpen333::process::subprocess p2(
      {"./child2"},
      true,
      true
  );

  cpen333::process::subprocess p3(
      {"./child3", "fred", "1.2", "child 3"},  // pass arguments to child 3's argv[]
      false,
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

  cpen333::pause();

  return 0;
}