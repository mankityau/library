#include <iostream>
#include <thread>
#include "BankAccount.h"
#include "cpen333/util.h"
#include "cpen333/process/subprocess.h"


//
// A "Monitor" is an object that ensures safe concurrent access, providing mutual exclusion internally when needed.
// It also usually provides methods for waiting until certain conditions are met, such as the availability of a
// resource.
//

int main() {

  // BankAccount is a monitor class that provides safe access for depositing/withdrawing funds, and checking balances.
  BankAccount forschool("Jamie");

  // every week put 200 in
  for (int i=0; i<52; ++i) {
    cpen333::process::subprocess bigspender({"./child"}, true, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "Giving child $300 for school" << std::endl;
    forschool.DepositFunds(300);
    // wait until child is out of money or done for the week
    std::cout << "Waiting until child has spent all money" << std::endl;
    forschool.WaitForBankrupt();
    bigspender.join();  // wait for child
  }

  // terminate account
  forschool.Unlink();

  std::cout << "Done." << std::endl;
  cpen333::pause();

}