#include <iostream>
#include <iomanip>
#include <limits>
#include <termios.h>
#include <unistd.h>
#include "database.h"

using namespace std;

bool readInt(const string& prompt, int& value) {
    cout << prompt;

    if (cin >> value) {
        return true;
    }

    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Invalid number entered.\n";
    return false;
}

bool readFloat(const string& prompt, float& value) {
    cout << prompt;

    if (cin >> value) {
        return true;
    }

    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Invalid price entered.\n";
    return false;
}

string getHiddenPassword() {
    termios oldt, newt;
    string password;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    getline(cin, password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    cout << endl;

    return password;
}

void adminMenu(Database& db) {
    int choice;

    do {
        cout << "\n===== ADMIN MENU =====\n";
        cout << "1. View Products\n";
        cout << "2. Add Product\n";
        cout << "3. Search Product\n";
        cout << "4. Update Stock\n";
        cout << "5. Remove Product\n";
        cout << "6. Sell Product\n";
        cout << "7. View Sales History\n";
        cout << "8. Daily Revenue\n";
        cout << "9. Low Stock Alerts\n";
        cout << "10. View Users\n";
        cout << "11. Add User\n";
        cout << "12. Remove User\n";
        cout << "13. Exit\n";
        if (!readInt("Enter choice: ", choice)) {
            continue;
        }

        switch (choice) {
            case 1:
                db.viewProducts();
                break;

            case 2: {
                int id, stock;
                string name;
                float price;

                if (!readInt("Enter Product ID: ", id)) {
                    break;
                }
                cin.ignore();

                cout << "Enter Product Name: ";
                getline(cin, name);

                if (!readFloat("Enter Price: ", price)) {
                    break;
                }

                if (id <= 0) {
                    cout << "Product ID must be greater than 0.\n";
                    break;
                }

                if (name.find_first_not_of(" \t\r\n") == string::npos) {
                    cout << "Product name cannot be empty.\n";
                    break;
                }

                if (price < 0) {
                    cout << "Price cannot be negative.\n";
                    break;
                }

                if (!readInt("Enter Stock: ", stock)) {
                    break;
                }

                if (stock < 0) {
                    cout << "Stock cannot be negative.\n";
                    break;
                }

                db.addProduct(id, name, price, stock);
                break;
            }

            case 3: {
                int id;
                if (!readInt("Enter Product ID to search: ", id)) {
                    break;
                }
                db.searchProduct(id);
                break;
            }

            case 4: {
                int id, newStock;
                if (!readInt("Enter Product ID: ", id) ||
                    !readInt("Enter New Stock: ", newStock)) {
                    break;
                }

                if (id <= 0) {
                    cout << "Product ID must be greater than 0.\n";
                    break;
                }

                if (newStock < 0) {
                    cout << "Stock cannot be negative.\n";
                    break;
                }
                db.updateStock(id, newStock);
                break;
            }

            case 5: {
                int id;
                if (!readInt("Enter Product ID to remove: ", id)) {
                    break;
                }

                if (id <= 0) {
                    cout << "Product ID must be greater than 0.\n";
                    break;
                }
                db.deleteProduct(id);
                break;
            }

            case 6: {
                int id, qty;
                if (!readInt("Enter Product ID: ", id) ||
                    !readInt("Enter Quantity: ", qty)) {
                    break;
                }

                if (id <= 0) {
                    cout << "Product ID must be greater than 0.\n";
                    break;
                }

                if (qty <= 0) {
                    cout << "Quantity must be greater than 0.\n";
                    break;
                }
                db.sellProduct(id, qty);
                break;
            }

            case 7:
                db.viewSales();
                break;

            case 8:
                db.dailyRevenue();
                break;
            case 9:
                db.lowStockProducts();
                break;        
            case 10:
                db.viewUsers();
                break;

            case 11: {
                string uname, pass, role;
                cout << "Enter Username: ";
                cin >> uname;
                cin.ignore();

                cout << "Enter Password: ";
                pass = getHiddenPassword();

                cout << "Enter Role (admin/cashier): ";
                cin >> role;

                if (uname.find_first_not_of(" \t\r\n") == string::npos) {
                    cout << "Username cannot be empty.\n";
                    break;
                }

                if (pass.find_first_not_of(" \t\r\n") == string::npos) {
                    cout << "Password cannot be empty.\n";
                    break;
                }

                if (role != "admin" && role != "cashier") {
                    cout << "Role must be either 'admin' or 'cashier'.\n";
                    break;
                }

                db.addUser(uname, pass, role);
                break;
            }

            case 12: {
                string uname;
                cout << "Enter Username to remove: ";
                cin >> uname;

                if (uname.find_first_not_of(" \t\r\n") == string::npos) {
                    cout << "Username cannot be empty.\n";
                    break;
                }

                db.removeUser(uname);
                break;
            }

            case 13:
                cout << "Exiting...\n";
                break;

            default:
                cout << "Invalid Choice!\n";
        }

    } while (choice != 13);
}

void cashierMenu(Database& db) {
    int choice;

    do {
        cout << "\n===== CASHIER MENU =====\n";
        cout << "1. View Products\n";
        cout << "2. Sell Product\n";
        cout << "3. Low stock Alerts\n";
        cout << "4. Exit\n";
        if (!readInt("Enter choice: ", choice)) {
            continue;
        }

        switch (choice) {
            case 1:
                db.viewProducts();
                break;

            case 2: {
                int id, qty;
                if (!readInt("Enter Product ID: ", id) ||
                    !readInt("Enter Quantity: ", qty)) {
                    break;
                }

                if (id <= 0) {
                    cout << "Product ID must be greater than 0.\n";
                    break;
                }

                if (qty <= 0) {
                    cout << "Quantity must be greater than 0.\n";
                    break;
                }
                db.sellProduct(id, qty);
                break;
            }

            case 3:
                db.lowStockProducts();
                break;

            case 4:
                cout << "Exiting...\n";
                break;

            default:
                cout << "Invalid Choice!\n";
        }

    } while (choice != 4);
}

int main() {
    try {
        Database db;

        string username, password;
        string role = "";
        int attempts = 3;

        cout << "=======LOGIN======\n";

        while (attempts > 0) {
            cout << "Username: ";
            cin >> username;
            cin.ignore();

            cout << "Password: ";
            password = getHiddenPassword();

            role = db.login(username, password);

            if (!role.empty()) {
                break;
            }

            attempts--;

            if (attempts > 0) {
                cout << "Invalid credentials! " << attempts << " attempt(s) remaining.\n\n";
            }
        }

        if (role.empty()) {
            cout << "Too many failed attempts. Exiting...\n";
            return 0;
        }

        cout << "\nLogin Successful! Welcome, " << username << "! Role: " << role << "\n";

        if (role == "admin") {
            adminMenu(db);
        } else if (role == "cashier") {
            cashierMenu(db);
        } else {
            cout << "Unknown role. Exiting...\n";
        }
    } catch (const std::exception& ex) {
        cerr << "Application error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
