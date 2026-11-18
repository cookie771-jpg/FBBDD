#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

#define CHAR_LENGTH 512
int use(char * tableName);
int insert();
int print();
int exit();

int main() {
    /* se pasa de funcion a funcion (abrir y cerrar fichero)*/
    char tableName[CHAR_LENGTH] = "";

    /* Use: */
    do {
    printf("\n\n\n----  DATABASE OF  ----\n");
    printf("---- ZIQI & JOSEMI ----\n\n");


    } while(7);
    
    /* Insert: */

    


    return 0;
}

int use(char * tableName) {
    if (tableName == NULL) {
        return -1;
    }

    do {
        printf("Enter the Table's name >");
        fgets(tableName, CHAR_LENGTH, stdin);
        getchar();

        if (createTable(tableName) == false) {
            printf("ERROR: ");
        }
    } while (strcmp(tableName, "") == 0);
    
    return 0;
}