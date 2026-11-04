/**
 * @file products.c
 * @author José Miguel Romero Oubiña
 * @brief C file of the products' menu 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "products.h"
#include "odbc.h"
#include "pagination.h"

/** @brief Buffer length for string operations */
#define BufferLength 512

/** @brief Menu's prompt to choose the action you want  */
#define PROMPT "Enter a number that corresponds to your choice > "

/**
 * @brief Function that shows the stock of a specific product code
 * 
 * @param dbc connection handle
 * @author José Miguel Romero Oubiña
 */
static void products_stock(SQLHDBC dbc) {
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    int ret; /* odbc.c */
    SQLRETURN ret2; /* ODBC API return status */
    int quantityinstock = 0;
    char productcode[BufferLength] = "\0";
    SQLLEN cbProductCode = SQL_NTS;

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) { return; }

    /* Prepares the SQL statement */
    ret = SQLPrepare(stmt, (SQLCHAR*) "SELECT quantityinstock FROM products WHERE productcode = ?", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    printf("Enter productcode > ");
    (void) fflush(stdout);

    /* Scans the user's input */
    if (scanf("%511s", productcode) != EOF) {
        (void) getchar();
        cbProductCode = SQL_NTS;
        (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                                SQL_C_CHAR, SQL_CHAR,
                                BufferLength, 0, productcode, BufferLength, &cbProductCode);

        if (SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
            (void) SQLBindCol(stmt, 1, SQL_INTEGER,(SQLPOINTER) &quantityinstock, 0, NULL);

            /* If the productcode exists, prints the stock available */
            if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
                printf("\nQuantity of %s in stock: %d\n", productcode, quantityinstock);
                (void) SQLCloseCursor(stmt);
                (void) fflush(stdout);

                
            } else {
                printf("No data found for productcode: %s\n", productcode);
            }
        } else {
            odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        }

        (void) SQLCloseCursor(stmt);
        (void) fflush(stdout);
    }
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
 * @brief Function that shows the products that match with the name you enter
 * 
 * @param dbc connection handle
 * @author José Miguel Romero Oubiña
 */
static void products_find(SQLHDBC dbc) {
    SQLHSTMT stmt = SQL_NULL_HSTMT;
    int ret; /* odbc.c */
    SQLRETURN ret2; /* ODBC API return status */
    char productcode[BufferLength] = "\0";
    char productname[BufferLength] = "\0";
    char buf[2*BufferLength] = "%";
    int bufLen;
    int flag = 0;

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) { return; }

    /* Prepare the SQL statement */
    ret = SQLPrepare(stmt, (SQLCHAR*) "SELECT p.productcode, p.productname FROM products p WHERE p.productname LIKE ? ORDER BY p.productcode", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    printf("Enter productname > ");
    (void) fflush(stdout);

    if (fgets(buf + 1, BufferLength, stdin) != NULL) {

        /* quitar el salto de linea solo si existe */
        buf[strcspn(buf, "\r\n")] = '\0';

        bufLen = (int) strlen(buf);
        buf[bufLen] = '%';
        buf[bufLen+1] = '\0';

        (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                                SQL_C_CHAR, SQL_CHAR,
                                BufferLength, 0, buf, 0, NULL);

        ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            odbc_extract_error("", stmt, SQL_HANDLE_STMT);
            (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return;
        }

        (void) SQLBindCol(stmt, 1, SQL_C_CHAR, (SQLCHAR*) productcode, BufferLength, NULL);
        (void) SQLBindCol(stmt, 2, SQL_C_CHAR, (SQLCHAR*) productname, BufferLength , NULL);

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            printf("%s \t %s\n", productcode, productname);
            flag = 1;
        }
        /* if no data found */
        if (flag == 0) { printf("No data found\n"); }

        ret2 = SQLCloseCursor(stmt);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_STMT);
            (void) SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return;
        }

        (void) fflush(stdout);
    }
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
 * @brief Function that displays the products menu
 * @author José Miguel Romero Oubiña
 * @param dbc connection handle
 */
void menu_products(SQLHDBC dbc) {
    char line[16] = "\0";
    int choice = 0;

    do {
        printf("\n--- PRODUCTS' MENU ---\n");
        printf(" (1) Stock\n (2) Find Product\n (3) Back\n\n");
        printf(PROMPT);
        if (read_line(line, (int) sizeof(line)) != 0) return;
        choice = atoi(line);

        switch (choice) {
            case 1: products_stock(dbc); break;
            case 2: products_find(dbc); break;
            case 3: break;
            default: printf("\nYou have entered an invalid choice. Please try again\n");
        }
    } while (choice != 3);
}