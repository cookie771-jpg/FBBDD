#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"
#include "customers.h"
#include "pagination.h"

void menu_customers(SQLHDBC dbc) {
    int opcion = 0;
    do {
        printf("\n--- CUSTOMERS ---\n");
        printf("1. Find\n");
        printf("2. List Products\n");
        printf("3. Balance\n");
        printf("4. Back\n");
        printf("Enter a number that corresponds to your choice > ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: opcion_find(dbc); break;
            case 2: opcion_list_products(dbc); break;
            case 3: opcion_balance(dbc); break;
            case 4: printf("Volviendo...\n"); break;
            default: printf("Opcion no valida.\n");
        }
    } while (opcion != 4);
}

void opcion_find(SQLHDBC dbc) {
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    SQLRETURN ret;
    char busqueda[50];
    char like_busqueda[54];
    int offset = 0;
    int continuar = 1;

    printf("Introduce nombre o apellido: ");
    scanf("%49s", busqueda);
    snprintf(like_busqueda, sizeof(like_busqueda), "%%%s%%", busqueda);

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) return;

    ret = SQLPrepare(stmt, (SQLCHAR*)"SELECT customernumber, customername, contactfirstname, contactlastname FROM customers WHERE contactfirstname LIKE ? OR contactlastname LIKE ? ORDER BY customernumber LIMIT ? OFFSET ?", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) { SQLFreeHandle(SQL_HANDLE_STMT, stmt); return; }

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, like_busqueda, 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, like_busqueda, 0, NULL);

    while (continuar) {
        continuar = handle_pagination(stmt, dbc, &offset, 2);
        if (continuar == 1) {
            SQLCloseCursor(stmt);
        } else {
            break;
        }
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void opcion_list_products(SQLHDBC dbc) {
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    SQLRETURN ret;
    int customernumber;
    int offset = 0;
    int continuar = 1;

    printf("Introduce customer number: ");
    scanf("%d", &customernumber);

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) return;

    ret = SQLPrepare(stmt, (SQLCHAR*)"SELECT p.productCode, p.productName, SUM(od.quantityOrdered) FROM products p JOIN orderdetails od ON p.productCode = od.productCode JOIN orders o ON od.orderNumber = o.orderNumber WHERE o.customerNumber = ? GROUP BY p.productCode, p.productName ORDER BY p.productCode LIMIT ? OFFSET ?", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) { SQLFreeHandle(SQL_HANDLE_STMT, stmt); return; }

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &customernumber, 0, NULL);

    while (continuar) {
        continuar = handle_pagination(stmt, dbc, &offset, 1);
        if (continuar == 1) {
            SQLCloseCursor(stmt);
        } else {
            break;
        }
    }
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void opcion_balance(SQLHDBC dbc) {
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    SQLRETURN ret;
    int customernumber;
    double pagos = 0.0, compras = 0.0;
    SQLLEN ind_pagos, ind_compras;

    printf("Introduce customer number: ");
    scanf("%d", &customernumber);

    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) return;

    ret = SQLPrepare(stmt, (SQLCHAR*)"SELECT SUM(amount) FROM payments WHERE customerNumber = ?", SQL_NTS);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &customernumber, 0, NULL);
    SQLExecute(stmt);
    if (SQL_SUCCEEDED(SQLFetch(stmt))) {
        SQLGetData(stmt, 1, SQL_C_DOUBLE, &pagos, 0, &ind_pagos);
        if (ind_pagos == SQL_NULL_DATA) pagos = 0.0;
    }
    SQLCloseCursor(stmt);

    ret = SQLPrepare(stmt, (SQLCHAR*)"SELECT SUM(quantityOrdered * priceEach) FROM orderdetails JOIN orders ON orderdetails.orderNumber = orders.orderNumber WHERE orders.customerNumber = ?", SQL_NTS);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &customernumber, 0, NULL);
    SQLExecute(stmt);
    if (SQL_SUCCEEDED(SQLFetch(stmt))) {
        SQLGetData(stmt, 1, SQL_C_DOUBLE, &compras, 0, &ind_compras);
        if (ind_compras == SQL_NULL_DATA) compras = 0.0;
    }

    printf("Balance: %.2f\n", pagos - compras);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}