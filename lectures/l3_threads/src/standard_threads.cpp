#include <iostream>
#include <thread>

struct ThreadData {
  int idx;
};

void MyThreadFunction( ThreadData* data ) {
  std::cout << "hello thread " << data->idx << std::endl;
}

int main() {
  const int MAX_THREADS = 4;
  std::thread* threads[MAX_THREADS];
  ThreadData data[MAX_THREADS];  // data to pass to thread

  for (int i=0; i<MAX_THREADS; ++i) {
    data[i].idx = i;       // set data
    threads[i] = new std::thread(
        MyThreadFunction,  // function
        &data[i]           // optional argument(s)
    );
  }

  // wait for threads to finish
  for (int i=0; i<MAX_THREADS; ++i) {
    threads[i]->join();
    delete threads[i];     // clean-up memory
    threads[i] = nullptr;
  }

  return 0;
}

