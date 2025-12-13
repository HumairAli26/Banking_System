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
int current_user_index = -1;

void displayBMSBanner() 
{
    //cout<<"\033[2J\033[1;1H";
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
    char ch;
    bool isValid = true;
    while (cin.get(ch)) 
    {
        if ((ch < '0' || ch > '9') && ch != '-' && ch != '\n' && ch != ' ') 
        {
            isValid = false;
            break;
        }
        if (ch == '\n') 
        {
            break;
        }
    }
    if (!isValid) 
    {
        return false;
    }
    cin.putback(ch);
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
        cout <<GREEN << "Enter a valid CNIC (13 digits): "<<endl;
        getline(cin, new_account.cnic);
    }
    cout <<GREEN << "Enter your Date of Birth (DD/MM/YYYY): ";
    getline(cin, new_account.DOB);
    new_account.acc_no = 10000000000 + rand() % 90000000000;
    cout <<GREEN << "Your Account Number is: " << new_account.acc_no << endl;
    cout <<GREEN << "Please set a PIN for your Account: "<<endl;
    cin >> PIN;
    while (!is_integer_input() || !is_valid_pin(PIN))
    {
        cout <<RED << "Invalid input. Please enter a valid 4-digit PIN: ";
        cin >> PIN;
    }
    cout << "Please re-confirm your PIN: ";
    cin >> con_pin;
    while (PIN != con_pin)
    {
        cout <<RED << "Your PIN did not match."<<endl;
        cout <<GREEN << "Re-enter your PIN for confirmation: "<<endl;
        cin >> con_pin;
    }
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
    int PIN;
    string cnic;
    cout <<GREEN << "Enter CNIC of Account you want to Modify balance of:" <<endl;
    cin.ignore();
    getline(cin,cnic);
    while (!is_valid_cnic(cnic)) 
    {
        cout <<RED << "Invalid CNIC." <<endl;
        cout<<GREEN << "enter a valid CNIC (13 digits): ";
        getline(cin, cnic);
    }
    cout <<GREEN << "Enter the PIN" <<endl;
    cin >> PIN;
    while (!is_integer_input() || !is_valid_pin(PIN))
    {

        cout <<RED << "Invalid input." <<endl; 
        cout <<GREEN << "Please enter a valid 4-digit PIN: ";
        cin >> PIN;
    }
    for(int i=0; i<num_accounts; i++)
    {
        if(accounts[i].pin == PIN && accounts[i].is_deactivated == false)
        {
            int new_balance;
            cout <<GREEN << "Enter your new balance: ";
            cin >> new_balance;
            while (!is_valid_amount(new_balance))
            {
                cout <<RED << "Invalid amount. Please enter a positive amount: ";
                cin >> new_balance;
            }
            accounts[current_user_index].balance = new_balance;
            cout <<GREEN << "Your new balance is Rs." << accounts[current_user_index].balance << endl;
            save_accounts_to_file();
        }
    }
    getch();
}

void modify_pin() 
{
    displayBMSBanner();
    int PIN;
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
    cout <<GREEN << "Enter the PIN" <<endl;
    cin >> PIN;
    while (!is_integer_input() || !is_valid_pin(PIN))
    {

        cout <<RED << "Invalid input." <<endl; 
        cout <<GREEN << "Please enter a valid 4-digit PIN: ";
        cin >> PIN;
    }
    for(int i=0; i<num_accounts; i++)
    {
        if(accounts[i].pin == PIN && accounts[i].is_deactivated == true)
        {
            int new_pin;
            cout <<GREEN << "Enter your new PIN: ";
            cin >> new_pin;
            while (!is_valid_pin(new_pin)) 
            {
                cout <<RED << "Invalid PIN. Please enter a 4-digit PIN: ";
                cin >> new_pin;
            }
            accounts[current_user_index].pin = new_pin;
            cout <<GREEN << "Your new PIN has been set to: " << accounts[current_user_index].pin << endl;
            save_accounts_to_file();
        }
    }
    getch();
}

