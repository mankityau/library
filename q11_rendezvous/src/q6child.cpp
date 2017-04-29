#include <iostream>
#include <thread>
#include "cpen333/process/rendezvous.h"

int main(int argc, char* argv[]) {

  // grab name from input arguments
  int id = 0;
  if (argc > 1) {
    id = atoi(argv[1]);
  }

  std::cout << "Child " << id << " running ...." << std::endl;

  // open rendezvous
  cpen333::process::rendezvous rendezvous("q6rendezvous", 6);

  for(int i = 0; i < 10; i ++)	{
    std::this_thread::sleep_for(std::chrono::seconds(id));  // sleep for id seconds
    std::cout << "Child " << id << " Arriving at Rendezvous....." << std::endl;
    rendezvous.wait();                                      // wait at the rendezvous point
    std::cout << "Go " << id << std::endl;
  }

  return 0;
}