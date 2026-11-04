/**
 * @file customers.h
 * @author Ziqi Zhoulin
 * @brief Header file for customers functions
 */
#ifndef CUSTOMERS_H
#define CUSTOMERS_H

#include <sql.h>
#include <sqlext.h>

/**
 * @brief Displays the customers menu
 * @author Ziqi Zhoulin
 * @param dbc connection handle
 */
void menu_customers(SQLHDBC dbc);

#endif /* CUSTOMERS_H */
