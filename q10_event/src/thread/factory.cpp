#include <iostream>
#include <thread>

#include "cpen333/thread/event.h"
#include "cpen333/thread/thread_object.h"

//
//   An event is just like a "condition", except that it auto-resets once other threads/processes pass through.  It also
//   has the option to either let one thread pass, notify_one(), or all currently waiting threads pass, notify_all().
//

// Production Line
class ProductionLine : public cpen333::thread::thread_object {
 private:
  cpen333::thread::event &chassis_ready_;
  cpen333::thread::event &robot_finished_;
  
 public:
  ProductionLine(cpen333::thread::event &chassis_ready, cpen333::thread::event &robot_finished) :
      chassis_ready_{chassis_ready}, robot_finished_{robot_finished} {}

 private:
  // main method
  void run() {
    // make 20 cars
    for (int i=0; i<20; ++i) {
      std::cout << "Production Line: Waiting for next chassis" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(2500));
      std::cout << "Production Line: New Chassis Arrived" << std::endl;
      chassis_ready_.notify_one();  // signal one robot that a chassis is ready
      robot_finished_.wait();       // wait until robot is done
      std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    }
  }
  
};

// Robot
class Robot : public cpen333::thread::thread_object {
 private:
  cpen333::thread::event &chassis_ready_;
  cpen333::thread::event &robot_finished_;


 public:
  Robot(cpen333::thread::event &chassis_ready, cpen333::thread::event &robot_finished) :
      chassis_ready_{chassis_ready}, robot_finished_{robot_finished} {}

 private:
  // main method
  void run() {
    // make 20 cars
    for (int i=0; i<20; ++i) {
      std::cout << "Robot: Waiting for Next Chassis" << std::endl;
      chassis_ready_.wait();   // wait for next chassis
      std::cout << "Robot: Assembling Chassis" << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(4));   // delay while we assemble car
      std::cout << "Robot: Finished Assembly" << std::endl;
      robot_finished_.notify_one();  // robot has finished assembling car
    }
  }

};

int main() {

  // create events
  cpen333::thread::event chassis_ready;
  cpen333::thread::event robot_finished;

  std::cout << "Factory started..." << std::endl;

  // robot must start waiting first
  Robot robot(chassis_ready, robot_finished);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // create a production line and robot, wait for them to finish
  ProductionLine production_line(chassis_ready, robot_finished);

  // wait for thread to complete
  production_line.join();
  robot.join();

  std::cout << "Done." << std::endl;

}