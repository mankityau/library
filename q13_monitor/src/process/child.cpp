#include <iostream>
#include "BankAccount.h"

int main() {

  BankAccount forfun("Jamie");

  // I need money to spend
  std::cout << "Waiting for money to be put into account" << std::endl;
  forfun.WaitForMoney();

  for (int i=0; i<7; ++i) {
    if (forfun.WithdrawFunds(40)) {
      std::cout << "  Spent $40" << std::endl;
    } else {
      int balance = forfun.GetBalance();
      if (balance > 0) {
        std::cout << "  Uh oh, only " << balance << " left, withdrawing..." << std::endl;
        forfun.WithdrawFunds(balance);
      } else {
        std::cout << "  Out of money :(." << std::endl;
        return 0; // break out of loop
      }
    }
  }

  // end of the week, spend all my money
  int balance = forfun.GetBalance();
  forfun.WithdrawFunds(balance);
  std::cout << "  Spending all my extra money :), $" << balance << std::endl;

  return 0;
}