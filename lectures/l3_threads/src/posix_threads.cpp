#include <iostream>
#include <pthread.h>

struct ThreadData {
  int idx;
};

void* MyThreadFunction( void* in ) {
  ThreadData* data = (ThreadData*)in;
  std::cout << "hello thread " << data->idx << std::endl;
  return NULL;
}

int main() {
  const int MAX_THREADS = 4;
  pthread_t threads[MAX_THREADS];
  ThreadData data[MAX_THREADS];  // data to pass to thread

  for (int i=0; i<MAX_THREADS; ++i) {
    data[i].idx = i;       // set data
    int success = pthread_create(
       &threads[i],         // thread handle
       NULL,                // attributes
       MyThreadFunction,    // function
       (void *) &data[i]);  // input
  }

  // wait for threads to finish
  for (int i=0; i<MAX_THREADS; ++i) {
    pthread_join( threads[i], NULL );
  }

  return 0;
}

