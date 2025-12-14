// FINALPROJECT_OOP.cpp
// OOP refactor of the Bank Management System
// Single-file implementation (Person, User, Admin, Account, Bank)

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>   // getch()
#include <Windows.h> // optional (kept from original)
#include <limits>

using namespace std;

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define BLUE  "\033[34m"
#define RESET "\033[0m"

// -----------------------------
// Helper validation functions
// -----------------------------
bool is_valid_pin(int pin) {
    return pin >= 1000 && pin <= 9999;
}

bool is_valid_amount(int amount) {
    return amount > 0;
}

bool is_valid_cnic(const string& cnic) {
    if (cnic.length() != 13) return false;
    for (char ch : cnic) if (ch < '0' || ch > '9') return false;
    return true;
}

// reads an integer safely
int read_int() {
    int x;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << RED << "Invalid input. Enter a number: " << RESET;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return x;
}

// reads a line safely
string read_line() {
    string s;
    getline(cin, s);
    return s;
}

// pause helper (keeps getch from original)
void pause() {
    cout << GREEN << "\nPress any key to continue..." << RESET;
    _getch();
    cout << endl;
}

// draw banner
void displayBMSBanner() {
    // system("CLS"); // optional
    cout << BLUE << "\t\t\t\t===========================================" << endl;
    cout << BLUE << "\t\t\t\t|        BANK MANAGEMENT SYSTEM           |" << endl;
    cout << BLUE << "\t\t\t\t===========================================" << endl;
    cout << BLUE << "\t\t\t\t|  Welcome to the Bank Management System! |" << endl;
    cout << BLUE << "\t\t\t\t|    Secure, Reliable, and Efficient.     |" << endl;
    cout << BLUE << "\t\t\t\t===========================================" << RESET << endl << endl;
}

// -----------------------------
// Account class
// -----------------------------
class Account {
private:
    string user_name;
    string cnic;
    unsigned long long acc_no;
    int pin;
    string DOB;
    int balance;
    bool is_deactivated;

public:
    Account() : acc_no(0), pin(0), balance(0), is_deactivated(false) {}

    // setters/getters
    void setUserName(const string& n) { user_name = n; }
    string getUserName() const { return user_name; }

    void setCNIC(const string& c) { cnic = c; }
    string getCNIC() const { return cnic; }

    void setAccNo(unsigned long long a) { acc_no = a; }
    unsigned long long getAccNo() const { return acc_no; }

    void setPin(int p) { pin = p; }
    int getPin() const { return pin; }

    void setDOB(const string& d) { DOB = d; }
    string getDOB() const { return DOB; }

    void setBalance(int b) { balance = b; }
    int getBalance() const { return balance; }

    void setDeactivated(bool v) { is_deactivated = v; }
    bool getDeactivated() const { return is_deactivated; }

    // operations
    void deposit(int amount) {
        balance += amount;
    }

    bool withdraw(int amount) {
        if (amount <= balance) { balance -= amount; return true; }
        return false;
    }

    // serialization for file storage (each field on its own line)
    string serialize() const {
        // fields: user_name,cnic,acc_no,pin,DOB,balance,is_deactivated
        // for safety, replace newlines in user_name (unlikely)
        string s = user_name + "\n" + cnic + "\n";
        s += to_string(acc_no) + "\n";
        s += to_string(pin) + "\n";
        s += DOB + "\n";
        s += to_string(balance) + "\n";
        s += (is_deactivated ? "1\n" : "0\n");
        return s;
    }

    // static deserialization (reads from stream) - returns false if EOF encountered prematurely
    static bool deserialize(istream& in, Account& out) {
        string line;
        if (!getline(in, out.user_name)) return false;
        if (!getline(in, out.cnic)) return false;

        if (!getline(in, line)) return false;
        out.acc_no = stoull(line);

        if (!getline(in, line)) return false;
        out.pin = stoi(line);

        if (!getline(in, out.DOB)) return false;

        if (!getline(in, line)) return false;
        out.balance = stoi(line);

        if (!getline(in, line)) return false;
        out.is_deactivated = (line == "1");

        return true;
    }
};

