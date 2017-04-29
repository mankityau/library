#include <cstdlib>

#include "common.h"
#include "cpen333/process/shared_memory.h"
#include "cpen333/process/mutex.h"

int main(int argc, char* argv[]) {

  // parse counter from argv
  int n = 100;
  if (argc > 1) {
    n = atoi(argv[1]);
  }

  // shared memory
  cpen333::process::shared_object<SharedData> data(MUTEX_SHARED_MEMORY_NAME);
  cpen333::process::mutex mutex(MUTEX_MUTEX_NAME);

  // safely increment shared data a
  for (int i=0; i<n; ++i) {
    mutex.lock();
    data->a = data->a+1;
    mutex.unlock();
  }

  return 0;
}