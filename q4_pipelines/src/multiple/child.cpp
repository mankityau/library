#include "common.h"
#include "cpen333/process/pipe.h"

#include <iostream>
#include <cstdlib>
#include <thread>

int main(int argc, char* argv[]) {

  // detect id from command-line
  int id = 0;
  if (argc > 1) {
    id = atoi(argv[1]);
  }

  std::string pipe_name = std::string(PIPELINES_MULTIPLE_PREFIX) + std::to_string(id);

  std::cout << "Child " << id << " Process Creating the Pipeline \"" << pipe_name << "\"..." << std::endl;
  cpen333::process::pipe pipe(pipe_name);

  int start = id*10000;
  for (int i=start; i<start+100; ++i) {
    std::cout << "Child " << id << " writing " << i << " to the pipe" << std::endl;
    pipe.write(i);
    std::this_thread::sleep_for(std::chrono::seconds(2*id+1));
  }

  return 0;
}