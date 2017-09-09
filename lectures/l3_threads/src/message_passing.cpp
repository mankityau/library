#include <cpen333/thread/thread_object.h>
#include <thread>
#include <chrono>

// object-oriented thread that can count
class CountingThread : public cpen333::thread::thread_object {
  int counter_;  // private counter variable
 public:

  // constructor
  CountingThread() : counter_(0) {}

  // method for message-passing
  void increment() {
    counter_++;
  }

 protected:
  int main() {
    while(counter_ < 10) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      std::cout << "count is " << counter_ << std::endl;
    }
    return counter_;
  }
};

class CounterTrigger : public cpen333::thread::thread_object {
  CountingThread* counter_;

 public:
  // constructor
  CounterTrigger(CountingThread* counter) : counter_(counter) {}

 protected:
  int main() {
    counter_->start();
    while (!counter_->terminated()) {
      counter_->increment();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    // get result of counter
    int result = counter_->join();

    return result;
  }

};

int main() {

  CountingThread counter;
  CounterTrigger trigger(&counter);
  int result = trigger.join();

  std::cout << "trigger's return value: " << result << std::endl;

  return 0;
}