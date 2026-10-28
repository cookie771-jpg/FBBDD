#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "odbc.h"
#include "customers.h"
#include "pagination.h"

#define PROMPT "Enter a number that corresponds to your choice > "
#define NAME_MAX_LEN 50

/* Customers whose contact first or last name contains the text (paginated). */
#define SQL_FIND \
    "SELECT customernumber, customername, contactfirstname, contactlastname " \
    "FROM customers " \
    "WHERE contactfirstname LIKE ? OR contactlastname LIKE ? " \
    "ORDER BY customernumber LIMIT ? OFFSET ?"

/* Total units per product ordered by one customer, all orders added (paginated). */
#define SQL_LIST_PRODUCTS \
    "SELECT p.productname, SUM(od.quantityordered) " \
    "FROM orders o " \
    "JOIN orderdetails od ON od.ordernumber = o.ordernumber " \
    "JOIN products p ON p.productcode = od.productcode " \
    "WHERE o.customernumber = ? " \
    "GROUP BY p.productcode, p.productname " \
    "ORDER BY p.productcode LIMIT ? OFFSET ?"

/* Payments minus purchases in ONE query. Two independent subqueries avoid
 * multiplying rows (payments x orderdetails). Returns no row if the
 * customer does not exist. */
#define SQL_BALANCE \
    "SELECT (SELECT COALESCE(SUM(pa.amount), 0) FROM payments pa " \
    "         WHERE pa.customernumber = c.customernumber) " \
    "     - (SELECT COALESCE(SUM(od.quantityordered * od.priceeach), 0) " \
    "         FROM orders o JOIN orderdetails od ON od.ordernumber = o.ordernumber " \
    "         WHERE o.customernumber = c.customernumber) " \
    "FROM customers c WHERE c.customernumber = ?"

/**
 * @brief Asks for a customer number.
 * @param out where the number is stored
 * @return 0 if a valid number was read, -1 otherwise
 * @author 
 */
static int ask_customer_number(SQLINTEGER *out) {
    char line[32];
    char *end;
    long value;

    printf("Enter customer number > ");
    if (read_line(line, sizeof(line)) != 0) return -1;
    value = strtol(line, &end, 10);
    if (end == line || *end != '\0') {
        printf("Invalid customer number.\n");
        return -1;
    }
    *out = (SQLINTEGER)value;
    return 0;
}

/**
 * @brief Customers -> Find: search by contact first or last name.
 * @param dbc open connection
 * @author 
 */
static void customers_find(SQLHDBC dbc) {
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    char text[NAME_MAX_LEN + 1];
    char pattern[NAME_MAX_LEN + 3];

    printf("Enter customer name > ");
    if (read_line(text, sizeof(text)) != 0) return;
    sprintf(pattern, "%%%s%%", text);

    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt))) return;
    if (SQL_SUCCEEDED(SQLPrepare(stmt, (SQLCHAR *)SQL_FIND, SQL_NTS))) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                         NAME_MAX_LEN + 2, 0, pattern, 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                         NAME_MAX_LEN + 2, 0, pattern, 0, NULL);
        paginate(stmt, 2);
    } else {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

/**
 * @brief Customers -> List Products: products ordered by a customer.
 * @param dbc open connection
 * @author 
 */
static void customers_list_products(SQLHDBC dbc) {
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    SQLINTEGER customer = 0;

    if (ask_customer_number(&customer) != 0) return;

    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt))) return;
    if (SQL_SUCCEEDED(SQLPrepare(stmt, (SQLCHAR *)SQL_LIST_PRODUCTS, SQL_NTS))) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                         0, 0, &customer, 0, NULL);
        paginate(stmt, 1);
    } else {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

/**
 * @brief Customers -> Balance: payments minus purchases (no pagination).
 * @param dbc open connection
 * @author 
 */
static void customers_balance(SQLHDBC dbc) {
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    SQLINTEGER customer = 0;
    double balance = 0.0;
    SQLLEN ind = 0;

    if (ask_customer_number(&customer) != 0) return;

    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt))) return;
    if (SQL_SUCCEEDED(SQLPrepare(stmt, (SQLCHAR *)SQL_BALANCE, SQL_NTS))) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                         0, 0, &customer, 0, NULL);
        if (!SQL_SUCCEEDED(SQLExecute(stmt))) {
            odbc_extract_error("SQLExecute", stmt, SQL_HANDLE_STMT);
        } else if (SQL_SUCCEEDED(SQLFetch(stmt))) {
            SQLGetData(stmt, 1, SQL_C_DOUBLE, &balance, 0, &ind);
            printf("Balance = %.2f\n", balance);
        } else {
            printf("Customer %ld not found.\n", (long)customer);
        }
    } else {
        odbc_extract_error("SQLPrepare", stmt, SQL_HANDLE_STMT);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

/**
 * @brief Customers submenu loop.
 * @param dbc open connection
 * @author 
 */
void menu_customers(SQLHDBC dbc) {
    char line[16];
    int choice = 0;

    do {
        printf("\n (1) Find\n (2) List Products\n (3) Balance\n (4) Back\n\n");
        printf(PROMPT);
        if (read_line(line, sizeof(line)) != 0) return;
        choice = atoi(line);

        switch (choice) {
            case 1: customers_find(dbc); break;
            case 2: customers_list_products(dbc); break;
            case 3: customers_balance(dbc); break;
            case 4: break;
            default: printf("You have entered an invalid choice. Please try again\n");
        }
    } while (choice != 4);
}
