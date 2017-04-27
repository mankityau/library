#include <iostream>
#include <chrono>
#include <thread>

#include "cpen333/thread/suspended_thread.h"  // allow thread to run in suspended state

// function that will be used for thread body
void thread1() {
  for(int i = 0; i < 50; ++i) {
    std::cout << "Hello from child thread 1....." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}

// struct that will be used for thread body
struct thread2 {
  // here we override the () operator
  void operator()(/*input*/) {
      for(int i = 0; i < 30; ++i) {
        std::cout << "Hello from child thread 2....." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
      }
  }
};

// thread3 is known as a "lambda expression", representing a "closure".
//   it can optionally refer to things outside the function if they are put in
//   the "capture" brackets
auto thread3 = [/*capture*/](int input){
  for(int i = 0; i < 30; i ++) {
    std::cout << "Hello from child thread 3...I Have been given the data " << input << "....." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
};

// main thread
int main() {

  std::cout << "Parent Thread: Creating Child Thread 1 in Active State" << std::endl;

  // alias for creating suspended threads
  using thread = cpen333::thread::suspended_thread;

  // create a child thread to run concurrently with the main thread
  thread t1(thread1);
  thread t2(thread2{});   // Note: we need to create an instance of struct thread2, I'm using a brace-initialization
  thread t3(thread3, 10);

  // suspended threads need to be manually started
  t1.start();
  t2.start();
  t3.start();

  for(int i = 0; i < 40; i ++) {
    std::cout << "..........Hello from Parent thread ....." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  // wait for threads to terminate
  t1.join();
  t2.join();
  t3.join();

  return 0;
}