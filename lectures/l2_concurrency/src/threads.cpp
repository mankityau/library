#include <thread>
#include <vector>
#include <iostream>

void thread_fn(int idx) {
  std::cout << "hello thread " << idx << std::endl;
}

int main() {

  std::vector<std::thread> threads;

  for (int i=0; i<4; ++i) {
    threads.push_back( std::thread(thread_fn, i) );
  }

  // wait for all threads to finish
  for (auto& thread : threads) {
    thread.join();
  }

  return 0;
}
