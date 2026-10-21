#include <stdio.h>
#include <stdlib.h>
#include "pagination.h"
#include "odbc.h"

int handle_pagination(SQLHSTMT stmt, SQLHDBC dbc, int *offset, int total_params) {
    (void)dbc;
    char tecla;
    SQLRETURN ret;
    int limit = PAGE_SIZE;
    int rows_fetched = 0;
    char buffer[256];

    SQLBindParameter(stmt, total_params + 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &limit, 0, NULL);
    SQLBindParameter(stmt, total_params + 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, offset, 0, NULL);

    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        odbc_extract_error("SQLExecute", stmt, SQL_HANDLE_STMT);
        return -1;
    }

    printf("\n--- Resultados (Pagina %d) ---\n", (*offset / PAGE_SIZE) + 1);

    while (SQL_SUCCEEDED(SQLFetch(stmt))) {
        SQLGetData(stmt, 1, SQL_C_CHAR, buffer, sizeof(buffer), NULL);
        printf("%s\n", buffer);
        rows_fetched++;
    }

    if (rows_fetched == 0 && *offset == 0) {
        printf("No se encontraron resultados.\n");
        return 0;
    }

    printf("\nOpciones: [>] Siguiente, [<] Anterior, [q] Salir\n");
    printf("Seleccione una opcion: ");
    scanf(" %c", &tecla);

    if (tecla == '>') {
        if (rows_fetched == PAGE_SIZE) {
            *offset += PAGE_SIZE;
        } else {
            printf("No hay mas paginas.\n");
        }
    } else if (tecla == '<') {
        if (*offset >= PAGE_SIZE) {
            *offset -= PAGE_SIZE;
        } else {
            printf("Ya estas en la primera pagina.\n");
        }
    } else {
        return 0;
    }

    return 1;
}