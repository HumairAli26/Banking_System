#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <Windows.h>

using namespace std;

#define GREEN "\033[32m"
#define RED "\033[31m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

struct Account 
{
    string user_name;
    string cnic;
    unsigned long long acc_no;
    int pin;
    string DOB;
    int balance;
    bool is_deactivated;
};

Account* accounts = nullptr;
int num_accounts = 0;
int current_user_index = -1; // Represents the currently logged-in user

void displayBMSBanner() 
{
    cout<<"\033[2J\033[1;1H";
    cout <<BLUE << "\t\t\t\t===========================================" <<endl;
    cout <<BLUE << "\t\t\t\t|        BANK MANAGEMENT SYSTEM           |" <<endl;
    cout <<BLUE << "\t\t\t\t===========================================" <<endl;
    cout <<BLUE << "\t\t\t\t|  Welcome to the Bank Management System! |" <<endl;
    cout <<BLUE << "\t\t\t\t|    Secure, Reliable, and Efficient.     |" <<endl;
    cout <<BLUE << "\t\t\t\t===========================================" <<endl;
    cout<<"\n\n\n"<<endl;
}

void load_accounts_from_file() 
{
    ifstream infile("accounts.txt", ios::in);
    if (infile.is_open()) 
    {
        infile >> num_accounts;
        if (num_accounts > 0)
        {
            accounts = new Account[num_accounts];
            for (int i = 0; i < num_accounts; i++) 
            {
                infile >> accounts[i].user_name;
                infile >> accounts[i].cnic ;
                infile >> accounts[i].acc_no; 
                infile >> accounts[i].pin;
                infile >> accounts[i].DOB; 
                infile >> accounts[i].balance; 
                infile >> accounts[i].is_deactivated;
            }
        }
        infile.close();
        //cout <<GREEN << "Account data has been loaded from file." << endl;
    } 
    else 
    {
        cout <<RED << "No previous account data found. Starting fresh." << endl;
    }
}

void save_accounts_to_file() 
{
    ofstream outfile("accounts.txt", ios::out);
    if (outfile.is_open()) 
    {
        outfile << num_accounts << endl;
        for (int i = 0; i < num_accounts; i++) 
        {
            outfile << accounts[i].user_name << endl;
            outfile << accounts[i].cnic << endl;
            outfile << accounts[i].acc_no << endl;
            outfile << accounts[i].pin << endl;
            outfile << accounts[i].DOB << endl;
            outfile << accounts[i].balance << endl;
            outfile << accounts[i].is_deactivated << endl;
        }
        outfile.close();
        //cout <<GREEN << "Account data has been saved to file." << endl;
    } 
    else 
    {
        cout <<RED << "Failed to open file for saving." << endl;
    }
}


bool is_valid_pin(int pin)
{
    return pin >= 1000 && pin <= 9999;
}

bool is_integer_input() 
{
    // This is a basic check, a more robust one would handle all edge cases
    if (cin.fail())
    {
        cin.clear(); // clear error flag
        cin.ignore(10000, '\n'); // discard invalid input
        return false;
    }
    return true;
}

bool is_valid_account_number(unsigned long long acc_no)
{
    return acc_no >= 10000000000 && acc_no <= 99999999999;
}

bool is_valid_amount(int amount)
{
    return amount > 0;
}

bool is_valid_cnic(const string& cnic)
{
    if (cnic.length() != 13)
    {
        return false;
    }
    for (char ch : cnic)
    {
        if (ch < '0' || ch > '9')
        {
            return false;
        }
    }

    return true;
}

