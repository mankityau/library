#ifndef Q8_MUTEX_PROCESS_H
#define Q8_MUTEX_PROCESS_H

#define MUTEX_SHARED_MEMORY_NAME "q8_mutex_shared"
#define MUTEX_MUTEX_NAME "q8_mutex_mutex"
#define MUTEX_SEMAPHORE_NAME "q8_mutex_semaphore"


// struct definition for shared data
struct SharedData {
  int a;
};

#endif //Q8_MUTEX_PROCESS_H
