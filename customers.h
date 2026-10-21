#ifndef CUSTOMERS_H
#define CUSTOMERS_H

#include <sql.h>
#include <sqlext.h>

void menu_customers(SQLHDBC dbc);
void opcion_find(SQLHDBC dbc);
void opcion_list_products(SQLHDBC dbc);
void opcion_balance(SQLHDBC dbc);

#endif