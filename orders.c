/**
 * @file orders.c
 * @author José Miguel Romero Oubiña
 * @brief Implementation of orders functions
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "orders.h"
#include "odbc.h"
#include "pagination.h"

/** @brief Buffer length for string fields */
#define BufferLength 512
/** @brief Date length */
#define DateLength 11
/** @brief Prompt for user input */
#define PROMPT "Enter a number that corresponds to your choice > "

/**
 * @brief Displays not shipped orders
 * @author José Miguel Romero Oubiña
 * @param dbc connection handle
 */
static void orders_open(SQLHDBC dbc) {
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    int ret; /* odbc.c */
    SQLRETURN ret2; /* ODBC API return status */
    int ordernumber = -1;

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    /* Prepare the SQL statement */
    ret = SQLPrepare(stmt, (SQLCHAR*) "SELECT ordernumber FROM orders WHERE shippeddate IS NULL ORDER BY ordernumber", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return;
    }

    printf("\nordernumber of NOT SHIPPED orders: \n");
    (void) SQLExecute(stmt);

    (void) SQLBindCol(stmt, 1, SQL_INTEGER, (SQLPOINTER) &ordernumber, 0, NULL);
    
    /* Loop through the rows in the result-set */
    while(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
        printf("%d\n", ordernumber);
    }	

    (void) SQLCloseCursor(stmt);
    (void) fflush(stdout);

    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return;
    }

    return;
}

/**
 * @brief Displays orders within a date range
 * @author José Miguel Romero Oubiña
 * @param dbc connection handle
 */
static void orders_range(SQLHDBC dbc){
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    int ret; /* odbc.c */
    SQLRETURN ret2; /* ODBC API return status*/
    int ordernumber = -1;
    char orderdate[DateLength]="\0";
    char shippeddate[DateLength]="\0";
    char guion;
    SQLLEN ind_shipped = 0;
    int found = 0;

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* Prepare the SQL statement */
    ret = SQLPrepare(stmt, (SQLCHAR*) "SELECT ordernumber, orderdate, shippeddate FROM orders WHERE orderdate >= ? AND orderdate <= ? ORDER BY ordernumber", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)){
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return;
    }

    printf("Enter dates (YYYY-MM-DD - YYYY-MM-DD) > ");
    (void)fflush(stdout);

    /* Scan the user's input */
    if (scanf("%10s %c %10s", orderdate, &guion, shippeddate) != EOF){
        (void) getchar();
        (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, orderdate, DateLength, NULL);
        (void) SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, shippeddate, DateLength, NULL);
        
        (void) SQLExecute(stmt);

        (void) SQLBindCol(stmt, 1, SQL_INTEGER, (SQLPOINTER) &ordernumber, 0, NULL);
        (void) SQLBindCol(stmt, 2, SQL_CHAR, (SQLCHAR*) orderdate, DateLength, NULL);
        (void) SQLBindCol(stmt, 3, SQL_CHAR, (SQLCHAR*) shippeddate, DateLength, &ind_shipped);
        
        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            if (ind_shipped == SQL_NULL_DATA) {
                strcpy(shippeddate, "NOTSHIPPED");
            }
            printf("%d %s %s\n", ordernumber, orderdate, shippeddate);
            found = 1;
        }
        /* if no data found */
        if (found == 0) {
            printf("No data found in that range\n");
        }
   
        (void) SQLCloseCursor(stmt);

        (void)fflush(stdout);
    }
    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)){
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return;
    }

    return; 
}

/**
 * @brief Displays the details of a specific order
 * @author José Miguel Romero Oubiña
 * @param dbc connection handle
 */
static void orders_detail(SQLHDBC dbc){
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    int ret;
    SQLRETURN ret2;
    int ordernumber = 0;
    char orderdate[DateLength] = "";
    char status[BufferLength] = "";
    char productcode[BufferLength] = "";
    int quantityordered = 0;
    double priceeach = 0.0;
    double total = 0.0;
    int i = 0;

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* Prepare the SQL statement */
    ret = SQLPrepare(stmt, (SQLCHAR*) "SELECT o.orderdate, o.status, d.productcode, d.quantityordered, d.priceeach, (SELECT SUM(d2.quantityordered * d2.priceeach) FROM orderdetails d2 WHERE d2.ordernumber = o.ordernumber) AS total FROM orders o JOIN orderdetails d ON o.ordernumber = d.ordernumber WHERE o.ordernumber = ? ORDER BY d.orderlinenumber", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)){
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return;
    }

    printf("Enter ordernumber > ");
    (void)fflush(stdout);

    /* Scan the user's input */
    if (scanf("%d", &ordernumber) == 1){
        (void) getchar();
        (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ordernumber, 0, NULL);

        (void) SQLExecute(stmt);

        (void) SQLBindCol(stmt, 1, SQL_C_CHAR, orderdate, DateLength, NULL);
        (void) SQLBindCol(stmt, 2, SQL_C_CHAR, status, BufferLength, NULL);
        (void) SQLBindCol(stmt, 3, SQL_C_CHAR, productcode, BufferLength, NULL);
        (void) SQLBindCol(stmt, 4, SQL_C_LONG, &quantityordered, 0, NULL);
        (void) SQLBindCol(stmt, 5, SQL_C_DOUBLE, &priceeach, 0, NULL);
        (void) SQLBindCol(stmt, 6, SQL_C_DOUBLE, &total, 0, NULL);

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
            if (i == 0){
                printf("%s %s\n", orderdate, status);
                printf("%.2f\n", total);
                i++;
            }
            printf("%s %d %.2f\n", productcode, quantityordered, priceeach);
        }

        if (i == 0){
            printf("Order %d not found\n", ordernumber);
        }

        (void) SQLCloseCursor(stmt);
        (void) fflush(stdout);
    }

    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)){
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return;
    }

    return;
}

/**
 * @brief Displays the orders menu
 * @author José Miguel Romero Oubiña
 * @param dbc connection handle
 */
void menu_orders(SQLHDBC dbc) {
    char line[16] = "\0";
    int choice = 0;

    do {
        printf("\n--- ORDERS' MENU ---\n");
        printf(" (1) Open\n (2) Range\n (3) Details\n (4) Back\n\n");
        printf(PROMPT);
        if (read_line(line, (int) sizeof(line)) != 0) return;
        choice = atoi(line);

        switch (choice) {
            case 1: orders_open(dbc); break;
            case 2: orders_range(dbc); break;
            case 3: orders_detail(dbc); break;
            case 4: break;
            default: printf("\nYou have entered an invalid choice. Please try again\n");
        }
    } while (choice != 4);
}