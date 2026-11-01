#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "orders.h"
#include "odbc.h"
#include "pagination.h"

#define BufferLength 512
#define DateLength 11

int orders_open() {
    SQLHENV env = NULL;
    SQLHDBC dbc = NULL;
    SQLHSTMT stmt = NULL;
    int ret; /* odbc.c */
    SQLRETURN ret2; /* ODBC API return status */
    int ordernumber = -1;

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)){
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    ret = SQLPrepare(stmt, (SQLCHAR*) "SELECT ordernumber FROM orders WHERE shippeddate IS NULL ORDER BY ordernumber", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return ret;
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
        return ret;
    }

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)){
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
    }

int orders_range(){
    SQLHENV env = NULL;
    SQLHDBC dbc = NULL;
    SQLHSTMT stmt = NULL;
    int ret; /* odbc.c */
    SQLRETURN ret2; /* ODBC API return status*/
    int ordernumber = -1;
    char orderdate[DateLength]="\0";
    char shippeddate[DateLength]="\0";
    char guion;


    /* CONECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)){
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    ret = SQLPrepare(stmt, (SQLCHAR*) "SELECT ordernumber, orderdate, shippeddate FROM orders WHERE orderdate >= ? AND orderdate <= ? ORDER BY ordernumber", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)){
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return ret;
    }

    printf("Enter dates (YYYY-MM-DD - YYYY-MM-DD) > ");
    (void)fflush(stdout);

    if (scanf("%s %c %s", orderdate, &guion, shippeddate) != EOF){
        (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, orderdate, DateLength, NULL);
        (void) SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TYPE_DATE, 0, 0, shippeddate, DateLength, NULL);
        
        (void) SQLExecute(stmt);

        (void) SQLBindCol(stmt, 1, SQL_INTEGER, (SQLPOINTER) &ordernumber, 0, NULL);
        (void) SQLBindCol(stmt, 2, SQL_CHAR, (SQLCHAR*) orderdate, DateLength, NULL);
        (void) SQLBindCol(stmt, 3, SQL_CHAR, (SQLCHAR*) shippeddate, DateLength, NULL);
    
        do {
            ret = SQLFetch (stmt);
            if (ret != SQL_SUCCESS) strcpy(shippeddate, "NOTSHIPPED");
                if (ret != SQL_NO_DATA) {
                    printf("%d %s %s\n", ordernumber, orderdate, shippeddate);
                }
        } while (ret == SQL_SUCCESS || ret != SQL_NO_DATA);

   
        (void) SQLCloseCursor(stmt);

        (void)fflush(stdout);
    }
    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)){
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return ret;
    }

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)){
    return EXIT_FAILURE;
    }

    return EXIT_FAILURE; 
    }

int orders_detail(){
    SQLHENV env = NULL;
    SQLHDBC dbc = NULL;
    SQLHSTMT stmt = NULL;
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

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)){
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    ret = SQLPrepare(stmt, (SQLCHAR*) "SELECT o.orderdate, o.status, d.productcode, d.quantityordered, d.priceeach, (SELECT SUM(d2.quantityordered * d2.priceeach) FROM orderdetails d2 WHERE d2.ordernumber = o.ordernumber) AS total FROM orders o JOIN orderdetails d ON o.ordernumber = d.ordernumber WHERE o.ordernumber = ? ORDER BY d.orderlinenumber", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)){
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return ret;
    }

    printf("Enter ordernumber > ");
    (void)fflush(stdout);

    if (scanf("%d", &ordernumber) == 1){
        (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ordernumber, 0, NULL);

        (void) SQLExecute(stmt);

        (void) SQLBindCol(stmt, 1, SQL_C_CHAR, orderdate, DateLength, NULL);
        (void) SQLBindCol(stmt, 2, SQL_C_CHAR, status, BufferLength, NULL);
        (void) SQLBindCol(stmt, 3, SQL_C_CHAR, productcode, BufferLength, NULL);
        (void) SQLBindCol(stmt, 4, SQL_C_LONG, &quantityordered, 0, NULL);
        (void) SQLBindCol(stmt, 5, SQL_C_DOUBLE, &priceeach, 0, NULL);
        (void) SQLBindCol(stmt, 6, SQL_C_DOUBLE, &total, 0, NULL);

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
        return ret;
    }

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)){
        return EXIT_FAILURE;
    }
}

    void menu_orders();