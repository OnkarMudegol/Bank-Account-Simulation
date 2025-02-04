#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iomanip>

// Custom exception for banking-related errors
class BankingException : public std::runtime_error {
public:
    BankingException(const std::string& message) 
        : std::runtime_error(message) {}
};

// Abstract Base Class for Bank Accounts
class BankAccount {
protected:
    std::string accountNumber;
    std::string accountHolderName;
    double balance;

public:
    // Constructor
    BankAccount(const std::string& number, 
                const std::string& name, 
                double initialBalance = 0.0) 
        : accountNumber(number), 
          accountHolderName(name), 
          balance(initialBalance) {
        if (initialBalance < 0) {
            throw BankingException("Initial balance cannot be negative");
        }
    }

    // Virtual destructor for proper inheritance
    virtual ~BankAccount() = default;

    // Pure virtual method to be implemented by derived classes
    virtual void applyMonthlyFee() = 0;

    // Common methods
    virtual void deposit(double amount) {
        if (amount <= 0) {
            throw BankingException("Deposit amount must be positive");
        }
        balance += amount;
    }

    virtual bool withdraw(double amount) {
        if (amount <= 0) {
            throw BankingException("Withdrawal amount must be positive");
        }
        if (amount > balance) {
            return false;
        }
        balance -= amount;
        return true;
    }

    // Getters
    double getBalance() const { return balance; }
    std::string getAccountNumber() const { return accountNumber; }
    std::string getAccountHolderName() const { return accountHolderName; }

    // Virtual method to display account details
    virtual void displayAccountInfo() const {
        std::cout << "Account Number: " << accountNumber << std::endl;
        std::cout << "Account Holder: " << accountHolderName << std::endl;
        std::cout << "Balance: $" << std::fixed << std::setprecision(2) << balance << std::endl;
    }
};

// Checking Account Derivation
class CheckingAccount : public BankAccount {
private:
    static constexpr double MONTHLY_FEE = 10.0;
    static constexpr double OVERDRAFT_LIMIT = 100.0;

public:
    CheckingAccount(const std::string& number, 
                    const std::string& name, 
                    double initialBalance = 0.0)
        : BankAccount(number, name, initialBalance) {}

    bool withdraw(double amount) override {
        if (amount <= 0) {
            throw BankingException("Withdrawal amount must be positive");
        }
        
        // Allow overdraft up to a limit
        if (amount > balance + OVERDRAFT_LIMIT) {
            return false;
        }
        
        balance -= amount;
        return true;
    }

    void applyMonthlyFee() override {
        if (balance >= MONTHLY_FEE) {
            balance -= MONTHLY_FEE;
        }
    }

    void displayAccountInfo() const override {
        BankAccount::displayAccountInfo();
        std::cout << "Account Type: Checking" << std::endl;
        std::cout << "Monthly Fee: $" << MONTHLY_FEE << std::endl;
    }
};

// Savings Account Derivation
class SavingsAccount : public BankAccount {
private:
    static constexpr double INTEREST_RATE = 0.05;
    static constexpr double MINIMUM_BALANCE = 100.0;

public:
    SavingsAccount(const std::string& number, 
                   const std::string& name, 
                   double initialBalance = 0.0)
        : BankAccount(number, name, initialBalance) {
        if (initialBalance < MINIMUM_BALANCE) {
            throw BankingException("Minimum initial balance for Savings is $" + 
                                   std::to_string(MINIMUM_BALANCE));
        }
    }

    void applyMonthlyFee() override {
        // Instead of a fee, apply interest
        balance += balance * INTEREST_RATE;
    }

    void displayAccountInfo() const override {
        BankAccount::displayAccountInfo();
        std::cout << "Account Type: Savings" << std::endl;
        std::cout << "Interest Rate: " << INTEREST_RATE * 100 << "%" << std::endl;
    }
};

// Bank Management System
class Bank {
private:
    std::vector<std::unique_ptr<BankAccount>> accounts;

public:
    // Add a new account
    void addAccount(std::unique_ptr<BankAccount> account) {
        accounts.push_back(std::move(account));
    }

    // Find account by account number
    BankAccount* findAccount(const std::string& accountNumber) {
        for (const auto& account : accounts) {
            if (account->getAccountNumber() == accountNumber) {
                return account.get();
            }
        }
        return nullptr;
    }

    // Process monthly updates for all accounts
    void processMonthlyUpdates() {
        for (auto& account : accounts) {
            account->applyMonthlyFee();
        }
    }

    // Display all accounts
    void displayAllAccounts() {
        for (const auto& account : accounts) {
            account->displayAccountInfo();
            std::cout << "------------------------" << std::endl;
        }
    }
};

// Main demonstration
int main() {
    Bank myBank;

    try {
        // Create different account types
        myBank.addAccount(std::make_unique<CheckingAccount>("CH001", "John Doe", 500.0));
        myBank.addAccount(std::make_unique<SavingsAccount>("SV001", "Jane Smith", 1000.0));

        // Demonstrating operations
        BankAccount* johnAccount = myBank.findAccount("CH001");
        if (johnAccount) {
            std::cout << "Initial John's Account Balance: $" 
                      << johnAccount->getBalance() << std::endl;
            
            johnAccount->deposit(200.0);
            johnAccount->withdraw(50.0);
            
            std::cout << "Updated John's Account Balance: $" 
                      << johnAccount->getBalance() << std::endl;
        }

        // Process monthly updates
        myBank.processMonthlyUpdates();

        // Display all accounts after updates
        std::cout << "\nAccounts after monthly updates:" << std::endl;
        myBank.displayAllAccounts();

    } catch (const BankingException& e) {
        std::cerr << "Banking Error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected Error: " << e.what() << std::endl;
    }

    return 0;
}