#include <iostream>
#include <string>

#include "common.h"
#include "cpen333/process/shared_memory.h"
#include "cpen333/process/mutex.h"
#include "cpen333/process/unlinker.h"
#include "cpen333/process/subprocess.h"

int main() {

  // shared memory
  cpen333::process::shared_object<SharedData> data(MUTEX_SHARED_MEMORY_NAME);
  cpen333::process::unlinker<decltype(data)> data_unlinker(data);           // unlink when out of scope

  // mutex
  cpen333::process::mutex mutex(MUTEX_MUTEX_NAME);
  cpen333::process::unlinker<decltype(mutex)> mutex_unlinker(mutex);        // unlink when out of scope

  // Unsafe access
  std::cout << "Unsafe access to shared variable 'a':" << std::endl;
  auto start = std::chrono::steady_clock::now();
  data->a = 0;
  // run child processes
  cpen333::process::subprocess p1({"./child_unsafe", "1000000"});
  cpen333::process::subprocess p2({"./child_unsafe", "1000000"});
  p1.join();
  p2.join();
  auto end = std::chrono::steady_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << "   Value of a: " << data->a << " , time: " << time.count() << "ms" << std::endl;

  // Safe access
  std::cout << "Safe access to shared variable 'a':" << std::endl;
  start = std::chrono::steady_clock::now();
  data->a = 0;
  // run child processes
  cpen333::process::subprocess p3({"./child_safe", "1000000"});
  cpen333::process::subprocess p4({"./child_safe", "1000000"});
  p3.join();
  p4.join();
  end = std::chrono::steady_clock::now();
  time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << "   Value of a: " << data->a << " , time: " << time.count() << "ms" << std::endl;

  return 0;
}
