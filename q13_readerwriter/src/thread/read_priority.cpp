#include <iostream>
#include <thread>
#include <vector>
#include "cpen333/thread/shared_mutex.h"

// template function is compiled for every type of MutexType
template<typename MutexType>
void read_thread(MutexType& mutex, size_t n) {
  for (int i=0; i<n; ++i) {
    {
      // lock in "shared" mode
      std::shared_lock<MutexType> lock(mutex);
      std::cout << "R";
      std::cout.flush();  // flush read
      // std::this_thread::sleep_for(std::chrono::milliseconds(100));  // reads are usually pretty fast
    }
    std::this_thread::yield();  // prevent this thread from hogging resources
  }
}

// template function is compiled for every type of MutexType
template<typename MutexType>
void write_thread(MutexType& mutex, size_t n) {
  for (int i=0; i<n; ++i) {
    {
      // lock in "exclusive" mode
      std::lock_guard<MutexType> lock(mutex);
      std::cout << "W";
      std::cout.flush();  // flush read
      // std::this_thread::sleep_for(std::chrono::milliseconds(100));  // writes usually take longer
    }
    std::this_thread::yield();  // prevent this thread from hogging resources
  }
}

int main() {

  std::vector<std::thread> readers;  // reader threads
  std::vector<std::thread> writers;  // writer threads

  // share a read-priority mutex
  std::cout << "Battle of the Readers vs Writers, read priority" << std::endl;

  // start separate threads with readers and writers
  cpen333::thread::shared_mutex_shared rmutex;  // gives preference to sharers (in our case, readers)
  for (int i=0; i<100; ++i) {
    writers.push_back(std::thread(&write_thread<decltype(rmutex)>, std::ref(rmutex), 200));
    readers.push_back(std::thread(&read_thread<decltype(rmutex)>, std::ref(rmutex), 200));
  }
  // wait until all threads complete, then dispose
  for (int i=0; i<100; ++i) {
    readers[i].join();
    writers[i].join();
  }
  readers.clear();
  writers.clear();


  std::cout << std::endl << "Done." << std::endl;
  return 0;
}