// -----------------------------
// Person base class
// -----------------------------
class Person {
protected:
    string name;
public:
    Person() {}
    Person(const string& n) : name(n) {}
    virtual ~Person() {}
    virtual void showMenu() = 0; // abstract
    string getName() const { return name; }
};

// Forward declaration
class Bank;

// -----------------------------
// User class (inherits Person)
// -----------------------------
class User : public Person {
protected:
    Bank* bank;        // pointer to bank to perform operations
    int currentIndex;  // index of logged in account in bank's vector
public:
    User(Bank* b = nullptr) : bank(b), currentIndex(-1) {}
    virtual ~User() {}
    // login by PIN (keeps behavior similar to original)
    virtual bool login();
    virtual void showMenu() override;
    virtual void userOperations(); // operations loop
};

// -----------------------------
// Admin class
// -----------------------------
class Admin : public Person {
    Bank* bank;
public:
    Admin(Bank* b = nullptr) : bank(b) {}
    virtual ~Admin() {}
    bool login(); // admin login
    virtual void showMenu() override;
};

// -----------------------------
// Bank class: manages accounts and business logic
// -----------------------------
class Bank {
private:
    vector<Account> accounts;
    int current_user_index = -1;
    const string filename = "accounts.txt";

public:
    Bank() { srand((unsigned int)time(nullptr)); }

    // file I/O
    void load_accounts_from_file() {
        accounts.clear();
        ifstream infile(filename);
        if (!infile.is_open()) {
            cout << RED << "No previous account data found. Starting fresh.\n" << RESET;
            return;
        }

        string header;
        if (!getline(infile, header)) {
            infile.close();
            return;
        }
        // header may be number of accounts (legacy) but we'll read until EOF
        // try to parse using Account::deserialize
        infile.clear();
        infile.seekg(0);
        // if first line is a number, skip it
        long long maybeNum;
        if ( (infile >> maybeNum) && infile.peek() == '\n') {
            // we consumed the number (legacy format) -> continue
        } else {
            // rewind
            infile.clear();
            infile.seekg(0);
        }
        // Now read properly using deserialize
        infile.ignore(numeric_limits<streamsize>::max(), '\n'); // ensure line alignment if we parsed number
        // If EOF after that then nothing.
        infile.clear();
        infile.seekg(0);
        // We'll attempt to read using a simple approach: read whole file lines and parse groups of 7 lines
        vector<string> lines;
        string ln;
        while (getline(infile, ln)) {
            if (!ln.empty() || ln == "") lines.push_back(ln);
        }
        infile.close();
        // If file is empty -> nothing
        if (lines.empty()) return;

        // If file was written by our new serialize format, it contains sequences of 7 lines per account.
        // Try to parse sequences of 7 lines.
        size_t i = 0;
        while (i + 6 < lines.size()) {
            Account a;
            a.setUserName(lines[i]);
            a.setCNIC(lines[i+1]);
            a.setAccNo(stoull(lines[i+2]));
            a.setPin(stoi(lines[i+3]));
            a.setDOB(lines[i+4]);
            a.setBalance(stoi(lines[i+5]));
            a.setDeactivated(lines[i+6] == "1");
            accounts.push_back(a);
            i += 7;
        }
        // if parsing failed (no accounts loaded) try legacy format (first line = count)
        if (accounts.empty()) {
            ifstream inf2(filename);
            if (!inf2.is_open()) return;
            int count = 0; inf2 >> count;
            inf2.ignore(numeric_limits<streamsize>::max(), '\n');
            for (int k = 0; k < count; ++k) {
                Account a;
                string uname, cnic, dob;
                unsigned long long acc_no;
                int pin, balance;
                int deact;
                if (!getline(inf2, uname)) break;
                if (!getline(inf2, cnic)) break;
                inf2 >> acc_no;
                inf2 >> pin;
                inf2.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(inf2, dob);
                inf2 >> balance;
                inf2 >> deact;
                inf2.ignore(numeric_limits<streamsize>::max(), '\n');
                a.setUserName(uname);
                a.setCNIC(cnic);
                a.setAccNo(acc_no);
                a.setPin(pin);
                a.setDOB(dob);
                a.setBalance(balance);
                a.setDeactivated(deact == 1);
                accounts.push_back(a);
            }
            inf2.close();
        }
    }