void create_account() 
{
    displayBMSBanner();
    Account new_account;
    int PIN, con_pin;
    srand(time(0));
    cout <<GREEN << "To create an account, please enter your information" << endl;
    cout <<GREEN << "Enter your Name: ";
    cin.ignore();
    getline(cin, new_account.user_name);
    cout <<GREEN << "Enter your National Identity Card No: ";
    getline(cin, new_account.cnic);
    while (!is_valid_cnic(new_account.cnic)) 
    {
        cout <<RED << "Invalid CNIC."<<endl;
        cout <<GREEN << "Enter a valid CNIC (13 digits): ";
        getline(cin, new_account.cnic);
    }
    cout <<GREEN << "Enter your Date of Birth (DD/MM/YYYY): ";
    getline(cin, new_account.DOB);
    new_account.acc_no = 10000000000 + rand() % 90000000000;
    cout <<GREEN << "Your Account Number is: " << new_account.acc_no << endl;
    cout <<GREEN << "Please set a 4-digit PIN for your Account: ";
    cin >> PIN;
    while (!is_integer_input() || !is_valid_pin(PIN))
    {
        cout <<RED << "Invalid input. Please enter a valid 4-digit PIN: ";
        cin.clear();
        cin.ignore(10000, '\n');
        cin >> PIN;
    }
    cout << "Please re-confirm your PIN: ";
    cin >> con_pin;
    while (PIN != con_pin)
    {
        cout <<RED << "Your PIN did not match."<<endl;
        cout <<GREEN << "Re-enter your PIN for confirmation: ";
        cin >> con_pin;
    }
    new_account.pin = PIN; // Set the PIN
    cout <<GREEN << "Your PIN has been successfully set!" << endl;
    new_account.balance = 0;
    int n;
    cout<<"Do you want to Deposit Money"<<endl;
    cout<<"1. YES"<<endl;
    cout<<"0. NO"<<endl;
    cin >> n;
    switch (n)
    {
        case 1:
            cout<<"Enter ammount you want to deposit"<<endl;
            cin>>new_account.balance;
            while (!is_valid_amount(new_account.balance))
            {
                cout << RED << "Invalid amount. Please enter a positive amount: ";
                cin.clear();
                cin.ignore(10000, '\n');
                cin >> new_account.balance;
            }
            break;
        case 0:
            break;
    }
    new_account.is_deactivated = false;
    Account* new_accounts = new Account[num_accounts + 1];
    for (int i = 0; i < num_accounts; i++) 
    {
        new_accounts[i] = accounts[i];
    }
    new_accounts[num_accounts] = new_account;
    if(accounts != nullptr)
        delete[] accounts;
    accounts = new_accounts;
    num_accounts++;
    cout <<GREEN << "Your account has been created!" << endl;
    save_accounts_to_file();
    getch();
}

void modify_balance() 
{
    displayBMSBanner();
    string cnic;
    cout <<GREEN << "Enter CNIC of Account you want to modify balance for:" <<endl;
    cin.ignore();
    getline(cin,cnic);
    while (!is_valid_cnic(cnic)) 
    {
        cout <<RED << "Invalid CNIC." <<endl;
        cout<<GREEN << "enter a valid CNIC (13 digits): ";
        getline(cin, cnic);
    }
    
    int account_index = -1;
    for(int i=0; i<num_accounts; i++)
    {
        if(accounts[i].cnic == cnic)
        {
            account_index = i;
            break;
        }
    }

    if(account_index != -1)
    {
        int new_balance;
        cout << GREEN << "Current balance for " << accounts[account_index].user_name << " is Rs." << accounts[account_index].balance << endl;
        cout << GREEN << "Enter the new balance: ";
        cin >> new_balance;
        // Admin can set balance to 0, so we check for < 0
        while (new_balance < 0)
        {
            cout <<RED << "Invalid amount. Please enter a positive amount or 0: ";
            cin.clear();
            cin.ignore(10000, '\n');
            cin >> new_balance;
        }
        accounts[account_index].balance = new_balance;
        cout <<GREEN << "Balance successfully updated." << endl;
        cout << GREEN << "New balance for " << accounts[account_index].user_name << " is Rs." << accounts[account_index].balance << endl;
        save_accounts_to_file();
    }
    else
    {
        cout << RED << "Account with that CNIC not found." << endl;
    }
    getch();
}

