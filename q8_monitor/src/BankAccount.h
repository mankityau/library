#ifndef BANKACCOUNT_H
#define BANKACCOUNT_H

#include "cpen333/process/shared_memory.h"
#include "cpen333/process/mutex.h"

// Random number for checking if our bank account has been initialized
#define BANK_ACCOUNT_INITIALIZED 0x89812312

class BankAccount   {

 private:
  struct BankData {
    double balance ;	// the data to be protected
    int initialized;    // special marker for ensuring our bank account is initialized
  };

  cpen333::process::shared_object<BankData> data_;
  cpen333::process::mutex mutex_;

 public:

  /**
   * Safely withdraw funds if there is sufficient money in the bank
   * @param amount amount to withdraw
   * @return true if successful, false if not sufficient funds
   */
  bool WithdrawFunds (double amount) {
    std::lock_guard<cpen333::process::mutex> lock(mutex_);

    bool status = false;
    if (data_->balance >= amount) {
      status = true;
      data_->balance -= amount;
    }
    return status ;
  }

  /**
   * Add funds to bank account
   * @param amount amount to deposit
   */
  void DepositFunds (double amount) {
    std::lock_guard<cpen333::process::mutex> lock(mutex_); // protect data
    data_->balance += amount;
  }

  /**
   * Retrieve bank balance
   * @return your current savings, treat with care
   */
  double GetBalance() {
    std::lock_guard<cpen333::process::mutex> lock(mutex_);  // this will automatically release mutex after return
    return data_->balance;
  }

  /**
   * Set a new balance in your bank account
   * @param newBalance are you a world-class hacker?  How can you do this without depositing money?
   */
  void SetBalance(double new_balance) {
    std::lock_guard<cpen333::process::mutex> lock(mutex_);
    data_->balance = new_balance;
  }

  // constructor and destructor
  BankAccount (const std::string &name) :
      mutex_{name}, data_{name} {

    // maybe initialize bank account data
    std::lock_guard<cpen333::process::mutex> lock(mutex_);
    if (data_->initialized != BANK_ACCOUNT_INITIALIZED) {
      data_->balance = 0;
      data_->initialized = BANK_ACCOUNT_INITIALIZED;
    }

  }

  ~BankAccount () {}

  /**
   * Unlink the bank account, preventing others from ever accessing it
   */
  void Unlink() {
    // unlink named resources
    mutex_.unlink();
    data_.unlink();
  }

} ;

#endif //BANKACCOUNT_H
