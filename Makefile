#Makefile # Compilador y flags
CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi
LDLIBS = -lodbc

# Variables de entorno para PostgreSQL
export PGDATABASE:=classicmodels
export PGUSER :=alumnodb
export PGPASSWORD :=alumnodb
export PGCLIENTENCODING:=LATIN9
export PGHOST:=localhost

# Herramientas de Base de Datos
DBNAME = $(PGDATABASE)
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists
PG_DUMP = pg_dump
PG_RESTORE = pg_restore

# Archivos de cabecera
HEADERS = odbc.h products.h customers.h pagination.h

# Ejecutables
ODBC_EXE = odbc-connection-test odbc-connection-test-2 odbc-example1 odbc-example2 odbc-example3 odbc-example4
MENU_EXE = menu

ALL_EXE = $(ODBC_EXE) $(MENU_EXE)

# -----------------------------------------------------------------
# Compilar
# -----------------------------------------------------------------

# Compila todo (ODBC + menu)
all: $(ALL_EXE)


compile: $(MENU_EXE)

# Compila únicamente los programas de ODBC
odbc: $(ODBC_EXE)

# Compila únicamente el programa menu
menu: $(MENU_EXE)

# Compilación de archivos objeto con dependencia de los headers
%.o: %.c $(HEADERS)
	@echo Compilando $<...
	$(CC) $(CFLAGS) -c -o $@ $<

# Vinculación de los ejecutables ODBC (cada uno con su .o y odbc.o)
$(ODBC_EXE): % : %.o odbc.o
	$(CC) -o $@ $^ $(LDLIBS)

# Vinculación del ejecutable menu (con todos vuestros archivos)
$(MENU_EXE): menu.o products.o customers.o pagination.o odbc.o
	$(CC) -o $@ $^ $(LDLIBS)

# -----------------------------------------------------------------
# Base de Datos
# -----------------------------------------------------------------

classic: dropdb createdb restore shell

createdb:
	@echo Creando BBDD
	@$(CREATEDB)

dropdb:
	@echo Eliminando BBDD
	@$(DROPDB) $(DBNAME)
	rm -f *.log

dump:
	@echo Creando dumpfile
	@$(PG_DUMP) > $(DBNAME).sql

restore:
	@echo Restaurando base de datos
	@cat $(DBNAME).sql | $(PSQL)

psql: shell

shell:
	@echo Creando psql shell...
	@$(PSQL)

# -----------------------------------------------------------------
# Ejecutar
# -----------------------------------------------------------------
run:
	./menu

# -----------------------------------------------------------------
# Limpieza
# -----------------------------------------------------------------

# Limpia todo
clean: clean-odbc clean-menu
	rm -f *.o core *.log

# Limpia solo los ejecutables y objetos de ODBC
clean-odbc:
	rm -f $(ODBC_EXE) $(addsuffix .o,$(ODBC_EXE)) odbc.o

# Limpia solo el ejecutable y objeto del menú
clean-menu:
	rm -f $(MENU_EXE) menu.o products.o customers.o pagination.o

.PHONY: all compile odbc menu clean clean-odbc clean-menu test01 createdb dropdb dump restore psql shell