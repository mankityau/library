#include <iostream>
#include <thread>   // for std::thread

// a thread function
void child_thread (int thread_number) {

  for ( int i = 0; i < 1000; i ++) {
    std::cout << "I am the Child thread: My thread number is [" << thread_number << "]" << std::endl;
  }

}

int main() {

  // an array of integers
  int id[8] = {1,2,3,4,5,6,7,8};

  // an array of thread POINTERS
  std::thread *threads[8] ;

  // let's create 8 instances of our thread code, letting it know what number it is
  for (int i = 0; i < 8; ++i) {
    std::cout << "Parent Thread: Creating Child Thread " << id[i] << std::endl;
    threads[i] = new std::thread(child_thread, id[i]);
  }

  // wait for threads to terminate, then delete thread objects we created above
  for(int j = 0; j < 8; ++j) {
    threads[j]->join();
    delete threads[j] ;	// delete the object created by ‘new’
  }

  return 0 ;
}