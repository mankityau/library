#include <iostream> // for io
#include <thread>   // for sleep

#include "cpen333/process/subprocess.h"
#include "cpen333/process/rendezvous.h"
#include "cpen333/process/unlinker.h"  // for unlinking shared resource

int main() {

  // create a shared rendezvous
  cpen333::process::rendezvous rendezvous("q6rendezvous", 6);
  // we want the rendezvous to be unlinked when it goes out of scope (to destroy the shared rendezvous after this
  // process terminates), so I will use a special "unlinker" here, using the RAII pattern
  cpen333::process::unlinker<cpen333::process::rendezvous> unlinker(rendezvous);

  // create 6 child process
  cpen333::process::subprocess* processes[6];
  for (int i=0; i<6; ++i) {
    std::cout << "Creating child process " << i << std::endl;
    processes[i] = new cpen333::process::subprocess({"./q6child", std::to_string(i+1)}, true, true);
  }

  // wait for all processes to complete and cleanup memory
  for (int i=0; i<6; ++i) {
    processes[i]->join();
    delete processes[i];
  }

  std::cout << "Done." << std::endl;

  return 0;
}