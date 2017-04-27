#include <iostream>
#include "BankAccount.h"

int main() {

  BankAccount forfun("Jamie");

  for (int i=0; i<7; ++i) {
    if (forfun.WithdrawFunds(30)) {
      std::cout << "  Spent $30" << std::endl;
    } else {
      int balance = forfun.GetBalance();
      if (balance > 0) {
        std::cout << "  Uh oh, only " << balance << " left, withdrawing..." << std::endl;
        forfun.WithdrawFunds(balance);
      } else {
        std::cout << "  Out of money :(." << std::endl;
        break; // break out of loop
      }
    }
  }

  return 0;
}