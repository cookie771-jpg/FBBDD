# Makefile Practica 2
CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi
LDLIBS = -lodbc

# Variables de entorno para PostgreSQL
export PGDATABASE := classicmodels
export PGUSER := alumnodb
export PGPASSWORD := alumnodb
export PGCLIENTENCODING := LATIN9
export PGHOST := localhost

DBNAME = $(PGDATABASE)
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists

HEADERS = odbc.h customers.h pagination.h

ODBC_EXE = odbc-connection-test odbc-connection-test-2 odbc-example1 odbc-example2 odbc-example3 odbc-example4
MENU_EXE = menu
# Cuando A termine, anadir aqui products.o orders.o
MENU_OBJ = menu.o customers.o pagination.o odbc.o

# -----------------------------------------------------------------
# Compilar
# -----------------------------------------------------------------
compile: $(MENU_EXE)

odbc: $(ODBC_EXE)

%.o: %.c $(HEADERS)
	@echo Compilando $<...
	$(CC) $(CFLAGS) -c -o $@ $<

$(ODBC_EXE): % : %.o odbc.o
	$(CC) -o $@ $^ $(LDLIBS)

$(MENU_EXE): $(MENU_OBJ)
	$(CC) -o $@ $^ $(LDLIBS)

# -----------------------------------------------------------------
# Base de datos: make all = borrar, crear y poblar
# -----------------------------------------------------------------
all: dropdb createdb restore

createdb:
	@echo Creando BBDD
	@$(CREATEDB)

dropdb:
	@echo Eliminando BBDD
	@$(DROPDB) $(DBNAME)

restore:
	@echo Restaurando base de datos
	@cat $(DBNAME).sql | $(PSQL) -q

shell:
	@$(PSQL)

# -----------------------------------------------------------------
# Limpieza (no borra splint.log)
# -----------------------------------------------------------------
clean:
	rm -f *.o core $(ODBC_EXE) $(MENU_EXE)

.PHONY: all compile odbc clean createdb dropdb restore shell