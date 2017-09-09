#include <iostream>
#include <thread>

// struct storing shared memory
struct SharedMemory {
  int counter;
};

// thread method that accepts shared memory
void thread_increment(SharedMemory* shared) {
  shared->counter++;
}

int main() {

  const int NUM_THREADS = 1000;

  SharedMemory memory = {0};  // initialize memory with a zero count

  // start a bunch of threads to increment the shared counter
  std::thread* threads[NUM_THREADS];
  for (int i=0; i<NUM_THREADS; ++i) {
    threads[i] = new std::thread(thread_increment, &memory);
  }

  // wait for all threads to finish
  for (int i=0; i<NUM_THREADS; ++i) {
    threads[i]->join();
    delete threads[i];
    threads[i] = nullptr;
  }

  // print value of memory
  std::cout << memory.counter << std::endl;

  return 0;
}