    void save_accounts_to_file() {
        ofstream outfile(filename);
        if (!outfile.is_open()) {
            cout << RED << "Failed to open file for saving.\n" << RESET;
            return;
        }
        // write accounts sequentially (7 lines per account)
        for (const Account& a : accounts) {
            outfile << a.serialize();
        }
        outfile.close();
    }

    // account management
    void create_account() {
        displayBMSBanner();
        Account new_account;
        cout << GREEN << "To create an account, please enter your information\n" << RESET;

        cout << GREEN << "Enter your Name: " << RESET;
        string name;
        getline(cin, name);
        while (name.empty()) {
            cout << RED << "Name cannot be empty. Enter your Name: " << RESET;
            getline(cin, name);
        }
        new_account.setUserName(name);

        string cnic;
        cout << GREEN << "Enter your National Identity Card No (13 digits): " << RESET;
        getline(cin, cnic);
        while (!is_valid_cnic(cnic)) {
            cout << RED << "Invalid CNIC. Enter a valid CNIC (13 digits): " << RESET;
            getline(cin, cnic);
        }
        new_account.setCNIC(cnic);

        cout << GREEN << "Enter your Date of Birth (DD/MM/YYYY): " << RESET;
        string dob;
        getline(cin, dob);
        new_account.setDOB(dob);

        // generate account number (11 digits)
        unsigned long long accno = 10000000000ULL + (unsigned long long)(rand() % 90000000000ULL);
        new_account.setAccNo(accno);
        cout << GREEN << "Your Account Number is: " << accno << RESET << endl;

        // set PIN
        cout << GREEN << "Please set a 4-digit PIN for your account: " << RESET;
        int pin;
        pin = read_int();
        while (!is_valid_pin(pin)) {
            cout << RED << "Invalid PIN. Please enter a 4-digit PIN: " << RESET;
            pin = read_int();
        }
        cout << GREEN << "Confirm your PIN: " << RESET;
        int pin2 = read_int();
        while (pin2 != pin) {
            cout << RED << "PINs do not match. Re-enter confirmation: " << RESET;
            pin2 = read_int();
        }
        new_account.setPin(pin);
        cout << GREEN << "PIN set successfully!" << RESET << endl;

        // optional initial deposit
        cout << GREEN << "Do you want to deposit initial money? (1=Yes / 0=No): " << RESET;
        int choice = read_int();
        if (choice == 1) {
            cout << GREEN << "Enter amount to deposit: Rs. " << RESET;
            int amt = read_int();
            while (!is_valid_amount(amt)) {
                cout << RED << "Invalid amount. Enter positive amount: " << RESET;
                amt = read_int();
            }
            new_account.setBalance(amt);
        } else {
            new_account.setBalance(0);
        }

        new_account.setDeactivated(false);
        accounts.push_back(new_account);
        save_accounts_to_file();
        cout << GREEN << "Account created successfully!\n" << RESET;
        pause();
    }

