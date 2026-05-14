#include <iostream>
#include <iomanip>
#include "sale.h"

using namespace std;

Sale::Sale(int pid, string pname, int qty, float t) {
    productId = pid;
    productName = pname;
    quantity = qty;
    total = t;
}

void Sale::displaySale() {
    cout << left
         << setw(8)  << productId
         << " | "
         << setw(15) << productName
         << " | "
         << setw(5)  << quantity
         << " | "
         << total
         << endl;
}