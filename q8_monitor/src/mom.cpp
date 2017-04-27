#include <iostream>
#include "BankAccount.h"
#include "cpen333/process/subprocess.h"

int main() {

  BankAccount forschool("Jamie");

  forschool.SetBalance(500);

  // every week put 200 in
  for (int i=0; i<52; ++i) {
    forschool.DepositFunds(200);
    cpen333::process::subprocess bigspender({"./child"});
    std::cout << "Balance: " << forschool.GetBalance() << std::endl;
    bigspender.join();  // wait for child
  }

  // terminate account
  forschool.Unlink();

  std::cout << "Done." << std::endl;

}