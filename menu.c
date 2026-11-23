#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

#define CHAR_LENGTH 512
int use(char * tableName);
int insert(char * book_id, int bookOffset, const char * indexName); 
/* MAL, USAR ESTRUCTURAS DEFINIDAS PUNTO 3
PRIMERO QUE SE HAGA EL PUNTO 3, LUEGO HACER INTERFAZ */
int print(char * tableName);
int exit();

int main() {
    /* se pasa de funcion a funcion (abrir y cerrar fichero)*/
    char tableName[CHAR_LENGTH] = "";
    char book_id[CHAR_LENGTH] = "";
    int bookOffset = -1;
    char indexName[CHAR_LENGTH] = "";
    int option;
    /* Use: */
    do {
        printf("\n\n\n----  DATABASE OF  ----\n");
        printf("---- ZIQI & JOSEMI ----\n\n");
        printf("(1) Use\n(2) Insert\n(3) Print\n(4) Exit\n");
        printf("Enter an option > ");
        scanf("%d", &option);
        
        switch (option) {
            case 1:
                use(*tableName);
                break;
            case 2:
                insert(* book_id, bookOffset, * indexName);
                break;
            case 3:
                print(tableName);
                break;
            case 4:
                exit;
                break;
            default:
                printf("Enter a valid option\n");
                break;
        }
    } while(option != 4);
    return 0;
}


int use(char * tableName) {
    if (tableName == NULL) {
        return -1;
    }

    do {
        printf("\nEnter the Table's name >");
        fgets(tableName, CHAR_LENGTH, stdin);
        getchar();

        if (createTable(tableName) == false) {
            printf("ERROR: not valid tableName\n");
            if (strcpy(tableName, "") == NULL) {
                printf("Function Error Strcpy\n");
                return -1;
            }
        }
    } while (strcmp(tableName, "") == 0);
    
    return 0;
}


/*void use(char * tableName) {
    if (tableName == NULL) {
        return;
    }

    printf("\nEnter the Table's name >");
    fgets(tableName, CHAR_LENGTH, stdin);
    getchar();

    if (createTable(tableName) == false) {
        printf("ERROR: not valid tableName\n");
    }    
    return;
}*/

int insert(char * book_id, int bookOffset, const char * indexName) {}

/*
bool createTable(const char * tableName);
bool createIndex(const char * indexName);
bool findKey(const char * book_id, const char * indexName,
             int * nodeIDOrDataOffset);
bool addTableEntry(Book * book, const char * tableName,
                   const char * indexName);
bool addIndexEntry(char * book_id, int bookOffset, const char * indexName);

void printTree(size_t level, const char * indexName);

*/