// RENAMED from modify_pin
void admin_reset_pin() 
{
    displayBMSBanner();
    string cnic;
    cout <<GREEN << "Enter CNIC of Account you want to Change PIN of:" <<endl;
    cin.ignore();
    getline(cin,cnic);
    while (!is_valid_cnic(cnic)) 
    {
        cout <<RED << "Invalid CNIC." <<endl;
        cout<<GREEN << "enter a valid CNIC (13 digits): ";
        getline(cin, cnic);
    }
   
    int account_index = -1;
    for(int i=0; i<num_accounts; i++)
    {
        if(accounts[i].cnic == cnic)
        {
            account_index = i;
            break;
        }
    }
    
    if (account_index == -1)
    {
        cout << RED << "Account with that CNIC not found." << endl;
        getch();
        return;
    }

    // Per documentation, admin can only reset PIN for DEACTIVATED accounts
    if(accounts[account_index].is_deactivated == true)
    {
        int new_pin;
        cout <<GREEN << "Account is deactivated. Ready for PIN reset." << endl;
        cout <<GREEN << "Enter the new 4-digit PIN: ";
        cin >> new_pin;
        while (!is_integer_input() || !is_valid_pin(new_pin)) 
        {
            cout <<RED << "Invalid PIN. Please enter a 4-digit PIN: ";
            cin.clear();
            cin.ignore(10000, '\n');
            cin >> new_pin;
        }
        accounts[account_index].pin = new_pin;
        cout <<GREEN << "The new PIN has been set to: " << accounts[account_index].pin << endl;
        cout << GREEN << "Remember to reactivate the account." << endl;
        save_accounts_to_file();
    }
    else
    {
        cout << RED << "Account must be DEACTIVATED before admin can reset PIN." << endl;
        cout << RED << "Please deactivate the account first." << endl;
    }
    getch();
}

// NEW FUNCTION for the user/ATM side
void user_modify_own_pin()
{
    displayBMSBanner();
    int current_pin, new_pin, confirm_pin;

    cout << GREEN << "Please enter your CURRENT 4-digit PIN to verify: ";
    cin >> current_pin;

    if (!is_integer_input() || accounts[current_user_index].pin != current_pin)
    {
        cout << RED << "Incorrect PIN. Returning to menu." << endl;
        getch();
        return;
    }

    cout << GREEN << "Enter your NEW 4-digit PIN: ";
    cin >> new_pin;
    while (!is_integer_input() || !is_valid_pin(new_pin))
    {
        cout << RED << "Invalid input. Please enter a valid 4-digit PIN: ";
        cin.clear();
        cin.ignore(10000, '\n');
        cin >> new_pin;
    }

    cout << GREEN << "Please re-confirm your NEW PIN: ";
    cin >> confirm_pin;

    if (new_pin != confirm_pin)
    {
        cout << RED << "Your new PINs did not match. PIN change canceled." << endl;
        getch();
        return;
    }

    accounts[current_user_index].pin = new_pin;
    save_accounts_to_file();
    cout << GREEN << "Your PIN has been successfully changed!" << endl;
    getch();
}


void deposit_Money() 
{
    displayBMSBanner();
    int amount;
    cout <<GREEN << "Enter the amount you want to deposit: Rs. ";
    cin >> amount;
    while (!is_integer_input() || !is_valid_amount(amount)) 
    {
        cout <<RED << "Invalid amount. "<<endl;
        cout <<GREEN<<"Please enter a positive amount: ";
        cin.clear();
        cin.ignore(10000, '\n');
        cin >> amount;
    }
    accounts[current_user_index].balance += amount;
    cout <<GREEN << "Your current balance is: Rs." << accounts[current_user_index].balance << endl;
    save_accounts_to_file(); 
    getch();
}

