#include <iostream>
#include <string>

#include "cpen333/thread/thread_object.h"

class ClassWithThreads : public cpen333::thread::suspended_thread_object {
  int id_;

 public:
  ClassWithThreads(int id) : id_{id} {};

 private:
  void PrintMessageThread(const std::string& msg) {
    // print out message
    for (int i=0; i<1000; ++i) {
      std::cout << msg << std::endl;
    }
  }

  void DisplayIdThread() {
    // print out id
    for(int i = 0; i < 1000; i ++) {
      std::cout << id_ << std::endl;
    }

  }

  // main thread
  void run() {
    std::string msg1 = "Object threads are as easy as this....." ;
    std::string msg2 = "It really is a concurrent world" ;

    //	Create two regular threads for the function 'PrintMessageThread'
    //	Pass each thread a different message so it does something different to illustrate the concept

    //	We have to pass to the thread the point 'this' as this will allow the thread
    //	to gain access to the private/public member variables and functions declared within the class
    //	This concept is demonstrated in the 3rd thread below
    std::thread thread1(&ClassWithThreads::PrintMessageThread, this, msg1);
    //     ^                   ^                    ^           ^      ^----- argument to the thread
    //     |                   |                    |           |------------ 'this' means this object
    //     |                   |                    |------------------------  name of the child thread function
    //     |                   |---------------------------------------------  name of this classs
    //     |-----------------------------------------------------------------  standard thread object

    std::thread thread2(&ClassWithThreads::PrintMessageThread, this, msg2);


    //	create a 3rd thread in suspended state. No data is passed to this thread
    cpen333::thread::suspended_thread thread3(&ClassWithThreads::DisplayIdThread, this) ;

    // wait for the above active threads to complete
    thread1.join();
    thread2.join();

    // resume the 3rd thread and wait for it to complete
    thread3.start();
    thread3.join();
  }

};


int main() {

  // create 1st instance of the above thread object on the heap using new and delete
  // then allow it to run and then wait for it to terminate
  ClassWithThreads	*object1 = new ClassWithThreads(5) ;
  object1->start();  // start the class

  std::cout << "Program main(): Waiting for Child object1 threads to terminate....." << std::endl;
  object1->join();

  delete object1 ;		// object1 destructor called here

  // create 2nd instance of the above active object on the stack
  // then allow it to run and hen wait for it to terminate
  ClassWithThreads	object2(10);
  object2.start();

  std::cout << "Program main(): Waiting for Child object2 threads to terminate....." << std::endl;
  object2.join();

  std::cout << "Finished" << std::endl;
  return 0;
}