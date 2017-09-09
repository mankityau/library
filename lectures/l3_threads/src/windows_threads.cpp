#include <windows.h>
#include <iostream>

struct ThreadData {
  int idx;
};

DWORD WINAPI MyThreadFunction( LPVOID lpParam ) {
  ThreadData* data = (ThreadData*)lpParam;
  std::cout << "hello thread " << data->idx << std::endl;
}

int main() {

  const int MAX_THREADS = 4;
  HANDLE threads[MAX_THREADS];   // store thread handles
  DWORD thread_ids[MAX_THREADS]; // store thread IDs
  ThreadData data[MAX_THREADS];  // data to pass to thread

  for (int i=0; i<MAX_THREADS; ++i) {
    data[i].idx = i;       // set data

    // create actual thread
    threads[i] = CreateThread(
        NULL,              // security attributes (NULL for default)
        0,                 // stack size  (0 for default)
        MyThreadFunction,  // function to be executed
        &data[i],          // pointer to a variable to be passed to thread
        0,                 // flags that control startup
        &thread_ids[i]     // ouput, receives a thread identifier
    );
  }

  // wait for threads to finish
  WaitForMultipleObjects(MAX_THREADS, threads, TRUE, INFINITE);

  // close all threads
  for (int i=0; i<MAX_THREADS; ++i) {
    CloseHandle(threads[i]);
  }

  return 0;

}