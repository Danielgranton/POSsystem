#include <iostream>
#include <iomanip>
#include "database.h"

using namespace std;
using namespace pqxx;

Database::Database()
        : conn(
           "dbname=pos_system user=pos_user password=possystem port=5433" 
        ) 
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
    work txn(conn);

    txn.exec(
        "INSERT INTO users (username, password, role) VALUES ($1, $2, $3)",
        pqxx::params(username, password, role)
    );

    txn.commit();
    cout << "User added successfully!\n";
}

void Database::removeUser(const string& username) {
    work txn(conn);

    txn.exec(
        "DELETE FROM users WHERE username = $1",
        pqxx::params(username)
    );

    txn.commit();
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
    work txn(conn);

    string query = 
        "INSERT INTO products(id, name, price, stock) VALUES(" +
        to_string(id) + ",'" +
        name + "', " +
        to_string(price) + ", " +
        to_string(stock) + ");";

    txn.exec(query);
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
    nontransaction txn(conn);

    string query = "SELECT * FROM products WHERE id = " + to_string(id);
    result res = txn.exec(query);

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
    work txn(conn);

    string query = 
        "UPDATE products SET stock = " + to_string(newStock) 
        + " WHERE id = " + to_string(id);

    txn.exec(query);
    txn.commit();

    cout << "Stock updated successfully!\n";
}

void Database::deleteProduct(int id) {
    work txn(conn);

    string query = "DELETE FROM products WHERE id = " + to_string(id);

    txn.exec(query);
    txn.commit();
    
    cout << "Product deleted successfully!\n";
}

void Database::sellProduct(int id, int qty) {

    work txn(conn);

    string query =
        "SELECT * FROM products WHERE id = " +
        to_string(id);

    result res = txn.exec(query);

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

    string updateQuery =
        "UPDATE products SET stock = " +
        to_string(newStock) +
        " WHERE id = " +
        to_string(id);

    txn.exec(updateQuery);

    string saleQuery =
        "INSERT INTO sales(product_id, product_name, quantity, total) VALUES(" +
        to_string(id) + ", '" +
        name + "', " +
        to_string(qty) + ", " +
        to_string(total) + ") RETURNING invoice_no, created_at;";

    result saleRes =
        txn.exec(saleQuery);

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
        res[0]["revenue"].as<float>();

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