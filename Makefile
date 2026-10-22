#Makefile Practica 2 -Ziqi Zhoulin y José Miguel Romero Oubiña
# Compilador y flags
CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic -ansi
LDLIBS = -lodbc

# Variables de entorno para PostgreSQL
export PGDATABASE    := test01
export PGUSER        := alumnodb
export PGPASSWORD    := alumnodb
export PGCLIENTENCODING := LATIN9
export PGHOST        := localhost

# Herramientas de Base de Datos
DBNAME     = $(PGDATABASE)
PSQL       = psql
CREATEDB   = createdb
DROPDB     = dropdb --if-exists
PG_DUMP    = pg_dump
PG_RESTORE = pg_restore

# Archivos de cabecera
HEADERS = odbc.h

# Ejecutables
ODBC_EXE = odbc-connection-test odbc-connection-test-2 odbc-example1 odbc-example2 odbc-example3 odbc-example4
MENU_EXE = menu

ALL_EXE = $(ODBC_EXE) $(MENU_EXE)

# -----------------------------------------------------------------
# Reglas de Compilación
# -----------------------------------------------------------------

# Compila todo (ODBC + menu)
all: $(ALL_EXE)

# Compila únicamente los programas de ODBC
odbc: $(ODBC_EXE)

# Compila únicamente el programa menu
menu: $(MENU_EXE)

# Compilación de archivos objeto con dependencia del header odbc.h
%.o: %.c $(HEADERS)
	@echo Compilando $<...
	$(CC) $(CFLAGS) -c -o $@ $<

# Vinculación de los ejecutables ODBC (cada uno con su .o y odbc.o)
$(ODBC_EXE): % : %.o odbc.o
	$(CC) -o $@ $^ $(LDLIBS)

# Vinculación del ejecutable menu (con menu.o y odbc.o)
$(MENU_EXE): menu.o odbc.o
	$(CC) -o $@ $^ $(LDLIBS)

# -----------------------------------------------------------------
# Reglas de Base de Datos
# -----------------------------------------------------------------

test01: dropdb createdb restore shell

createdb:
	@echo Creando BBDD...
	@$(CREATEDB)

dropdb:
	@echo Eliminando BBDD...
	@$(DROPDB) $(DBNAME)
	rm -f *.log

dump:
	@echo Creando dumpfile...
	@$(PG_DUMP) > $(DBNAME).sql

restore:
	@echo Restaurando base de datos...
	@cat $(DBNAME).sql | $(PSQL)

psql: shell

shell:
	@echo Creando psql shell...
	@$(PSQL)

# -----------------------------------------------------------------
# Reglas de Limpieza
# -----------------------------------------------------------------

# Limpia todo
clean: clean-odbc clean-menu
	rm -f *.o core *.log

# Limpia solo los ejecutables y objetos de ODBC
clean-odbc:
	rm -f $(ODBC_EXE) $(addsuffix .o,$(ODBC_EXE)) odbc.o

# Limpia solo el ejecutable y objeto del menú
clean-menu:
	rm -f $(MENU_EXE) menu.o

.PHONY: all odbc menu clean clean-odbc clean-menu test01 createdb dropdb dump restore psql shell