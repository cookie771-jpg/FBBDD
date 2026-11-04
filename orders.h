/**
 * @file orders.h
 * @author José Miguel Romero Oubiña
 * @brief Header file for orders functions
 */
#ifndef ORDERS_H
#define ORDERS_H

#include <sql.h>
#include "odbc.h"

/**
 * @brief Displays the orders menu
 * @author José Miguel Romero Oubiña
 * @param dbc connection handle
 */
void menu_orders(SQLHDBC dbc);

#endif /* ORDERS_H */