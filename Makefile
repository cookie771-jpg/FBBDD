CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic
LDLIBS = -lodbc

export PGDATABASE:=classicmodels
export PGUSER :=alumnodb
export PGPASSWORD :=alumnodb
export PGCLIENTENCODING:=LATIN9
export PGHOST:=localhost

DBNAME = $(PGDATABASE)
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists
PG_DUMP = pg_dump
PG_RESTORE = pg_restore

HEADERS = odbc.h products.h orders.h customers.h pagination.h

MENU_EXE = menu
MENU_OBJ = menu.o products.o orders.o customers.o pagination.o odbc.o

ALL_EXE = $(ODBC_EXE) $(MENU_EXE)

all: classic compile

compile: $(MENU_EXE)

%.o: %.c $(HEADERS)
	@echo Compilando $<...
	$(CC) $(CFLAGS) -c -o $@ $<

$(MENU_EXE): $(MENU_OBJ)
	$(CC) -o $@ $^ $(LDLIBS)

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

run:
	./menu

vrun:
	valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --errors-for-leak-kinds=all \
         --error-exitcode=1 \
         ./menu
splint:
	splint -nullpass *.c *.h > splint.log 2>&1

bash:
	./products_find.sh
	./products_stock.sh
	./orders_open.sh
	./orders_range.sh
	./orders_detail.sh
	./customers_find.sh
	./customers_balance.sh
	FBD_PAGE_SIZE=1000 ./customers_list_products.sh

clean: clean-odbc clean-menu
	rm -f *.o core

clean-odbc:
	rm -f $(ODBC_EXE) $(addsuffix .o,$(ODBC_EXE)) odbc.o

clean-menu:
	rm -f $(MENU_EXE) menu.o products.o orders.o customers.o pagination.o

.PHONY: all compile odbc clean clean-odbc clean-menu classic createdb dropdb dump restore psql shell run vrun splint bash