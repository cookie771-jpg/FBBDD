/**
 * @file menu.c
 * @author Ziqi Zhoulin y José Miguel Romero Oubiña
 * @brief Main menu of the program
 */
#include <stdio.h>
#include <stdlib.h>
#include "odbc.h"
#include "products.h"
#include "orders.h"
#include "customers.h"
#include "pagination.h"

/**
 * @brief Main function of the program
 * @return int
 */
int main(void) {
    SQLHENV env = SQL_NULL_HENV;
    SQLHDBC dbc = SQL_NULL_HDBC;
    SQLRETURN ret;
    int opcion = 0;
    char line[16] = "\0";

    ret = (SQLRETURN) odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        fprintf(stderr, "Error al conectar.\n");
        return EXIT_FAILURE;
    }

    do {
        printf("\n--- MAIN MENU ---\n");
        printf("1. Products\n");
        printf("2. Orders\n");
        printf("3. Customers\n");
        printf("4. Exit\n");
        printf("Enter a number that corresponds to your choice > ");
        if (read_line(line, (int) sizeof(line)) != 0) break;
        opcion = atoi(line);

        switch (opcion) {
            case 1: menu_products(dbc); break;
            case 2: menu_orders(dbc); break;
            case 3: menu_customers(dbc); break;
            case 4: printf("Exiting...\n"); break;
            default: printf("\nYou have entered an invalid choice. Please try again\n");
        }
    } while (opcion != 4);

    (void) odbc_disconnect(env, dbc);
    return EXIT_SUCCESS;
}