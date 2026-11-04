/**
 * @file products.h
 * @author José Miguel Romero Oubiña
 * @brief Arhivo de cabecera del submenú products 
 */
#ifndef PRODUCTS_H
#define PRODUCTS_H

#include <sql.h>
#include "odbc.h"

/**
 * @brief products' menu that allows to know the stock and to find the product you need
 * @author José Miguel Romero Oubiña
 * @param dbc 
 */
void menu_products(SQLHDBC dbc);
#endif /* PRODUCTS_H */