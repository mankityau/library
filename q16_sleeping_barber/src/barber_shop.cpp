#include <iostream>
#include <thread>
#include "BarberShop.h"
#include "cpen333/process/condition.h"

int main() {

  BarberShop barber_shop(BARBER_SHOP_NAME, BARBER_SHOP_CHAIRS);
  cpen333::process::condition barbershop_opened(BARBER_SHOP_NAME);

  std::cout << "Opening barber shop" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  barber_shop.Open();  // start accepting customers and barbers
  barbershop_opened.notify();  // open gate

  // let's stay open for about 20 seconds
  std::this_thread::sleep_for(std::chrono::seconds(20));

  std::cout << "Closing barber shop" << std::endl;
  // detach name
  barber_shop.Close();

  // unlink condition to free resource
  barbershop_opened.unlink();

  return 0;
}