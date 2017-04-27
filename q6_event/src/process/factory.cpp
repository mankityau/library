#include <iostream>
#include <thread>
#include "cpen333/process/subprocess.h"

#include "factory.h"
#include "cpen333/process/event.h"
#include "cpen333/process/unlinker.h"

int main() {

  // create events and set to unlink when this process terminates
  cpen333::process::event chassis_ready(CHASSIS_READY_EVENT);
  cpen333::process::event robot_finished(ROBOT_FINISHED_EVENT);
  cpen333::process::unlinker<decltype(chassis_ready)> cunlink(chassis_ready);
  cpen333::process::unlinker<decltype(robot_finished)> runlink(robot_finished);


  std::cout << "Factory started..." << std::endl;



  // create a production line and robot, wait for them to finish
  cpen333::process::subprocess robot({"./robot"});
  std::this_thread::sleep_for(std::chrono::seconds(1));  // robot must start waiting first
  cpen333::process::subprocess production_line({"./production_line"});

  production_line.join();
  robot.join();

  std::cout << "Done." << std::endl;

  return 0;

}