    int find_account_by_pin(int pin) const {
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (accounts[i].getPin() == pin && !accounts[i].getDeactivated()) return (int)i;
        }
        return -1;
    }

    int find_account_by_cnic(const string& cnic) const {
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (accounts[i].getCNIC() == cnic) return (int)i;
        }
        return -1;
    }

    Account* get_account_ptr(int idx) {
        if (idx >= 0 && idx < (int)accounts.size()) return &accounts[idx];
        return nullptr;
    }

    const vector<Account>& get_all_accounts() const { return accounts; }

    void modify_balance_by_admin() {
        displayBMSBanner();
        cout << GREEN << "Enter CNIC of account you want to modify balance of: " << RESET;
        string cnic;
        getline(cin, cnic);
        while (!is_valid_cnic(cnic)) {
            cout << RED << "Invalid CNIC. Enter valid 13-digit CNIC: " << RESET;
            getline(cin, cnic);
        }
        int idx = find_account_by_cnic(cnic);
        if (idx == -1) {
            cout << RED << "Account not found.\n" << RESET;
            pause();
            return;
        }
        cout << GREEN << "Enter new balance: Rs. " << RESET;
        int nb = read_int();
        while (!is_valid_amount(nb) && nb != 0) {
            cout << RED << "Invalid amount. Enter positive amount (or 0): " << RESET;
            nb = read_int();
        }
        accounts[idx].setBalance(nb);
        save_accounts_to_file();
        cout << GREEN << "Balance updated successfully.\n" << RESET;
        pause();
    }

    void change_pin_by_admin() {
        displayBMSBanner();
        cout << GREEN << "Enter CNIC of account to change PIN: " << RESET;
        string cnic;
        getline(cin, cnic);
        while (!is_valid_cnic(cnic)) {
            cout << RED << "Invalid CNIC. Enter valid 13-digit CNIC: " << RESET;
            getline(cin, cnic);
        }
        int idx = find_account_by_cnic(cnic);
        if (idx == -1) {
            cout << RED << "Account not found.\n" << RESET;
            pause();
            return;
        }
        cout << GREEN << "Enter new 4-digit PIN: " << RESET;
        int np = read_int();
        while (!is_valid_pin(np)) {
            cout << RED << "Invalid PIN. Enter 4-digit PIN: " << RESET;
            np = read_int();
        }
        accounts[idx].setPin(np);
        save_accounts_to_file();
        cout << GREEN << "PIN changed successfully.\n" << RESET;
        pause();
    }

    void delete_account() {
        displayBMSBanner();
        cout << GREEN << "Enter CNIC of account to delete: " << RESET;
        string cnic;
        getline(cin, cnic);
        int idx = find_account_by_cnic(cnic);
        if (idx == -1) {
            cout << RED << "Account not found.\n" << RESET;
            pause();
            return;
        }
        accounts.erase(accounts.begin() + idx);
        save_accounts_to_file();
        cout << GREEN << "Account deleted successfully.\n" << RESET;
        pause();
    }

    void activate_account() {
        displayBMSBanner();
        cout << GREEN << "Enter CNIC of account to activate: " << RESET;
        string cnic;
        getline(cin, cnic);
        while (!is_valid_cnic(cnic)) {
            cout << RED << "Invalid CNIC. Enter valid 13-digit CNIC: " << RESET;
            getline(cin, cnic);
        }
        int idx = find_account_by_cnic(cnic);
        if (idx == -1) {
            cout << RED << "Account not found.\n" << RESET;
            pause();
            return;
        }
        if (!accounts[idx].getDeactivated()) {
            cout << GREEN << "Account already active.\n" << RESET;
            pause();
            return;
        }
        cout << GREEN << "Enter PIN to verify: " << RESET;
        int p = read_int();
        if (accounts[idx].getPin() == p) {
            accounts[idx].setDeactivated(false);
            save_accounts_to_file();
            cout << GREEN << "Account activated.\n" << RESET;
        } else {
            cout << RED << "PIN mismatch. Cannot activate.\n" << RESET;
        }
        pause();
    }

    void show_all_accounts() {
        displayBMSBanner();
        cout << GREEN << "Displaying all accounts:\n" << RESET;
        for (const Account& a : accounts) {
            cout << "Account No: " << a.getAccNo() << endl;
            cout << "Name: " << a.getUserName() << endl;
            cout << "Balance: Rs." << a.getBalance() << endl;
            cout << "Deactivated: " << (a.getDeactivated() ? "Yes" : "No") << endl;
            cout << "------------------------------------\n";
        }
        pause();
    }

    void search_account() {
        displayBMSBanner();
        cout << GREEN << "Enter CNIC to search for the account: " << RESET;
        string cnic;
        getline(cin, cnic);
        int idx = find_account_by_cnic(cnic);
        if (idx == -1) {
            cout << RED << "Account not found.\n" << RESET;
        } else {
            const Account& a = accounts[idx];
            cout << GREEN << "Account found:\n" << RESET;
            cout << "Account No: " << a.getAccNo() << endl;
            cout << "Name: " << a.getUserName() << endl;
            cout << "Balance: Rs." << a.getBalance() << endl;
            cout << "Deactivated: " << (a.getDeactivated() ? "Yes" : "No") << endl;
        }
        pause();
    }

    // User-side operations (call with current user index)
    bool login_user_by_pin(int pin) {
        int idx = find_account_by_pin(pin);
        if (idx != -1) {
            current_user_index = idx;
            return true;
        }
        return false;
    }

    void logout_user() {
        current_user_index = -1;
    }

    // deposit for logged-in user
    void deposit_current(int amount) {
        Account* a = get_account_ptr(current_user_index);
        if (!a) return;
        a->deposit(amount);
        save_accounts_to_file();
    }

    bool withdraw_current(int amount) {
        Account* a = get_account_ptr(current_user_index);
        if (!a) return false;
        bool ok = a->withdraw(amount);
        if (ok) save_accounts_to_file();
        return ok;
    }

    int get_current_balance() const {
        const Account* a = (current_user_index >= 0) ? &accounts[current_user_index] : nullptr;
        return a ? a->getBalance() : 0;
    }

    string get_current_username() const {
        const Account* a = (current_user_index >= 0) ? &accounts[current_user_index] : nullptr;
        return a ? a->getUserName() : "";
    }

    void transfer_interbank(const string& recipient_cnic, int amount) {
        int ridx = find_account_by_cnic(recipient_cnic);
        if (ridx == -1) {
            cout << RED << "Recipient account not found.\n" << RESET;
            return;
        }
        Account* sender = get_account_ptr(current_user_index);
        Account* receiver = get_account_ptr(ridx);
        if (!sender || !receiver) return;
        if (amount <= sender->getBalance()) {
            sender->withdraw(amount);
            receiver->deposit(amount);
            save_accounts_to_file();
            cout << GREEN << "Transfer successful! Rs." << amount << " transferred to " << receiver->getUserName() << ".\n" << RESET;
        } else {
            cout << RED << "Insufficient balance to complete the transfer.\n" << RESET;
        }
    }

    void transfer_otherbank(const string& recipient_account, const string& recipient_bank, int amount) {
        Account* sender = get_account_ptr(current_user_index);
        if (!sender) return;
        if (amount <= sender->getBalance()) {
            sender->withdraw(amount);
            save_accounts_to_file();
            cout << GREEN << "Transfer successful! Rs." << amount << " transferred to account " << recipient_account << " at " << recipient_bank << ".\n" << RESET;
        } else {
            cout << RED << "Insufficient balance to complete the transfer.\n" << RESET;
        }
    }

    bool pay_bill(long billno, int amount) {
        Account* a = get_account_ptr(current_user_index);
        if (!a) return false;
        if (amount <= a->getBalance()) {
            a->withdraw(amount);
            save_accounts_to_file();
            cout << GREEN << "Your bill has been paid. Bill Number: " << billno << "\n" << RESET;
            return true;
        } else {
            cout << RED << "Insufficient balance to pay the bill.\n" << RESET;
            return false;
        }
    }

    void deactivate_current_account() {
        Account* a = get_account_ptr(current_user_index);
        if (!a) return;
        a->setDeactivated(true);
        save_accounts_to_file();
        cout << GREEN << "Your account is now deactivated.\n" << RESET;
    }

    void change_pin_current(int new_pin) {
        Account* a = get_account_ptr(current_user_index);
        if (!a) return;
        a->setPin(new_pin);
        save_accounts_to_file();
        cout << GREEN << "Your new PIN has been set.\n" << RESET;
    }

    void modify_balance_by_user(int new_balance) {
        Account* a = get_account_ptr(current_user_index);
        if (!a) return;
        a->setBalance(new_balance);
        save_accounts_to_file();
    }

    // admin checks
    bool admin_check_credentials(const string& name, const string& password) const {
        // keep the original credentials for backward compatibility
        return (name == "Humair" && password == "humair123");
    }

}; // end Bank

