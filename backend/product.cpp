#include <iostream>
#include "product.h"

using namespace std;

Product::Product(int i, string n, float p, int s) {
    id = i;
    name = n;
    price = p;
    stock = s;
}

void Product::displayProduct() {
    cout<< id << " | "
        << name << " | "
        << price << " | "
        << stock << endl;
}

int Product::getId(){
    return id;
}

string Product::getName() {
    return name;
}

float Product::getPrice(){
    return price;
}

int Product::getStock(){
    return stock;
}

void Product::reduceStock(int quantity) {
    stock -= quantity;
}