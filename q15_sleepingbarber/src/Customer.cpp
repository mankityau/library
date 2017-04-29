#include <iostream>
#include <string>
#include <thread>

#include "BarberShop.h"

int main(int argc, char* argv[]) {

  // grab name from argv
  std::string name = "Customer";
  if (argc > 1) {
    name = argv[1];
  }

  // load barbershop
  BarberShop barbershop(BARBER_SHOP_NAME, BARBER_SHOP_CHAIRS);

  for (int i=0; i<20; ++i) {
    std::cout << "Customer " << name << " arriving for haircut" << std::endl;
    if (!barbershop.CustomerWaitsForHaircut()) {
      std::cout << name << " turned away :(" << std::endl;
      if (!barbershop.Opened()) {
        break;  // leave if shop is now closed
      }
      --i;  // try again
    } else {
      std::cout << name << "'s haircut is complete" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  }

  return 0;
}