// -----------------------------
// User methods
// -----------------------------
bool User::login() {
    displayBMSBanner();
    cout << GREEN << "Enter your PIN: " << RESET;
    int PIN = read_int();
    // validate PIN
    while (!is_valid_pin(PIN)) {
        cout << RED << "Invalid input. Please enter a valid 4-digit PIN: " << RESET;
        PIN = read_int();
    }
    if (bank->login_user_by_pin(PIN)) {
        currentIndex = bank->find_account_by_pin(PIN);
        cout << GREEN << "Welcome " << bank->get_current_username() << "!\n" << RESET;
        return true;
    } else {
        cout << RED << "Invalid PIN or account deactivated. Please try again.\n" << RESET;
        return false;
    }
}

void User::userOperations() {
    if (currentIndex < 0) return;
    int choice = -1;
    do {
        displayBMSBanner();
        cout << "1. Deposit Money" << endl;
        cout << "2. Withdraw Money" << endl;
        cout << "3. Transfer Money" << endl;
        cout << "4. Check Balance" << endl;
        cout << "5. Pay Bill" << endl;
        cout << "6. Modify PIN" << endl;
        cout << "7. Deactivate Account" << endl;
        cout << "0. Logout" << endl;
        cout << GREEN << "Choose: " << RESET;

        choice = read_int();
        switch (choice) {
            case 1: {
                cout << GREEN << "Enter amount to deposit: Rs. " << RESET;
                int amt = read_int();
                while (!is_valid_amount(amt)) {
                    cout << RED << "Invalid amount. Enter positive amount: " << RESET;
                    amt = read_int();
                }
                bank->deposit_current(amt);
                cout << GREEN << "Deposit successful. Current balance: Rs." << bank->get_current_balance() << "\n" << RESET;
                pause();
                break;
            }
            case 2: {
                cout << GREEN << "Enter amount to withdraw: Rs. " << RESET;
                int amt = read_int();
                while (!is_valid_amount(amt)) {
                    cout << RED << "Invalid amount. Enter positive amount: " << RESET;
                    amt = read_int();
                }
                if (bank->withdraw_current(amt)) {
                    cout << GREEN << "Withdrawal successful. New balance: Rs." << bank->get_current_balance() << "\n" << RESET;
                } else {
                    cout << RED << "Insufficient balance to complete withdrawal.\n" << RESET;
                }
                pause();
                break;
            }
            case 3: {
                displayBMSBanner();
                cout << "1. Inter Bank (existing account in this DB)" << endl;
                cout << "2. Other Bank" << endl;
                cout << "3. Cancel" << endl;
                cout << GREEN << "Choose: " << RESET;
                int t = read_int();
                if (t == 1) {
                    cout << GREEN << "Enter recipient's CNIC: " << RESET;
                    string rc; getline(cin, rc);
                    while (!is_valid_cnic(rc)) {
                        cout << RED << "Invalid CNIC. Enter 13-digit CNIC: " << RESET;
                        getline(cin, rc);
                    }
                    cout << GREEN << "Enter amount to transfer: Rs. " << RESET;
                    int amt = read_int();
                    while (!is_valid_amount(amt)) {
                        cout << RED << "Invalid amount. Enter positive amount: " << RESET;
                        amt = read_int();
                    }
                    bank->transfer_interbank(rc, amt);
                    pause();
                } else if (t == 2) {
                    cout << GREEN << "Enter recipient's account number: " << RESET;
                    string racc; getline(cin, racc);
                    cout << GREEN << "Enter recipient's bank name: " << RESET;
                    string rbank; getline(cin, rbank);
                    cout << GREEN << "Enter amount to transfer: Rs. " << RESET;
                    int amt = read_int();
                    while (!is_valid_amount(amt)) {
                        cout << RED << "Invalid amount. Enter positive amount: " << RESET;
                        amt = read_int();
                    }
                    bank->transfer_otherbank(racc, rbank, amt);
                    pause();
                } else {
                    cout << GREEN << "Transfer cancelled.\n" << RESET;
                    pause();
                }
                break;
            }
            case 4: {
                cout << GREEN << "Your current balance is: Rs." << bank->get_current_balance() << "\n" << RESET;
                pause();
                break;
            }
            case 5: {
                cout << GREEN << "Enter bill number: " << RESET;
                long billno; cin >> billno; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << GREEN << "Enter amount: Rs. " << RESET;
                int amt = read_int();
                while (!is_valid_amount(amt)) {
                    cout << RED << "Invalid amount. Enter positive amount: " << RESET;
                    amt = read_int();
                }
                bank->pay_bill(billno, amt);
                pause();
                break;
            }
            case 6: {
                cout << GREEN << "Enter new 4-digit PIN: " << RESET;
                int np = read_int();
                while (!is_valid_pin(np)) {
                    cout << RED << "Invalid PIN. Enter 4-digit PIN: " << RESET;
                    np = read_int();
                }
                bank->change_pin_current(np);
                pause();
                break;
            }
            case 7: {
                bank->deactivate_current_account();
                pause();
                break;
            }
            case 0:
                cout << GREEN << "Logging out.\n" << RESET;
                bank->logout_user();
                break;
            default:
                cout << RED << "Invalid option selected. Try again.\n" << RESET;
                pause();
                break;
        }
    } while (choice != 0);
}

