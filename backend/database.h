#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>
#include <string>

class Database {

    private:
        pqxx::connection conn;

    public:
        Database();

        std::string login(
            const std::string& username,
            const std::string& password
        );

        void addUser(
            const std::string& username,
            const std::string& password,
            const std::string& role
        );
        void removeUser(const std::string& username);
        void viewUsers();

        void addProduct(
            int id,
            const std::string& name,
            float price,
            int stock
        );
        void viewProducts();
        void searchProduct(int id);
        void updateStock(int id, int newStock);
        void deleteProduct(int id);

        void sellProduct(int id, int qty);
        void viewSales();
        void dailyRevenue();
        void lowStockProducts();
};

#endif