#include <iostream>
#include <thread>

#include "CookieQueue.h"
#include "cpen333/process/subprocess.h"

int main() {

  // create a cookie queue and unlinker to unlink cookie when out of scope
  CookieQueue::Unlink(COOKIE_QUEUE_NAME);  // free name

  CookieQueue queue(COOKIE_QUEUE_NAME, 32);

  // start 3 bakers
  cpen333::process::subprocess baker1({"./Baker", "1", "CHOCOLATE_CHIP"}, true, false);
  cpen333::process::subprocess baker2({"./Baker", "2", "OATMEAL_RAISIN"}, true, false);
  cpen333::process::subprocess baker3({"./Baker", "3", "GINGER_SNAP"}, true, false);

  // start 3 cookie monsters
  cpen333::process::subprocess monster1({"./CookieMonster", "1"}, true, true);
  cpen333::process::subprocess monster2({"./CookieMonster", "2"}, true, true);
  cpen333::process::subprocess monster3({"./CookieMonster", "3"}, true, true);

  // wait for bakers to finish
  baker1.join();
  baker2.join();
  baker3.join();

  // tell monsters there will be no more cookies, one for each monster
  queue.Push({CookieType::POISON, -1, -1});
  queue.Push({CookieType::POISON, -1, -1});
  queue.Push({CookieType::POISON, -1, -1});

  // wait for monsters to finish
  monster1.join();
  monster2.join();
  monster3.join();

  queue.Unlink(); // unlink names to free them on POSIX systems

  std::cout << "Goodbye." << std::endl;
  return 0;
}