void deposit_Money() 
{
    displayBMSBanner();
    int amount;
    cout <<GREEN << "Enter the amount you want to deposit: Rs. ";
    cin >> amount;
    while (!is_valid_amount(amount)) 
    {
        cout <<RED << "Invalid amount. "<<endl;
        cout <<GREEN<<"Please enter a positive amount: "<<endl;
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
    while (!is_valid_amount(amount)) 
    {
        cout <<RED << "Invalid amount."<<endl;
        cout<<GREEN << "Please enter a positive amount: " <<endl;
        cin >> amount;
    }
    if (amount <= accounts[current_user_index].balance) 
    {
        accounts[current_user_index].balance -= amount;
        cout << "Your withdrawal was successful. New balance is: Rs." << accounts[current_user_index].balance<<endl;
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
    cout << "1. Inter Bank" << endl;
    cout << "2. Other Bank" << endl;
    cout << "3. Cancel" << endl;
    cin >> n;
    while (!is_integer_input())
    {
        cout <<RED << "Invalid input." <<endl; 
        cout <<GREEN << "Please choose a valid option: ";
        cin >> n;
    }
    switch (n) 
    {
    case 1: 
    {
        displayBMSBanner();
        string recipient_CNIC;
        int amount;
        cout <<GREEN << "Enter the recipient's CNIC: ";
        cin >> recipient_CNIC;
        while (!is_valid_cnic(recipient_CNIC)) 
        {
            cout <<RED << "Invalid CNIC. "<<endl;
            cout <<GREEN << "Please enter a valid CNIC (13 digits): "<<endl;
            cin >> recipient_CNIC;
        }
        bool recipient_found = false;
        for (int j = 0; j < num_accounts; j++) 
        {
            if (accounts[j].cnic == recipient_CNIC) 
            {
                recipient_found = true;
                cout <<GREEN << "Enter the amount to transfer: Rs. ";
                cin >> amount;
                while (!is_valid_amount(amount)) 
                {
                    cout <<RED << "Invalid amount." <<endl;
                    cout <<GREEN << "Please enter a positive amount: ";
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
        while (!is_valid_amount(amount)) 
        {
            cout <<RED << "Invalid amount. Please enter a positive amount: ";
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
    long int billno;
    cout <<GREEN << "Enter your bill number to pay: ";
    cin >> billno;
    cout <<GREEN << "Enter the amount of the bill: Rs. ";
    cin >> amount;
    while (!is_valid_amount(amount)) 
    {
        cout <<RED << "Invalid amount. Please enter a positive amount: ";
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
    accounts[current_user_index].is_deactivated = true;
    cout <<GREEN << "Your account is now deactivated."<<endl;
    save_accounts_to_file();
    getch();
}

void activate_account() 
{
    displayBMSBanner();
    int PIN;
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
    cout <<GREEN << "Enter the PIN" <<endl;
    cin >> PIN;
    while (!is_integer_input() || !is_valid_pin(PIN))
    {

        cout <<RED << "Invalid input." <<endl; 
        cout <<GREEN << "Please enter a valid 4-digit PIN: ";
        cin >> PIN;
    }
    for(int i=0; i<num_accounts; i++)
    {
        if(accounts[i].pin == PIN && accounts[i].is_deactivated == true)
        {
            accounts[i].is_deactivated = false;
            cout <<GREEN << "Your account is now activated."<<endl;
            save_accounts_to_file();
        }
        else
        {
            cout<<"Account is already Active"<<endl;
        }
    }
    getch();
}

void show_all_accounts() 
{
    displayBMSBanner();
    cout <<GREEN << "Displaying all accounts:" << endl;
    for (int i = 0; i < num_accounts; i++) 
    {
        cout << "Account No: " << accounts[i].acc_no << endl;
        cout << "Name: " << accounts[i].user_name <<endl;
        cout << "Balance: Rs." << accounts[i].balance <<endl;
        cout << "Deactivated: " << (accounts[i].is_deactivated ? "Yes" : "No") << endl;
    }
    getch();
}

void search_account() 
{
    displayBMSBanner();
    string search_cnic;
    cout <<GREEN << "Enter the CNIC to search for the account: "<<endl;
    cin >> search_cnic;
    bool found = false;
    for (int i = 0; i < num_accounts; i++) 
    {
        if (accounts[i].cnic == search_cnic) 
        {
            found = true;
            cout <<GREEN << "Account found: " << endl;
            cout <<GREEN << "Account No: " << accounts[i].acc_no <<endl; 
            cout <<GREEN << "Name: " << accounts[i].user_name <<endl; 
            cout <<GREEN << "Balance: Rs." << accounts[i].balance << endl;
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
    cout<<GREEN << "Enter the CNIC of the account to delete: ";
    cin>>del_cnic;
    bool deleted=false;
    for (int i=0;i<num_accounts;i++) 
    {
        if (accounts[i].cnic==del_cnic) 
        {
            for (int j=i; j<num_accounts-1;j++) 
            {
                accounts[j] = accounts[j + 1];
            }
            num_accounts--;
            deleted=true;
            cout<<"Account deleted successfully."<<endl;
            break;
        }
    }
    if (!deleted) 
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
    cout <<GREEN << "Enter your Name: " << endl;
    cin.ignore();
    getline(cin, name);
    cout <<GREEN << "Enter your Password: " << endl;
    getline(cin, password);
    if (name == "Humair" && password == "humair123") 
    {
        do 
        {
            displayBMSBanner();
            cout << "1. Create Account" << endl;
            cout << "2. Show All Accounts" << endl;
            cout << "3. Search Account" << endl;
            cout << "4. Modify Balance" << endl;
            cout << "5. Change PIN" << endl;
            cout << "6. Delete Account" << endl;
            cout << "7. Activate Account" << endl;
            cout << "0. Exit" << endl;
            cin >> n;
            while (!is_integer_input())
            {
                cout <<RED << "Invalid input." <<endl; 
                cout <<GREEN << "Please choose a valid option: ";
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
                    modify_pin();
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
    cout <<GREEN << "Enter your PIN: ";
    cin >> PIN;
    while (!is_integer_input() || !is_valid_pin(PIN))
    {
        cout <<RED << "Invalid input. Please enter a valid 4-digit PIN: ";
        cin >> PIN;
    }
    for (int i = 0; i < num_accounts; i++) 
    {
        if (accounts[i].pin == PIN && accounts[i].is_deactivated == false) 
        {
            user_found = true;
            current_user_index = i;
            cout <<GREEN << "Welcome " << accounts[i].user_name << endl;
            break;
        }
    }
    if (!user_found) 
    {
        cout <<RED << "Invalid PIN. Please try again." << endl;
        return;
    }
    int choice;
    do 
    {
        displayBMSBanner();
        cout << "1. Deposit Money" << endl;
        cout << "2. Withdraw Money" << endl;
        cout << "3. Transfer Money" << endl;
        cout << "4. Check Balance" << endl;
        cout << "5. Pay Bill" << endl;
        cout << "6. Modify PIN" << endl;
        cout << "7. Deactivate Account" << endl;
        cout << "0. Exit" << endl;
        cin >> choice;
        while (!is_integer_input())
        {
            cout <<RED << "Invalid input." <<endl; 
            cout <<GREEN << "Please choose a valid option: ";
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
                modify_pin();
                break;
            case 7:
                deactivate_account();
                break;
            case 0:
                cout <<GREEN << "Logging out." << endl;
                break;
            default:
                cout <<RED << "Invalid option selected. Please try again." << endl;
                break;
        }
    } while (choice != 0);
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
        cout <<GREEN << "2. User Login" << endl;
        cout <<GREEN << "0. Exit" << endl;
        cout <<RESET << "Enter your choice: ";
        cin >> choice;
        while (!is_integer_input())
        {
            cout <<RED << "Invalid input." <<endl; 
            cout <<GREEN << "Please choose a valid option: ";
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
                cout <<GREEN << "Thank you for using the system!" << endl;
                break;
            default:
                cout <<RED << "Invalid option! Please try again." << endl;
                break;
        }
    } while (choice != 0);
    getch();
    return 0;
}
