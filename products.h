#ifndef PRODUCTS_H
#define PRODUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"


int products_stock();
int products_find();
void menu_products(SQLHDBC dbc);
#endif /* PRODUCTS_H */