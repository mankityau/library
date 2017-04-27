#include "CookieQueue.h"

#include <thread>
#include <string>

std::string get_cookie_name(CookieType type) {
  switch (type) {
    case CHOCOLATE_CHIP: {
      return "CHOCOLATE CHIP";
    }
    case OATMEAL_RAISIN: {
      return "OATMEAL RAISIN";
    }
    case GINGER_SNAP: {
      return "GINGER SNAP";
    }
  }
  return "UNKNOWN";
}


int main(int argc, char* argv[]) {

  // detect baker id from command line
  int monster_id = 0;
  if (argc > 1) {
    monster_id = atoi(argv[1]);
  }

  CookieQueue queue(COOKIE_QUEUE_NAME, 256);

  //while(true) { // keep eating cookies forever
  for (int i=0; i<10000; ++i) {
    Cookie cookie;

    if (queue.TryPop(cookie)) {
      if (cookie.type == CookieType::POISON) {
        std::cout << "Monster " << monster_id << " died from a POISON Cookie" << std::endl;
        break;
      } else {
        std::cout << "Monster " << monster_id << " ate a " << get_cookie_name(cookie.type)
                  << " cookie made by baker " << cookie.baker_id << std::endl;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

//  std::cout << "Press ENTER to Quit Monster" << std::endl;
//  std::cin.get();

  std::cout << "Cookie Monster Finished" << std::endl;
  return 0;
}