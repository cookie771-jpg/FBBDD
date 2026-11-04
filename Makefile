# Makefile Práctica 2 de Ziqi Zhoulin y José Miguel Romero Oubiña
# Compilador y flags
CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic
LDLIBS = -lodbc

# Variables de entorno para PostgreSQL
export PGDATABASE :=classicmodels
export PGUSER :=alumnodb
export PGPASSWORD :=alumnodb
export PGCLIENTENCODING :=LATIN9
export PGHOST :=localhost

# Herramientas de Base de Datos
DBNAME = $(PGDATABASE)
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists
PG_DUMP = pg_dump
PG_RESTORE = pg_restore

# Includes
HEADERS = odbc.h products.h orders.h customers.h pagination.h

# Ejecutables
MENU_EXE = menu
MENU_OBJ = menu.o products.o orders.o customers.o pagination.o odbc.o

# --------
# Compilar y Generar BD
# --------

# Base de Datos + Menu
all: classic compile

# Menu
compile: $(MENU_EXE)

%.o: %.c $(HEADERS)
	@echo Compilando $<...
	$(CC) $(CFLAGS) -c -o $@ $<

$(MENU_EXE): $(MENU_OBJ)
	$(CC) -o $@ $^ $(LDLIBS)

# Base de Datos Classicmodels
classic: dropdb createdb restore

createdb:
	@echo Creando BBDD
	@$(CREATEDB)

dropdb:
	@echo Eliminando BBDD
	@$(DROPDB) $(DBNAME)

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

# --------
# Ejecutar
# --------

# Ejecución normal
run:
	./menu

# Ejecución con Valgrind
vrun:
	valgrind --leak-check=full -s ./menu

# Ejecución de bash
bash:
	./products_find.sh
	./products_stock.sh
	./orders_open.sh
	./orders_range.sh
	./orders_detail.sh
	./customers_find.sh
	./customers_balance.sh
	FBD_PAGE_SIZE=1000 ./customers_list_products.sh

# --------
# Splint
# --------
splint:
	splint -nullpass *.c *.h > splint.log 2>&1

# --------
# Limpieza
# --------
clean:
	rm -f $(MENU_EXE) *.log *.o core
	
.PHONY: all compile classic createdb dropdb dump restore psql shell run vrun bash splint clean