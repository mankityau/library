#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

// condition class using a condition_variable
class condition {

 public:
  // start with gate closed
  condition(): open_{false}, cv_{}, mutex_{} {}

  void wait() {
    // wait on condition variable until gate is open
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [&](){return open_;});
  }

  void notify() {
    // protect data and open gate
    {
      std::lock_guard<std::mutex> lock(mutex_);
      open_ = true;
    }
    // notify all that gate is now open
    cv_.notify_all();
  }

  // close gate
  void reset() {
    // protect data and close gate
    std::lock_guard<std::mutex> lock(mutex_);
    open_ = false;
  }

 private:
  bool open_;  // gate
  std::condition_variable cv_;
  std::mutex mutex_;
};

// pedestrian thread function
void pedestrian(int id, condition& safetowalk) {
  // record current time for termination condition
  using clock = std::chrono::steady_clock;
  auto start_time = clock::now();

  // loop for 60 seconds
  while(std::chrono::duration_cast<std::chrono::seconds>(clock::now()-start_time).count() < 60) {
    std::this_thread::sleep_for(std::chrono::milliseconds(600*id));		// sleep for a time period
    safetowalk.wait() ;				 // wait for walk sign
    std::cout << "P" << id << " ";   // show that I have crossed
    std::cout.flush();
  }
}

// car thread function
void car(int id, condition& safetodrive) {

  // record current time for termination condition
  using clock = std::chrono::steady_clock;
  auto start_time = clock::now();

  // loop for 60 seconds
  while(std::chrono::duration_cast<std::chrono::seconds>(clock::now()-start_time).count() < 60) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300*id));		// sleep for a time period
    safetodrive.wait();              // wait for green light
    std::cout << "C" << id << " ";   // show that I have crossed
    std::cout.flush();
  }
}

int main() {

  std::thread *pedestrians[10]; // 10 pedestrians
  std::thread *cars[10];       // 10 cars

  // conditions
  condition safetowalk;
  condition safetodrive;

  for(int i = 0; i < 10; i++) {
    pedestrians[i] = new std::thread(pedestrian, i+1,std::ref(safetowalk));
    cars[i]  = new std::thread(car, i+1, std::ref(safetodrive));
  }

  // loop for about a minute
  using clock = std::chrono::steady_clock;
  auto start_time = clock::now();
  while(std::chrono::duration_cast<std::chrono::seconds>(clock::now()-start_time).count() < 60)	{
    safetowalk.reset();                                    // stop pedestrians
    std::cout << std::endl <<  "No Walking" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));   // wait a suitable time delay between lights changing
    std::cout << "Green Light" << std::endl;
    safetodrive.notify();                                   // allow cars to drive over pedestrian crossing
    std::this_thread::sleep_for(std::chrono::seconds(10));  // wait a suitable time delay between lights changing
    safetodrive.reset();                                    // stop cars
    std::cout << std::endl << "Red Light" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));   // wait a suitable time delay between lights changing
    std::cout << "Walk on" << std::endl;
    safetowalk.notify();                                   // allow pedestrians to cross
    std::this_thread::sleep_for(std::chrono::seconds(15));  // allow pedestrians to cross for 15 seconds

  }

  // clean up memory
  for (int i=0; i<10; ++i) {
    pedestrians[i]->join();
    cars[i]->join();
    delete pedestrians[i];
    delete cars[i];
  }

  std::cout << "Done." << std::endl;

  return 0;
}