void Withdraw_Money() 
{
    displayBMSBanner();
    int amount;
    cout <<GREEN << "Enter the amount you want to withdraw: Rs. ";
    cin >> amount;
    while (!is_integer_input() || !is_valid_amount(amount)) 
    {
        cout <<RED << "Invalid amount."<<endl;
        cout<<GREEN << "Please enter a positive amount: ";
        cin.clear();
        cin.ignore(10000, '\n');
        cin >> amount;
    }
    if (amount <= accounts[current_user_index].balance) 
    {
        accounts[current_user_index].balance -= amount;
        cout << "Your withdrawal was successful. New balance is: Rs." << accounts[current_user_index].balance << endl;
        save_accounts_to_file();
    } 
    else 
    {
        cout <<RED << "Insufficient balance to complete the withdrawal.\n";
    }
    getch();
}

void transfer_money() 
{
    displayBMSBanner();
    int n;
    cout << "1. Inter Bank (Transfer to another account in this bank)" << endl;
    cout << "2. Other Bank (Simulated)" << endl;
    cout << "3. Cancel" << endl;
    cin >> n;
    while (!is_integer_input())
    {
        cout <<RED << "Invalid input." <<endl; 
        cout <<GREEN << "Please choose a valid option: ";
        cin.clear();
        cin.ignore(10000, '\n');
        cin >> n;
    }
    switch (n) 
    {
    case 1: 
    {
        displayBMSBanner();
        string recipient_CNIC;
        int amount;
        cout <<GREEN << "Enter the recipient's 13-digit CNIC: ";
        cin >> recipient_CNIC;
        while (!is_valid_cnic(recipient_CNIC)) 
        {
            cout <<RED << "Invalid CNIC. "<<endl;
            cout <<GREEN << "Please enter a valid CNIC (13 digits): ";
            cin >> recipient_CNIC;
        }

        if (recipient_CNIC == accounts[current_user_index].cnic)
        {
            cout << RED << "You cannot transfer money to yourself." << endl;
            getch();
            break;
        }

        bool recipient_found = false;
        for (int j = 0; j < num_accounts; j++) 
        {
            if (accounts[j].cnic == recipient_CNIC) 
            {
                recipient_found = true;
                cout <<GREEN << "Enter the amount to transfer: Rs. ";
                cin >> amount;
                while (!is_integer_input() || !is_valid_amount(amount)) 
                {
                    cout <<RED << "Invalid amount." <<endl;
                    cout <<GREEN << "Please enter a positive amount: ";
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cin >> amount;
                }
                if (amount <= accounts[current_user_index].balance) 
                {
                    accounts[current_user_index].balance -= amount;
                    accounts[j].balance += amount;
                    cout << "Transfer successful! Rs." << amount << " has been transferred to " << accounts[j].user_name << "." << endl;
                    save_accounts_to_file();
                } 
                else 
                {
                    cout <<RED << "Insufficient balance to complete the transfer." << endl;
                }
                break;
            }
        }
        if (!recipient_found) 
        {
            cout <<RED << "Recipient account not found." << endl;
        }
        getch();
        break;
    }
    case 2: 
    {
        displayBMSBanner();
        string recipient_account, recipient_bank;
        int amount;
        cout <<GREEN << "Enter the recipient's account number: ";
        cin >> recipient_account;
        cout <<GREEN << "Enter the recipient's bank name: ";
        cin >> recipient_bank;
        cout <<GREEN << "Enter the amount to transfer: Rs. ";
        cin >> amount;
        while (!is_integer_input() || !is_valid_amount(amount)) 
        {
            cout <<RED << "Invalid amount. Please enter a positive amount: ";
            cin.clear();
            cin.ignore(10000, '\n');
            cin >> amount;
        }
        if (amount <= accounts[current_user_index].balance) 
        {
            accounts[current_user_index].balance -= amount;
            cout <<GREEN << "Transfer successful! Rs." << amount << " has been transferred to account number " << recipient_account << " at " << recipient_bank << "." << endl;
            save_accounts_to_file();
        } 
        else 
        {
            cout <<RED << "Insufficient balance to complete the transfer." << endl;
        }
        break;
    }
    case 3:
        displayBMSBanner();
        cout <<GREEN << "Transfer canceled." << endl;
        break;
    default:
        cout <<RED << "Invalid option selected. Please try again." << endl;
        break;
    }
    getch();
}

