CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LDFLAGS = -lodbc

# --- PARTE 1: BASE DE DATOS ---
export PGDATABASE := test01
export PGUSER := alumnodb
export PGPASSWORD := alumnodb
export PGCLIENTENCODING:=LATIN9
export PGHOST := localhost

DBNAME = $(PGDATABASE)
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists

# --- PARTE 2: COMPILACIÓN ---
# Añade aquí TODOS los ficheros .c del proyecto
OBJS = main.o menu.o odbc.o customers.o pagination.o products.o orders.o
TARGET = menu

# El 'make all' hace las dos cosas: crea la BD y compila
all: dropdb createdb restore $(TARGET)

# Crear la base de datos
dropdb:
	@echo "Eliminando BBDD..."
	@$(DROPDB) $(DBNAME) 2>/dev/null || true

createdb:
	@echo "Creando BBDD..."
	@$(CREATEDB)

restore:
	@echo "Restaurando datos..."
	@cat test01.sql | $(PSQL) 2>/dev/null || true

# Compilar el programa
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -f *.o $(TARGET) core

# Splint
splint:
	splint -nullpass *.c *.h > splint.log