#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include "database.h"

using namespace std;
using namespace pqxx;

namespace {
string trim(const string& value) {
    const auto first = find_if_not(value.begin(), value.end(), [](unsigned char ch) {
        return isspace(ch);
    });

    const auto last = find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) {
        return isspace(ch);
    }).base();

    if (first >= last) {
        return "";
    }

    return string(first, last);
}

string getEnvOrDefault(const char* key, const string& fallback) {
    const char* value = getenv(key);

    if (value == nullptr) {
        return fallback;
    }

    const string cleanValue = trim(value);
    return cleanValue.empty() ? fallback : cleanValue;
}

string buildConnectionString() {
    const string dbName = getEnvOrDefault("POS_DB_NAME", "pos_system");
    const string user = getEnvOrDefault("POS_DB_USER", "pos_user");
    const string password = getEnvOrDefault("POS_DB_PASSWORD", "possystem");
    const string host = getEnvOrDefault("POS_DB_HOST", "");
    const string port = getEnvOrDefault("POS_DB_PORT", "5433");

    string connectionString =
        "dbname=" + dbName +
        " user=" + user +
        " password=" + password +
        " port=" + port;

    if (!host.empty()) {
        connectionString += " host=" + host;
    }

    return connectionString;
}
}

Database::Database()
        : conn(buildConnectionString()) 
        {
            if(conn.is_open()) {
                cout<<"Database connected!\n";
            } else {
                cout<< "Failed to connect!\n";
            }
        }  
        
string Database::login(const string& username, const string& password) {
    nontransaction txn(conn);

    result res = txn.exec(
        "SELECT role FROM users WHERE username = $1 AND password = $2",
        pqxx::params(username, password)
    );

    if (res.empty()) {
        return "";
    }

    return res[0]["role"].as<string>();
}

void Database::addUser(const string& username, const string& password, const string& role) {
    const string cleanUsername = trim(username);
    const string cleanPassword = trim(password);
    const string cleanRole = trim(role);

    if (cleanUsername.empty()) {
        cout << "Username cannot be empty.\n";
        return;
    }

    if (cleanPassword.empty()) {
        cout << "Password cannot be empty.\n";
        return;
    }

    if (cleanRole != "admin" && cleanRole != "cashier") {
        cout << "Role must be either 'admin' or 'cashier'.\n";
        return;
    }

    work txn(conn);

    txn.exec(
        "INSERT INTO users (username, password, role) VALUES ($1, $2, $3)",
        pqxx::params(cleanUsername, cleanPassword, cleanRole)
    );

    txn.commit();
    cout << "User added successfully!\n";
}

void Database::removeUser(const string& username) {
    const string cleanUsername = trim(username);

    if (cleanUsername.empty()) {
        cout << "Username cannot be empty.\n";
        return;
    }

    work txn(conn);

    result res = txn.exec(
        "DELETE FROM users WHERE username = $1",
        pqxx::params(cleanUsername)
    );

    txn.commit();

    if (res.affected_rows() == 0) {
        cout << "User not found.\n";
        return;
    }

    cout << "User removed successfully!\n";
}

void Database::viewUsers() {
    nontransaction txn(conn);

    result res = txn.exec("SELECT id, username, role FROM users");

    cout << "\n";
    cout << left
         << setw(5)  << "ID"
         << " | "
         << setw(15) << "USERNAME"
         << " | "
         << "ROLE"
         << endl;
    cout << string(35, '-') << endl;

    for (auto row : res) {
        cout << left
             << setw(5)  << row["id"].as<int>()
             << " | "
             << setw(15) << row["username"].as<string>()
             << " | "
             << row["role"].as<string>()
             << endl;
    }
}

void Database::addProduct(int id, const std::string& name, float price, int stock) {
    const string cleanName = trim(name);

    if (id <= 0) {
        cout << "Product ID must be greater than 0.\n";
        return;
    }

    if (cleanName.empty()) {
        cout << "Product name cannot be empty.\n";
        return;
    }

    if (price < 0) {
        cout << "Price cannot be negative.\n";
        return;
    }

    if (stock < 0) {
        cout << "Stock cannot be negative.\n";
        return;
    }

    work txn(conn);

    txn.exec(
        "INSERT INTO products(id, name, price, stock) VALUES($1, $2, $3, $4)",
        pqxx::params(id, cleanName, price, stock)
    );
    txn.commit();

    cout << "Product Added to Database\n";
}

void Database::viewProducts() {
    nontransaction txn(conn);
    result res = txn.exec("SELECT * FROM products");

    cout << "\n";
    cout << left
         << setw(6)  << "ID"
         << " | "
         << setw(15) << "NAME"
         << " | "
         << setw(8)  << "PRICE"
         << " | "
         << "STOCK"
         << endl;
    cout << string(40, '-') << endl;

    for(auto row : res) {
        cout << left
             << setw(6)  << row["id"].as<int>()
             << " | "
             << setw(15) << row["name"].as<string>()
             << " | "
             << setw(8)  << row["price"].as<float>()
             << " | "
             << row["stock"].as<int>()
             << endl;
    }
}

