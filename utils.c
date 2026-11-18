#include "utils.h"

int no_deleted_registers = NO_DELETED_REGISTERS;

void replaceExtensionByIdx(const char *fileName, char * indexName) {
    return;
}


bool createTable(const char * tableName) {
    FILE *f;
    int numero=-1;
    f=fopen(tableName,"rb");
    if (f!=NULL)
    {
        fclose(f);
        return true;
    }

    f=fopen(tableName,"wb");
    if (f==NULL)
    {
        return false;
    }

    fwrite(&numero,sizeof(int),1,f);

    fclose(f);
    return true;

 }

bool createIndex(const char *indexName) {
    FILE *f;
    int numero=-1;
    f=fopen(indexName,"rb");
    if (f!=NULL)
    {
        fclose(f);
        return true;
    }
    
    f=fopen(indexName,"wb");
    if (f==NULL)
    {
        return false;
    }

    fwrite(&numero,sizeof(int),1,f);
    fwrite(&numero,sizeof(int),1,f);

    fclose(f);
    return true;

}
void printnode(size_t _level, size_t level,
               FILE * indexFileHandler, int node_id, char side) {


















    return;
}

void printTree(size_t level, const char * indexName)
{

    FILE*f;
    int raiz;

    f=fopen(indexName,"rb");
    if(f==NULL){
        printf("Error en la funcion prinTree\n");
        return false;
    }
    fread(&raiz,)













    return;
}

bool findKey(const char * book_id, const char *indexName,
             int * nodeIDOrDataOffset)
 {
     return true;
 }

bool addIndexEntry(char * book_id,  int bookOffset, char const * indexName) {
    return true;
}

bool addTableEntry(Book * book, const char * dataName,
                   const char * indexName) {
    return true;
}