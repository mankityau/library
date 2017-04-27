#include <iostream>
#include <thread>
#include <vector>
#include "cpen333/process/shared_mutex.h"
#include "cpen333/process/unlinker.h"

// template function is compiled for every type of MutexType
template<typename MutexType>
void read_thread(const std::string& mutex_name, size_t n) {
  MutexType mutex(mutex_name);                // load named mutex
  for (int i=0; i<n; ++i) {
    {
      // lock in "shared" mode
      std::shared_lock<MutexType> lock(mutex);
      std::cout << "R";
      std::cout.flush();  // flush read
      // std::this_thread::sleep_for(std::chrono::milliseconds(100));  // reads are usually pretty fast
    }
    std::this_thread::yield();  // prevent any single thread from hogging
  }
}

// template function is compiled for every type of MutexType
template<typename MutexType>
void write_thread(const std::string& mutex_name, size_t n) {
  MutexType mutex(mutex_name);              // load named mutex
  for (int i=0; i<n; ++i) {
    {
      // lock in "exclusive" mode
      std::lock_guard<MutexType> lock(mutex);
      std::cout << "W";
      std::cout.flush();  // flush read
      // std::this_thread::sleep_for(std::chrono::milliseconds(100));  // writes usually take longer
    }
    std::this_thread::yield();  // prevent any single thread from hogging
  }
}

int main() {

  std::vector<std::thread> readers;  // reader threads
  std::vector<std::thread> writers;  // writer threads

  // share a read-priority mutex
  std::cout << "Battle of the Readers vs Writers, read priority" << std::endl;

  // start separate threads with readers and writers
  cpen333::process::shared_mutex_shared rmutex("read_priority");  // gives preference to sharers (in our case, readers)
  cpen333::process::unlinker<decltype(rmutex)> unlinker(rmutex);  // unlink when done

  for (int i=0; i<100; ++i) {
    writers.push_back(std::thread(&write_thread<decltype(rmutex)>, "read_priority", 200));
    readers.push_back(std::thread(&read_thread<decltype(rmutex)>, "read_priority", 200));
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