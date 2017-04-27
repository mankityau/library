#include <iostream>
#include "cpen333/thread/thread_object.h"

// create a custom class with a "run" method that will execute within a thread
class CustomThread1 : public cpen333::thread::thread_object {

 private:
  int id_;

 public:
  CustomThread1(int id) : id_{id} {}


 private:
  // override "run" method, which is executed by thread
  void run() {
    for(int i = 0; i < 30; ++i)	{
      std::cout << "I am a CustomThread1 object/Thread. My id is " << id_ << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  }
};

// this thread object will start in "suspended" mode
class CustomThread2 : public cpen333::thread::suspended_thread_object {

 private:
  int id_;

 public:
  CustomThread2(int id) : id_{id} {}

  // override "run" method, which is executed by thread
  void run() {
    for(int i = 0; i < 30; ++i)	{
      std::cout << "I am a CustomThread2 object/Thread. My id is " << id_ << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  }
};


int main() {

  // create instance of the above active object on the stack
  // then allow it to run and then wait for it to terminate
  CustomThread1	object1(1), object2(2), object3(3) ;    // auto-started threads
  CustomThread2	object4(4), object5(5), object6(6);		// suspended threads

  // wait for 2 seconds
  std::this_thread::sleep_for(std::chrono::seconds(2));
  
  // start objects 4/5/6
  std::cout << "Starting suspended threads" << std::endl;
  object4.start();				// allow thread to run
  object5.start();				// allow thread to run
  object6.start();				// allow thread to run

  // wait for everyone to finish
  object1.join() ;
  object2.join() ;
  object3.join() ;
  object4.join() ;
  object5.join() ;
  object6.join() ;

  std::cout << "Finished." << std::endl;

  return 0;
}
