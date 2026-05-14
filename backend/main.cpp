#include <iostream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>
#include "database.h"

using namespace std;

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
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                db.viewProducts();
                break;

            case 2: {
                int id, stock;
                string name;
                float price;

                cout << "Enter Product ID: ";
                cin >> id;
                cin.ignore();

                cout << "Enter Product Name: ";
                getline(cin, name);

                cout << "Enter Price: ";
                cin >> price;

                cout << "Enter Stock: ";
                cin >> stock;

                db.addProduct(id, name, price, stock);
                break;
            }

            case 3: {
                int id;
                cout << "Enter Product ID to search: ";
                cin >> id;
                db.searchProduct(id);
                break;
            }

            case 4: {
                int id, newStock;
                cout << "Enter Product ID: ";
                cin >> id;
                cout << "Enter New Stock: ";
                cin >> newStock;
                db.updateStock(id, newStock);
                break;
            }

            case 5: {
                int id;
                cout << "Enter Product ID to remove: ";
                cin >> id;
                db.deleteProduct(id);
                break;
            }

            case 6: {
                int id, qty;
                cout << "Enter Product ID: ";
                cin >> id;
                cout << "Enter Quantity: ";
                cin >> qty;
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

                db.addUser(uname, pass, role);
                break;
            }

            case 12: {
                string uname;
                cout << "Enter Username to remove: ";
                cin >> uname;
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
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                db.viewProducts();
                break;

            case 2: {
                int id, qty;
                cout << "Enter Product ID: ";
                cin >> id;
                cout << "Enter Quantity: ";
                cin >> qty;
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

    return 0;
}