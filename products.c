#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "products.h"
#include "odbc.h"
#include "pagination.h"

#define PROMPT "Enter a number that corresponds to your choice > "

int products_stock() {
    SQLHENV env = NULL;
    SQLHDBC dbc = NULL;
    SQLHSTMT stmt = NULL;
    int ret; /* odbc.c */
    SQLRETURN ret2; /* ODBC API return status */
    #define BufferLength 512
    int  quantityinstock = 0;
    char productcode[BufferLength] = "\0";
    SQLLEN cbProductCode = SQL_NTS;

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_disconnect(env, dbc);
        return EXIT_FAILURE;
    }

    ret = SQLPrepare(stmt, (SQLCHAR*) "SELECT quantityinstock FROM products WHERE productcode = ?", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return EXIT_FAILURE;
    }

    printf("Enter productcode > ");
    (void) fflush(stdout);

    while (scanf("%511s", productcode) != EOF) {
        cbProductCode = SQL_NTS;
        (void) SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
                                SQL_C_CHAR, SQL_CHAR,
                                BufferLength, 0, productcode, BufferLength, &cbProductCode);

        if (SQL_SUCCEEDED(ret = SQLExecute(stmt))) {
            (void) SQLBindCol(stmt, 1, SQL_INTEGER,(SQLPOINTER) &quantityinstock, 0, NULL);

            /* Loop through the rows in the result-set */
            if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
                printf("quantityinstock = %d\n", quantityinstock);
            } else {
                printf("No data found for productcode: %s\n", productcode);
            }
        } else {
            odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        }

        (void) SQLCloseCursor(stmt);
        printf("Enter productcode > ");
        (void) fflush(stdout);
    }
    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        odbc_disconnect(env, dbc);
        return EXIT_FAILURE;
    }

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


int products_find() {
    SQLHENV env = NULL;
    SQLHDBC dbc = NULL;
    SQLHSTMT stmt = NULL;
    int ret; /* odbc.c */
    SQLRETURN ret2; /* ODBC API return status */
    #define BufferLength 512
    char productcode[BufferLength] = "\0";
    char productname[BufferLength] = "\0";
    char buf[2*BufferLength] = "%";
    int bufLen;

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_disconnect(env, dbc);
        return EXIT_FAILURE;
    }

    ret = SQLPrepare(stmt, (SQLCHAR*) "SELECT p.productcode, p.productname FROM products p WHERE p.productname LIKE ? ORDER BY p.productcode", SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        odbc_disconnect(env, dbc);
        return EXIT_FAILURE;
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
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            odbc_disconnect(env, dbc);
            return EXIT_FAILURE;
        }

        (void) SQLBindCol(stmt, 1, SQL_C_CHAR, (SQLCHAR*) productcode, BufferLength, NULL);
        (void) SQLBindCol(stmt, 2, SQL_C_CHAR, (SQLCHAR*) productname, BufferLength , NULL);

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            printf("%s \t %s\n", productcode, productname);
        }

        ret2 = SQLCloseCursor(stmt);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_STMT);
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            odbc_disconnect(env, dbc);
            return EXIT_FAILURE;
        }

        printf("Enter productname > ");
        (void) fflush(stdout);
    }

    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        odbc_disconnect(env, dbc);
        return EXIT_FAILURE;
    }

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void menu_products(SQLHDBC dbc) {
    char line[16];
    int choice = 0;

    do {
        printf("\n (1) Stock\n (2) Find Product\n (3) Back\n\n");
        printf(PROMPT);
        if (read_line(line, sizeof(line)) != 0) return;
        choice = atoi(line);

        switch (choice) {
            case 1: products_stock(dbc); break;
            case 2: products_find(dbc); break;
            case 3: break;
            default: printf("You have entered an invalid choice. Please try again\n");
        }
    } while (choice != 3);
}