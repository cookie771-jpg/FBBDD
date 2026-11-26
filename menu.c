#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

#define CHAR_LENGTH 512

/*
typedef struct book {
    char book_id[PK_SIZE ];
    size_t title_len;
    char *title; 
} Book;


typedef struct node {
    char book_id[PK_SIZE];
    int left, right, parent, offset;
}Node;
*/

void use(char * tableName);
int print(char * tableName);
int exit_menu();

int main() {
    /* se pasa de funcion a funcion (abrir y cerrar fichero)*/
    char tableName[CHAR_LENGTH] = "";
    int option;
    Book * book = NULL;
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
                break;
            case 3:
                break;
            case 4:
                break;
            default:
                printf("Enter a valid option\n");
                break;
        }
    } while(option != 4);
    return 0;
}

void use(char * tableName) {
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
}

bool insert(Book *book, const char * tableName, const char * indexName) {
    int flag = 0;
    char bk_id[PK_SIZE + 2] = "";
    char bk_title[512] = "";

    if (book == NULL || tableName == NULL || indexName == NULL) {
        printf("ERROR: You haven't used the command Use\n");
        return false;
    }

    printf("Enter the Book ID > ");
    if (fgets(bk_id, sizeof(bk_id), stdin) == NULL) {
        return false;
    }
    bk_id[strcspn(bk_id, "\n")] = '\0';
    if (strlen(bk_id) != PK_SIZE) {
        printf("ERROR: Book ID must be exactly 4 characters\n");
        return false;
    }

    printf("Enter the Book Title > ");
    if (fgets(bk_title, sizeof(bk_title), stdin) == NULL) {
        return false;
    }
    bk_title[strcspn(bk_title, "\n")] = '\0';

    strncpy(book->book_id, bk_id, PK_SIZE);
    book->title_len = strlen(bk_title);
    strncpy(book->title, bk_title, book->title_len);

    if (addTableEntry(book, tableName, indexName) == false) {
        printf("ERROR: Could not insert the book\n");
        return false;
    }

    return true;
}

int exit_menu() {return 0; }
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