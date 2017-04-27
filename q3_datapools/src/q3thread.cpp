#include <iostream>
#include <thread>    // for spawning threads

// data pools in threads are much easier to work with, since they can access shared memory
struct DataPoolData {
  int floor;            // floor corresponding to lifts current position
  int direction;        // direction of travel of lift
  int floors[10];       // an array representing the floors and whether requests are set
};

// create a function to run in thread that takes a reference to the shared data pool
void ThreadFunction(const std::string &name, DataPoolData& data) {
  // print out the data in the datapool that was stored there by the parent
  std::cout << "Child " << name << " Read value for Floor = " << data.floor << std::endl;
  std::cout << "Child " << name << " Read value for Direction = " << data.direction << std::endl;

  std::cout << "Child " << name << " Read values for Floors array = ";
  for(int i=0; i < 10; i ++) {
    std::cout << data.floors[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Press Enter to quit child " << name << std::endl;
  std::cin.get();  // pause so programmer can view output before child dies

}

int main() {
  
  DataPoolData data;
  
  std::cout << "Parent Writing value '10' to floor variable....." << std::endl;
  data.floor = 10;        // store 55 into the variable 'floor' in the datapool

  std::cout << "Parent Writing value '1' to direction variable....." << std::endl;
  data.direction = 1;     // store 1 into the variable 'direction' in the datapool

  std::cout << "Parent Writing value '0 1 2 3 4 5 6 7 8 9' to floors array....." << std::endl;
  for (int i = 0; i < 10; i++) {
    data.floors[i] = i;
  }

  //	Now that we have created the data pool and have stored data in it, it is safe to create
  //	a child thread that can access the data
  std::cout << "Press Enter to launch thread" << std::endl;
  std::cin.get();


  // Spawn child thread, pass it the shared data by reference
  // NOTE: thread functions require you to explicitly mark variables passed by reference using std::ref()
  // NOTE XXXX: ANYTHING PASSED BY REFERENCE MUST REMAIN IN SCOPE UNTIL THE THREADS COMPLETE
  //            Otherwise, when the thread tries to access the data, it may have been deleted off the stack already
  std::thread t1(ThreadFunction, "Thread1", std::ref(data));
  // wait for thread to complete
  t1.join();

  // ADVANCED:
  // You can also use a lambda expression to handle passing by reference
  // this lambda "captures" data by-reference, then calls the ThreadFunction on it
  auto lambda = [&data](const std::string& name) {
    ThreadFunction(name, data);
  };
  // lambda has only one argument now
  std::thread t2(lambda, "Thread2");
  t2.join();

}