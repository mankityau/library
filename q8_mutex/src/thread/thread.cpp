#include <iostream>
#include <thread>
#include <mutex>

// shared global variable
size_t a = 0;
std::mutex mutex;   // mut(ual)ex(lusion)

// thread function for incrementing a
void unsafe_increment(size_t n) {
  for (int i=0; i<n; ++i) {
    a = a+1;
  }
}

// safe thread function
void safe_increment(size_t n) {
  for (int i=0; i<n; ++i) {
    mutex.lock();    // protect access
    a = a+1;
    mutex.unlock();  // free access
  }
}

// safe thread function
void guarded_increment(size_t n) {
  for (int i=0; i<n; ++i) {
    std::lock_guard<std::mutex> lock(mutex);  // lock (with auto-unlock when out of scope)
    a = a+1;
  }
}

int main() {

  // Unsafe access
  std::cout << "Unsafe access to shared variable 'a':" << std::endl;
  auto start = std::chrono::steady_clock::now();
  a = 0;
  std::thread t1(unsafe_increment, 1000000);
  std::thread t2(unsafe_increment, 1000000);
  t1.join();
  t2.join();
  auto end = std::chrono::steady_clock::now();
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << "   Value of a: " << a << " , time: " << time.count() << "ms" << std::endl;


  // Safe access
  std::cout << "Safe access to shared variable 'a':" << std::endl;
  start = std::chrono::steady_clock::now();
  a = 0;
  t1 = std::thread(safe_increment, 1000000);
  t2 = std::thread(safe_increment, 1000000);
  t1.join();
  t2.join();
  end = std::chrono::steady_clock::now();
  time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << "   Value of a: " << a << " , time: " << time.count() << " ms" << std::endl;


  // Guarded access
  std::cout << "Guarded access to shared variable 'a':" << std::endl;
  start = std::chrono::steady_clock::now();
  a = 0;
  t1 = std::thread(guarded_increment, 1000000);
  t2 = std::thread(guarded_increment, 1000000);
  t1.join();
  t2.join();
  end = std::chrono::steady_clock::now();
  time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << "   Value of a: " << a << " , time: " << time.count() << " ms" << std::endl;

  return 0;
}