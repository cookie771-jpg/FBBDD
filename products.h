#ifndef PRODUCTS_H
#define PRODUCTS_H

typedef struct {
    int productcode;
    char productname[100];
} product;

int stockProductsMenu();
product findProductsMenu();
void backProductsMenu();

#endif