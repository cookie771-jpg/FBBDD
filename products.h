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
 * @brief Function that displays the products menu
 * @author José Miguel Romero Oubiña
 * @param dbc connection handle
 */
void menu_products(SQLHDBC dbc);
#endif /* PRODUCTS_H */