void check_balance() 
{
    displayBMSBanner();
    cout <<GREEN << "Your current balance is: Rs." << accounts[current_user_index].balance << endl;
    getch();
}

void billpay() 
{
    displayBMSBanner();
    int amount;
    long int billno; // This should probably be a string
    cout <<GREEN << "Enter your bill number to pay: ";
    cin >> billno;
    cout <<GREEN << "Enter the amount of the bill: Rs. ";
    cin >> amount;
    while (!is_integer_input() || !is_valid_amount(amount)) 
    {
        cout <<RED << "Invalid amount. Please enter a positive amount: ";
        cin.clear();
        cin.ignore(10000, '\n');
        cin >> amount;
    }
    if (amount <= accounts[current_user_index].balance) 
    {
        accounts[current_user_index].balance -= amount;
        cout << "Your bill has been paid. Bill Number: " << billno << endl;
        save_accounts_to_file();
    } 
    else 
    {
        cout <<RED << "Insufficient balance to pay the bill."<<endl;
    }
    getch();
}

void deactivate_account() 
{
    displayBMSBanner();
    char confirmation;
    cout << RED << "Are you sure you want to deactivate your account?" << endl;
    cout << RED << "You will NOT be able to log in again until an admin reactivates it." << endl;
    cout << GREEN << "Enter 'Y' to confirm, any other key to cancel: ";
    cin >> confirmation;

    if(confirmation == 'Y' || confirmation == 'y')
    {
        accounts[current_user_index].is_deactivated = true;
        cout <<GREEN << "Your account is now deactivated."<<endl;
        save_accounts_to_file();
        current_user_index = -1; // Log the user out immediately
    }
    else
    {
        cout << GREEN << "Deactivation canceled." << endl;
    }
    getch();
}

void activate_account() 
{
    displayBMSBanner();
    string cnic;
    cout <<GREEN << "Enter CNIC of Account you want to Activate" <<endl;
    cin.ignore();
    getline(cin,cnic);
    while (!is_valid_cnic(cnic)) 
    {
        cout <<RED << "Invalid CNIC." <<endl;
        cout<<GREEN << "enter a valid CNIC (13 digits): ";
        getline(cin, cnic);
    }

    int account_index = -1;
    for(int i=0; i<num_accounts; i++)
    {
        if(accounts[i].cnic == cnic)
        {
            account_index = i;
            break;
        }
    }

    if (account_index == -1)
    {
        cout << RED << "Account with that CNIC not found." << endl;
        getch();
        return;
    }

    if(accounts[account_index].is_deactivated == true)
    {
        accounts[account_index].is_deactivated = false;
        cout <<GREEN << "Account for " << accounts[account_index].user_name << " is now activated." << endl;
        save_accounts_to_file();
    }
    else
    {
        cout << GREEN << "This account is already active." << endl;
    }
    
    getch();
}

void show_all_accounts() 
{
    displayBMSBanner();
    cout <<GREEN << "Displaying all " << num_accounts << " accounts:" << endl;
    cout << "-----------------------------------" << endl;
    for (int i = 0; i < num_accounts; i++) 
    {
        cout << "Name: " << accounts[i].user_name <<endl;
        cout << "CNIC: " << accounts[i].cnic << endl;
        cout << "Account No: " << accounts[i].acc_no << endl;
        cout << "Balance: Rs." << accounts[i].balance <<endl;
        cout << "Deactivated: " << (accounts[i].is_deactivated ? "Yes" : "No") << endl;
        cout << "-----------------------------------" << endl;
    }
    getch();
}

