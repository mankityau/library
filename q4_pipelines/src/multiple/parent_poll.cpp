#include <iostream>
#include <string>

#include "common.h"
#include "cpen333/process/pipe.h"
#include "cpen333/process/subprocess.h"
#include "cpen333/util.h" // to test stdin

//
//  When we are communicating with multiple child processes using pipes, we have two options:
//      Continuously poll the pipes to see if new information has arrived
//      Create a separate thread for each pipe to read the data
//  Here we take the polling approach
//

int main() {

  const int NUM_PIPES = 3;

  std::cout << "Parent process creating the pipelines...." << std::endl;
  std::cout << "Type 'Q' to exit main thread" << std::endl;
  cpen333::process::pipe* pipes[NUM_PIPES];
  cpen333::process::subprocess* processes[NUM_PIPES];

  // create pipes, unlinkers, and processes
  for (int i=0; i<NUM_PIPES; ++i) {
    std::string pipe_name = std::string(PIPELINES_MULTIPLE_PREFIX) + std::to_string(i);
    pipes[i] = new cpen333::process::pipe(pipe_name);
    processes[i] = new cpen333::process::subprocess({"./child", std::to_string(i)}, true, true);
  }

  // main loop, polling pipes
  while(true) { // poll forever
    int data;

    // loop through each pipe checking if there is enough data to read
    for (int i=0; i<NUM_PIPES; ++i) {
      if (pipes[i]->available() >= sizeof(data)) {
        pipes[i]->read(&data);
        std::cout << "Parent read " << data << " from pipe " << i << std::endl;
      }
    }

    // check for keyboard input
    if (cpen333::test_stdin() != 0) {
      char c = std::cin.get();
      std::cout << "Parent read " << c << " from keyboard." << std::endl;
      if (c == 'Q') {
        break;
      }
    }
  }

  // clean up
  for (int i=0; i<NUM_PIPES; ++i) {
    delete processes[i];
    pipes[i]->unlink();    // unlink pipe and delete
    delete pipes[i];
  }

  return 0;
}
