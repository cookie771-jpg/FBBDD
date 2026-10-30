#include <stdio.h>
#include <stdlib.h>
#include "odbc.h"
#include "products.h"
#include "customers.h"
#include "pagination.h"

int main(void) {
    SQLHENV env = SQL_NULL_HENV;
    SQLHDBC dbc = SQL_NULL_HDBC;
    SQLRETURN ret;
    int opcion = 0;
    char line[16];

    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        fprintf(stderr, "Error al conectar.\n");
        return EXIT_FAILURE;
    }

    do {
        printf("\n--- MENU PRINCIPAL ---\n");
        printf("1. Products\n");
        printf("2. Orders\n");
        printf("3. Customers\n");
        printf("4. Exit\n");
        printf("Enter a number that corresponds to your choice > ");
        if (read_line(line, sizeof(line)) != 0) break;
        opcion = atoi(line);

        switch (opcion) {
            case 1: menu_products(dbc); break;
            case 2: printf("Pendiente Persona A\n"); break;
            case 3: menu_customers(dbc); break;
            case 4: printf("Saliendo...\n"); break;
            default: printf("Opcion no valida.\n");
        }
    } while (opcion != 4);

    odbc_disconnect(env, dbc);
    return EXIT_SUCCESS;
}