void search_account() 
{
    displayBMSBanner();
    string search_cnic;
    cout <<GREEN << "Enter the CNIC to search for the account: ";
    cin >> search_cnic;
    bool found = false;
    for (int i = 0; i < num_accounts; i++) 
    {
        if (accounts[i].cnic == search_cnic) 
        {
            found = true;
            cout <<GREEN << "Account found: " << endl;
            cout << "-----------------------------------" << endl;
            cout << "Name: " << accounts[i].user_name <<endl;
            cout << "CNIC: " << accounts[i].cnic << endl;
            cout << "Account No: " << accounts[i].acc_no <<endl; 
            cout << "Balance: Rs." << accounts[i].balance << endl;
            cout << "PIN: " << accounts[i].pin << endl; // Admin can see PIN
            cout << "DOB: " << accounts[i].DOB << endl;
            cout << "Deactivated: " << (accounts[i].is_deactivated ? "Yes" : "No") << endl;
            cout << "-----------------------------------" << endl;
            break;
        }
    }
    if (!found) 
    {
        cout <<RED << "Account with given CNIC not found." << endl;
    }
    getch();
}

void delete_account() 
{
    displayBMSBanner();
    string del_cnic;
    cout <<GREEN << "Enter the CNIC of the account to delete: ";
    cin >> del_cnic;
    
    int account_index = -1;
    for (int i = 0; i < num_accounts; i++) 
    {
        if (accounts[i].cnic == del_cnic)
        {
            account_index = i;
            break;
        }
    }

    if (account_index != -1) 
    {
        Account* new_accounts = new Account[num_accounts - 1];
        int new_index = 0;
        for(int i = 0; i < num_accounts; i++)
        {
            if (i != account_index)
            {
                new_accounts[new_index] = accounts[i];
                new_index++;
            }
        }
        
        delete[] accounts;
        accounts = new_accounts;
        num_accounts--;
        
        cout << "Account deleted successfully." << endl;
        save_accounts_to_file();
    }
    else 
    {
        cout <<RED << "Account with given CNIC not found." << endl;
    }
    getch();
}

