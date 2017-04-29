#include <iostream>
#include <string>
#include <thread>

#include "common.h"
#include "cpen333/process/pipe.h"
#include "cpen333/process/subprocess.h"
#include "cpen333/process/unlinker.h"
#include "cpen333/util.h" // to test stdin

//
//  When we are communicating with multiple child processes using pipes, we have two options:
//      Continuously poll the pipes to see if new information has arrived
//      Create a separate thread for each pipe to read the data
//  Here we take the separate threads approach
//

// non-atomic boolean for termination
volatile bool done = false;

void thread_consumer(int id) {
  // connect to shared pipe
  std::string pipe_name = std::string(Q7_MULTIPIPE_PREFIX) + std::to_string(id);
  cpen333::process::pipe pipe(pipe_name);
  cpen333::process::unlinker<decltype(pipe)> unlinker(pipe);  // clean-up by unlinking name when thread is finished

  // processes should eventually see when done flag is set
  while (!done) {
    int data;
    pipe.read(&data);
    std::cout << "Parent thread " << id << ": Read " << data << " from pipe " << id << std::endl;
  }

}

int main() {

  const int NUM_PIPES = 3;

  std::cout << "Parent Process Creating the Pipelines...." << std::endl;
  std::cout << "Type 'Q' to exit main thread" << std::endl;
  cpen333::process::pipe* pipes[NUM_PIPES];
  cpen333::process::subprocess* processes[NUM_PIPES];
  std::thread* threads[NUM_PIPES];

  // creates child processes and threads for servicing pipe
  for (int i=0; i<NUM_PIPES; ++i) {
    processes[i] = new cpen333::process::subprocess({"./child", std::to_string(i)}, true, true);
    threads[i] = new std::thread(thread_consumer, i);
  }

  // main loop, polling pipes
  while(true) { // poll forever
    // keyboard input
    char c = std::cin.get();
    std::cout << "Parent read " << c << " from keyboard." << std::endl;
    if (c == 'Q') {
      break;  // quit
    }
  }

  // signal complete
  done = true;

  for (int i=0; i<NUM_PIPES; ++i) {
    delete processes[i];
    pipes[i]->unlink();    // unlink pipe and delete

    threads[i]->join();    // wait for thread and delete
    delete threads[i];
  }

  std::cout << "Done." << std::endl;

  return 0;
}
