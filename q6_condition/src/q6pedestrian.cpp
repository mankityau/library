#include <iostream>
#include <cstdlib>   // atoi
#include <thread>
#include "cpen333/process/condition.h"

int main(int argc, char* argv[]) {

  // get id from arguments (if exists)
  int id = 0;
  if (argc > 1) {
    id = atoi(argv[1]);
  }

  // pedestrian condition
  cpen333::process::condition cross("q6/condition/safetocross");

  // record current time for termination condition
  using clock = std::chrono::steady_clock;
  auto start_time = clock::now();

  // loop for 60 seconds
  while(std::chrono::duration_cast<std::chrono::seconds>(clock::now()-start_time).count() < 60) {
    std::this_thread::sleep_for(std::chrono::milliseconds(600*id));		// sleep for a time period
    cross.wait() ;				     // wait for walk sign
    std::cout << "P" << id << " ";   // show that I have crossed
    std::cout.flush();
  }

  return 0;
}