void User::showMenu() {
    // login + operations
    if (login()) {
        userOperations();
    } else {
        pause();
    }
}

// -----------------------------
// Admin methods
// -----------------------------
bool Admin::login() {
    displayBMSBanner();
    cout << GREEN << "Enter your Name: " << RESET;
    string name; getline(cin, name);
    cout << GREEN << "Enter your Password: " << RESET;
    string password; getline(cin, password);

    if (bank->admin_check_credentials(name, password)) {
        cout << GREEN << "Admin login successful.\n" << RESET;
        return true;
    } else {
        cout << RED << "Incorrect admin credentials.\n" << RESET;
        return false;
    }
}

void Admin::showMenu() {
    if (!login()) { pause(); return; }
    int n = -1;
    do {
        displayBMSBanner();
        cout << "1. Create Account" << endl;
        cout << "2. Show All Accounts" << endl;
        cout << "3. Search Account" << endl;
        cout << "4. Modify Balance (by admin)" << endl;
        cout << "5. Change PIN (by admin)" << endl;
        cout << "6. Delete Account" << endl;
        cout << "7. Activate Account" << endl;
        cout << "0. Exit Admin" << endl;
        cout << GREEN << "Choose: " << RESET;
        n = read_int();
        switch (n) {
            case 1: bank->create_account(); break;
            case 2: bank->show_all_accounts(); break;
            case 3: bank->search_account(); break;
            case 4: bank->modify_balance_by_admin(); break;
            case 5: bank->change_pin_by_admin(); break;
            case 6: bank->delete_account(); break;
            case 7: bank->activate_account(); break;
            case 0: cout << GREEN << "Admin session ended.\n" << RESET; break;
            default: cout << RED << "Invalid choice!\n" << RESET; pause(); break;
        }
    } while (n != 0);
}

// -----------------------------
// main
// -----------------------------
int main() {
    Bank bank;
    bank.load_accounts_from_file();

    Admin admin(&bank);
    User user(&bank);

    int choice = -1;
    do {
        displayBMSBanner();
        cout << GREEN << "1. Admin Login" << endl;
        cout << GREEN << "2. User Login" << endl;
        cout << GREEN << "0. Exit" << RESET << endl;
        cout << RESET << "Enter your choice: " << RESET;

        choice = read_int();
        switch (choice) {
            case 1: admin.showMenu(); break;
            case 2: user.showMenu(); break;
            case 0:
                bank.save_accounts_to_file();
                cout << GREEN << "Thank you for using the system!\n" << RESET;
                break;
            default:
                cout << RED << "Invalid option! Please try again." << RESET << endl;
                pause();
                break;
        }
    } while (choice != 0);

    return 0;
}