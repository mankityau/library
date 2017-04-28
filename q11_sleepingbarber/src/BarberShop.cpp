#include <iostream>
#include "BarberShop.h"
#include "cpen333/process/condition.h"

int main() {

  BarberShop barber_shop(BARBER_SHOP_NAME, BARBER_SHOP_CHAIRS);
  cpen333::process::condition barbershop_opened(BARBER_SHOP_NAME);

  barber_shop.Open();  // start accepting customers and barbers
  barbershop_opened.notify();  // open gate

  std::cout << "Opening barber shop" << std::endl;
  std::cout << "Press 'Q' and ENTER to quit." << std::endl;
  bool quit = false;
  while (!quit) {
    char c = std::cin.get();
    if (c == 'Q') {
      quit = true;
    }
  }

  std::cout << "Closing barber shop" << std::endl;
  // detach name
  barber_shop.Close();

  return 0;
}