#include "utils.h"
#include <string.h>

int no_deleted_registers = NO_DELETED_REGISTERS;

void replaceExtensionByIdx(const char *fileName, char *indexName)
{
    strcpy(indexName, fileName);
    char *name = strrchr(indexName, '.');
    if (name != NULL)
    {
        *name = '\0';
    }
    strcat(indexName, ".idx");
}

bool createTable(const char *tableName)
{
    FILE *f;
    int numero = -1;
    f = fopen(tableName, "rb");
    if (f != NULL)
    {
        fclose(f);
        return true;
    }

    f = fopen(tableName, "wb");
    if (f == NULL)
    {
        return false;
    }

    fwrite(&numero, sizeof(int), 1, f);

    fclose(f);
    return true;
}

bool createIndex(const char *indexName)
{
    FILE *f;
    int numero = -1;
    f = fopen(indexName, "rb");
    if (f != NULL)
    {
        fclose(f);
        return true;
    }

    f = fopen(indexName, "wb");
    if (f == NULL)
    {
        return false;
    }

    fwrite(&numero, sizeof(int), 1, f);
    fwrite(&numero, sizeof(int), 1, f);

    fclose(f);
    return true;
}

void printnode(size_t _level, size_t level,
               FILE *indexFileHandler, int node_id, char side)
{
    Node nodo;
    long posicion = INDEX_HEADER_SIZE + node_id * sizeof(Node);
    int i;

    fseek(indexFileHandler, posicion, SEEK_SET);
    fread(&nodo, sizeof(Node), 1, indexFileHandler);

    for (i = 0; i < _level; i++)
    {
        printf(" ");
    }

    printf("%c %.4s (%d): %d\n", side, nodo.book_id, node_id, nodo.offset);

    if (nodo.left != -1 && _level < level)
    {
        printnode(_level + 1, level, indexFileHandler, nodo.left, 'l');
    }
    if (nodo.right != -1 && _level < level)
    {
        printnode(_level + 1, level, indexFileHandler, nodo.right, 'r');
    }

    return;
}

void printTree(size_t level, const char *indexName)
{
    FILE *f;
    int raiz;

    f = fopen(indexName, "rb");
    if (f == NULL)
    {
        printf("Error en la funcion printTree\n");
        return;
    }
    fread(&raiz, sizeof(int), 1, f);

    if (raiz == -1)
    {
        printf("El árbol está vacío\n");
        fclose(f);
        return;
    }

    printnode(0, level, f, raiz, ' ');
    fclose(f);

    return;
}

bool findKey(const char *book_id, const char *indexName,
             int *nodeIDOrDataOffset)
{
    FILE *f;
    int root;
    int num_nodes;
    Node n;
    int current;

    f = fopen(indexName, "rb");
    if (f == NULL)
    {
        return false;
    }

    fread(&root, sizeof(int), 1, f);
    fread(&num_nodes, sizeof(int), 1, f);

    if (root == -1)
    {
        fclose(f);
        return false;
    }

    current = root;
    while (current != -1)
    {
        long pos = INDEX_HEADER_SIZE + current * sizeof(Node);
        fseek(f, pos, SEEK_SET);
        fread(&n, sizeof(Node), 1, f);
        int cmp = strcmp(book_id, n.book_id);
        if (cmp == 0) /*compara si coincide*/
        {
            *nodeIDOrDataOffset = n.offset;
            fclose(f);
            return true;
        }
        else if (cmp < 0)
        {
            current = n.left;
        }
        else
        {
            current = n.right;
        }
    }
    fclose(f);
    return false;
}

bool addIndexEntry(char *book_id, int bookOffset, char const *indexName)
{
    FILE *f = NULL;
    int root = -1;
    int num_nodes = 0;

    f = fopen(indexName, "r+b");
    if (f == NULL)
    {
        return false;
    }

    fread(&root, sizeof(int), 1, f);
    fread(&num_nodes, sizeof(int), 1, f);

    /* caso árbol vacío */
    if (root == -1)
    {
        Node n;
        memset(&n, 0, sizeof(Node));          /* limpia padding */
        strncpy(n.book_id, book_id, 4);
        n.book_id[4] = '\0';
        n.offset = bookOffset;
        n.left = n.right = -1;

        fseek(f, INDEX_HEADER_SIZE + 0 * sizeof(Node), SEEK_SET);
        fwrite(&n, sizeof(Node), 1, f);

        root = 0;
        num_nodes = 1;
        fseek(f, 0, SEEK_SET);
        fwrite(&root, sizeof(int), 1, f);
        fwrite(&num_nodes, sizeof(int), 1, f);
        fclose(f);
        return true;
    }

    int current = root;
    int parent = -1;
    int side = 0;
    Node n;

    while (current != -1)
    {
        long pos = INDEX_HEADER_SIZE + current * sizeof(Node);
        fseek(f, pos, SEEK_SET);
        fread(&n, sizeof(Node), 1, f);
        int cmp = strcmp(book_id, n.book_id);
        if (cmp == 0) /* clave duplicada */
        {
            fclose(f);
            return false;
        }
        parent = current;

        if (cmp < 0)
        {
            side = 0;
            current = n.left;
        }
        else
        {
            side = 1;
            current = n.right;
        }
    }

    Node nuevo;
    memset(&nuevo, 0, sizeof(Node));          /* limpia padding */
    strncpy(nuevo.book_id, book_id, 4);
    nuevo.book_id[4] = '\0';
    nuevo.offset = bookOffset;
    nuevo.left = -1;
    nuevo.right = -1;

    int new_id = num_nodes;
    long pos_new = INDEX_HEADER_SIZE + new_id * sizeof(Node);
    fseek(f, pos_new, SEEK_SET);
    fwrite(&nuevo, sizeof(Node), 1, f);

    long pos_parent = INDEX_HEADER_SIZE + parent * sizeof(Node);
    fseek(f, pos_parent, SEEK_SET);
    fread(&n, sizeof(Node), 1, f);

    if (side == 0)
        n.left = new_id;
    else
        n.right = new_id;

    fseek(f, pos_parent, SEEK_SET);
    fwrite(&n, sizeof(Node), 1, f);

    num_nodes++;
    fseek(f, sizeof(int), SEEK_SET);
    fwrite(&num_nodes, sizeof(int), 1, f);

    fclose(f);
    return true;
}

bool addTableEntry(Book * book, const char * tableName, const char * indexName)
{
    FILE *f = NULL;
    int data = 0;
    int offset = 0;
    int bookOffset = 0;
    int lenTitle = 0;

    if (book == NULL || tableName == NULL || indexName == NULL) {
        printf("ERROR: an argument is invalid\n");
        return false;
    }

    if (findKey(book->book_id, indexName, &offset) == true) {
        printf("The information has already been added\n");
        return false;
    }

    if (!(f = fopen(tableName, "r+b"))) {
        printf("ERROR: can't open the file\n");
        return false;
    }

    lenTitle = strlen(book->title);

    if (fread(&data, sizeof(int), 1, f) != 1) {
        if (fclose(f) != 0) {
            return false;
        }
        return false;
    }

    if (data == -1) { /* No hay borrados */
        if (fseek(f, 0, SEEK_END) != 0) {
            if (fclose(f) != 0) {
                return false;
            }
            return false;
        }
    }
    else { /* Hay borrados */
        int prevOffset = 0;
    }






}