void admin_side() 
{
    displayBMSBanner();
    int n;
    string name, password;
    cout <<GREEN << "Enter your Admin Name: " << endl;
    cin.ignore();
    getline(cin, name);
    cout <<GREEN << "Enter your Admin Password: " << endl;
    // Basic password masking
    char ch;
    password = "";
    while((ch = _getch()) != 13) // 13 is Enter key
    {
        if (ch == 8) // 8 is Backspace
        {
            if (password.length() > 0)
            {
                password.pop_back();
                cout << "\b \b"; // Erase character on screen
            }
        }
        else
        {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;

    if (name == "Humair" && password == "humair123") 
    {
        cout << GREEN << "Admin login successful." << endl;
        getch();
        do 
        {
            displayBMSBanner();
            cout << "1. Create Account" << endl;
            cout << "2. Show All Accounts" << endl;
            cout << "3. Search Account" << endl;
            cout << "4. Modify Balance" << endl;
            cout << "5. Reset Customer PIN" << endl; // CHANGED
            cout << "6. Delete Account" << endl;
            cout << "7. Activate Account" << endl;
            cout << "0. Exit Admin Menu" << endl;
            cin >> n;
            while (!is_integer_input())
            {
                cout <<RED << "Invalid input." <<endl; 
                cout <<GREEN << "Please choose a valid option: ";
                cin.clear();
                cin.ignore(10000, '\n');
                cin >> n;
            }
            switch (n) 
            {
                case 1:
                    create_account();
                    break;
                case 2:
                    show_all_accounts();
                    break;
                case 3:
                    search_account();
                     break;
                case 4:
                    modify_balance();
                    break;
                case 5:
                    admin_reset_pin(); // CHANGED
                    break;
                case 6:
                    delete_account();
                    break;
                case 7:
                    activate_account();
                    break;
                case 0:
                    cout <<GREEN << "Admin session ended." << endl;
                    break;
                default:
                    cout <<RED << "Invalid choice!" << endl;
                    break;
            }
        } while (n != 0);
    } 
    else 
    {
        cout <<RED << "Incorrect admin credentials." << endl;
    }
    getch();
}

void user_side() 
{
    displayBMSBanner();
    int PIN;
    bool user_found = false;
    cout << GREEN << "Welcome to the ATM." << endl; // ADDED
    cout << GREEN << "Please enter your 4-digit PIN: "; // MODIFIED
    cin >> PIN;
    while (!is_integer_input() || !is_valid_pin(PIN))
    {
        cout <<RED << "Invalid input. Please enter a valid 4-digit PIN: ";
        cin.clear();
        cin.ignore(10000, '\n');
        cin >> PIN;
    }
    for (int i = 0; i < num_accounts; i++) 
    {
        if (accounts[i].pin == PIN) 
        {
            if (accounts[i].is_deactivated == false)
            {
                user_found = true;
                current_user_index = i;
                cout <<GREEN << "Welcome " << accounts[i].user_name << endl;
                getch();
                break;
            }
            else
            {
                cout << RED << "Your account is DEACTIVATED." << endl;
                cout << RED << "Please contact an administrator to reactivate it." << endl;
                getch();
                return;
            }
        }
    }
    if (!user_found) 
    {
        cout <<RED << "Invalid PIN or Account not found. Please try again." << endl;
        getch();
        return;
    }

    int choice;
    do 
    {
        displayBMSBanner();
        cout << "ATM Menu" << endl;
        cout << "1. Deposit Money" << endl;
        cout << "2. Withdraw Money" << endl;
        cout << "3. Transfer Money" << endl;
        cout << "4. Check Balance" << endl;
        cout << "5. Pay Bill" << endl;
        cout << "6. Change Your PIN" << endl; // CHANGED
        cout << "7. Deactivate Account" << endl;
        cout << "0. Exit / Remove Card" << endl; // CHANGED
        cin >> choice;
        while (!is_integer_input())
        {
            cout <<RED << "Invalid input." <<endl; 
            cout <<GREEN << "Please choose a valid option: ";
            cin.clear();
            cin.ignore(10000, '\n');
            cin >> choice;
        }
        switch (choice) 
        {
            case 1:
                deposit_Money();
                break;
            case 2:
                Withdraw_Money();
                break;
            case 3:
                transfer_money();
                break;
            case 4:
                check_balance();
                break;
            case 5:
                billpay();
                break;
            case 6:
                user_modify_own_pin(); // CHANGED
                break;
            case 7:
                deactivate_account();
                if (current_user_index == -1) // Check if user deactivated
                {
                    choice = 0; // Force exit from loop
                }
                break;
            case 0:
                cout <<GREEN << "Logging out. Thank you!" << endl;
                break;
            default:
                cout <<RED << "Invalid option selected. Please try again." << endl;
                break;
        }
    } while (choice != 0);
    
    current_user_index = -1; // Clear the current user on logout
    getch();
}

int main() 
{
    int choice;
    load_accounts_from_file();
    do 
    {
        displayBMSBanner();
        cout <<GREEN << "1. Admin Login" << endl;
        cout <<GREEN << "2. ATM / User Login" << endl; // CHANGED
        cout <<GREEN << "0. Exit System" << endl; // CHANGED
        cout <<RESET << "Enter your choice: ";
        cin >> choice;
        while (!is_integer_input())
        {
            cout <<RED << "Invalid input." <<endl; 
            cout <<GREEN << "Please choose a valid option: ";
            cin.clear();
            cin.ignore(10000, '\n');
            cin >> choice;
        }
        switch (choice) 
        {
            case 1:
                admin_side();
                break;
            case 2:
                user_side();
                break;
            case 0:
                save_accounts_to_file();
                cout <<GREEN << "Thank you for using the system! Shutting down." << endl;
                break;
            default:
                cout <<RED << "Invalid option! Please try again." << endl;
                break;
        }
    } while (choice != 0);
    
    // Clean up dynamically allocated memory
    if (accounts != nullptr)
    {
        delete[] accounts;
        accounts = nullptr;
    }

    getch();
    return 0;
}