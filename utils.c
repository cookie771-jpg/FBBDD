#include "utils.h"

int no_deleted_registers = NO_DELETED_REGISTERS;

void replaceExtensionByIdx(const char *fileName, char * indexName) {
    strcpy(indexName,fileName);
    char* name= strrchar(indexName,'.');
    if (name!=NULL)
    {
        *name='\0';
    }
    strcat(indexName,".");
    
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
    
    Node nodo;
    long posicion = INDEX_HEADER_SIZE + node_id * sizeof(Node);
    int i;

    fseek(indexFileHandler,posicion,SEEK_SET);
    fread(&nodo,sizeof(Node),1,indexFileHandler);

    for ( i = 0; i < _level; i++)
    {
        printf("");
    }
    
    printf("%c %.4s (%d): %d\n",side, nodo.book_id, node_id ,nodo.offset);

    if (nodo.left !=-1 && _level < level)
    {
        printnode(_level+1,level,indexFileHandler,nodo.left,'l');
    }
    if ( nodo.right !=-1 && _level<level)
    {
        printnode(_level+1,level,indexFileHandler,nodo.right,'r');
    }

    return;
}

void printTree(size_t level, const char * indexName)
{

    FILE*f;
    int raiz;

    f=fopen(indexName,"rb");
    if(f==NULL){
        printf("Error en la funcion printTree\n");
        return;
    }
    fread(&raiz,sizeof(int),1,f);

    if (raiz ==-1){
    printf("El árbol está vacío\n");
    fclose(f);
    return;
    }

    printnode(0,level,f,raiz,' ');
    fclose(f);

    return;
}

bool findKey(const char * book_id, const char *indexName,
             int * nodeIDOrDataOffset)
 {
    
    FILE *f;
    int root;
    int num_nodes;
    Node n;
    int current;

    f=fopen(indexName,"rb");
    if (f==NULL)
    {
        return false;
    }

    fread(&root,sizeof(int),1,f);
    fread(&num_nodes,sizeof(int),1,f);

    if()













 }

bool addIndexEntry(char * book_id,  int bookOffset, char const * indexName) {
    return true;
}

bool addTableEntry(Book * book, const char * dataName,
                   const char * indexName) {
    return true;
}