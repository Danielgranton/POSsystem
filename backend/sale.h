#ifndef SALE_H
#define SALE_H

#include <string>

using namespace std;

class Sale {
    private: 
        int productId;
        string productName;
        int quantity;
        float total;

    public:
        Sale(int pid, string pname, int qty, float t);

        void displaySale();
};

#endif