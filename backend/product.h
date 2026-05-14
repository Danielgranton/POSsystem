#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
using namespace std;

class Product {
    private : 
        int id;
        string name;
        float price;
        int stock;

    public :
        Product(int i,string n, float p, int s);
        
        void displayProduct();

        int getId();
        string getName();
        float getPrice();
        int getStock();

        void reduceStock(int quantity);    
};

#endif