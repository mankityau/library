#include "BarberShop.h"
#include "cpen333/process/subprocess.h"
#include <string>
#include <thread>
#include "cpen333/process/condition.h"

int main() {

  // barbershop
  cpen333::process::condition barbershop_opened(BARBER_SHOP_NAME);

  cpen333::process::subprocess barbershop({"./BarberShop"}, true, true);
  barbershop_opened.wait();  // wait until shop opened

  // a couple barbers
  cpen333::process::subprocess barber1({"./Barber", "Michael"}, true, true);
  cpen333::process::subprocess barber2({"./Barber", "Michelle"}, true, true);

  // a bunch of customers
  cpen333::process::subprocess customer1({"./Customer", "Hank"}, true, true);
  cpen333::process::subprocess customer2({"./Customer", "Homer"}, true, true);
  cpen333::process::subprocess customer3({"./Customer", "Henry"}, true, true);
  cpen333::process::subprocess customer4({"./Customer", "Hope"}, true, true);
  cpen333::process::subprocess customer5({"./Customer", "Heather"}, true, true);
  cpen333::process::subprocess customer6({"./Customer", "Holly"}, true, true);
  cpen333::process::subprocess customer7({"./Customer", "Hubert"}, true, true);
  cpen333::process::subprocess customer8({"./Customer", "Helen"}, true, true);
  cpen333::process::subprocess customer9({"./Customer", "Hugh"}, true, true);
  cpen333::process::subprocess customer10({"./Customer", "Hayden"}, true, true);

  // purposely leave processes open so you can add your own via command-line
  return 0;
}