void Database::searchProduct(int id) {
    if (id <= 0) {
        cout << "Product ID must be greater than 0.\n";
        return;
    }

    nontransaction txn(conn);

    result res = txn.exec(
        "SELECT * FROM products WHERE id = $1",
        pqxx::params(id)
    );

    if(res.empty()) {
        cout << "Product Not Found\n";
    } else {
        cout << "\n";
        cout << left
             << setw(6)  << "ID"
             << " | "
             << setw(15) << "NAME"
             << " | "
             << setw(8)  << "PRICE"
             << " | "
             << "STOCK"
             << endl;
        cout << string(40, '-') << endl;

        for(auto row : res) {
            cout << left
                 << setw(6)  << row["id"].as<int>()
                 << " | "
                 << setw(15) << row["name"].as<string>()
                 << " | "
                 << setw(8)  << row["price"].as<float>()
                 << " | "
                 << row["stock"].as<int>()
                 << endl;
        }
    }
}

void Database::updateStock(int id, int newStock) {
    if (id <= 0) {
        cout << "Product ID must be greater than 0.\n";
        return;
    }

    if (newStock < 0) {
        cout << "Stock cannot be negative.\n";
        return;
    }

    work txn(conn);

    result res = txn.exec(
        "UPDATE products SET stock = $1 WHERE id = $2",
        pqxx::params(newStock, id)
    );
    txn.commit();

    if (res.affected_rows() == 0) {
        cout << "Product not found.\n";
        return;
    }

    cout << "Stock updated successfully!\n";
}

void Database::deleteProduct(int id) {
    if (id <= 0) {
        cout << "Product ID must be greater than 0.\n";
        return;
    }

    work txn(conn);

    result res = txn.exec(
        "DELETE FROM products WHERE id = $1",
        pqxx::params(id)
    );
    txn.commit();

    if (res.affected_rows() == 0) {
        cout << "Product not found.\n";
        return;
    }

    cout << "Product deleted successfully!\n";
}

void Database::sellProduct(int id, int qty) {
    if (id <= 0) {
        cout << "Product ID must be greater than 0.\n";
        return;
    }

    if (qty <= 0) {
        cout << "Quantity must be greater than 0.\n";
        return;
    }

    work txn(conn);

    result res = txn.exec(
        "SELECT * FROM products WHERE id = $1",
        pqxx::params(id)
    );

    if(res.empty()) {

        cout << "Product Not Found!\n";

        return;
    }

    int stock =
        res[0]["stock"].as<int>();

    string name =
        res[0]["name"].as<string>();

    float price =
        res[0]["price"].as<float>();

    if(qty > stock) {

        cout << "Not Enough Stock!\n";

        return;
    }

    float total = qty * price;

    int newStock = stock - qty;

    txn.exec(
        "UPDATE products SET stock = $1 WHERE id = $2",
        pqxx::params(newStock, id)
    );

    result saleRes = txn.exec(
        "INSERT INTO sales(product_id, product_name, quantity, total) "
        "VALUES($1, $2, $3, $4) RETURNING invoice_no, created_at",
        pqxx::params(id, name, qty, total)
    );

    txn.commit();

    int invoiceNo =
        saleRes[0]["invoice_no"].as<int>();

    string createdAt =
        saleRes[0]["created_at"].as<string>();

    cout << fixed << setprecision(2);

    cout << "\n=================================\n";
    cout << "          SALES RECEIPT          \n";
    cout << "=================================\n";

    cout << left
         << setw(15) << "Invoice No"
         << ": " << invoiceNo << endl;

    cout << setw(15) << "Date"
         << ": " << createdAt << endl;

    cout << setw(15) << "Product"
         << ": " << name << endl;

    cout << setw(15) << "Quantity"
         << ": " << qty << endl;

    cout << setw(15) << "Unit Price"
         << ": KES " << price << endl;

    cout << setw(15) << "Total"
         << ": KES " << total << endl;

    cout << setw(15) << "Status"
         << ": PAID" << endl;

    cout << "=================================\n";
    cout << "      THANK YOU FOR SHOPPING     \n";
    cout << "=================================\n";
}

void Database::viewSales() {
    nontransaction txn(conn);
    result res = txn.exec("SELECT * FROM sales");

    cout << "\n";
    cout << left
         << setw(8)  << "SALE ID"
         << " | "
         << setw(15) << "PRODUCT"
         << " | "
         << setw(5)  << "QTY"
         << " | "
         << "TOTAL"
         << endl;
    cout << string(45, '-') << endl;

    for(auto row : res) {
        cout << left
             << setw(8)  << row["sale_id"].as<int>()
             << " | "
             << setw(15) << row["product_name"].as<string>()
             << " | "
             << setw(5)  << row["quantity"].as<int>()
             << " | "
             << row["total"].as<float>()
             << endl;
    }
}

void Database::dailyRevenue() {

    nontransaction txn(conn);

    result res =
        txn.exec(
            "SELECT SUM(total) AS revenue FROM sales"
        );

    float revenue =
        res[0]["revenue"].is_null() ? 0.0f : res[0]["revenue"].as<float>();

    cout << "\n===== DAILY REVENUE =====\n";

    cout << "Total Revenue: "
         << revenue
         << endl;
}

void Database::lowStockProducts() {

    nontransaction txn(conn);

    result res =
        txn.exec(
            "SELECT * FROM products WHERE stock < 5"
        );

    cout << "\n===== LOW STOCK PRODUCTS =====\n";

    if(res.empty()) {

        cout << "No Low Stock Products!\n";

        return;
    }

    for(auto row : res) {

        cout
            << row["id"].as<int>() << " | "
            << row["name"].as<string>() << " | "
            << row["stock"].as<int>()
            << endl;
    }
}
