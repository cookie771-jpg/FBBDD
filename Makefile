# --- COMPILADOR Y BANDERAS ---
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LDFLAGS = -lodbc

# --- CONFIGURACIÓN BASE DE DATOS (PostgreSQL) ---
export PGDATABASE := test01
export PGUSER := alumnodb
export PGPASSWORD := alumnodb
export PGCLIENTENCODING := LATIN9
export PGHOST := localhost

DBNAME = $(PGDATABASE)
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists

# --- ARCHIVOS A COMPILAR ---
# (He quitado products.o y orders.o porque no tienes esos archivos .c)
OBJS = main.o menu.o odbc.o customers.o pagination.o
TARGET = menu

# --- REGLA PRINCIPAL ---
# Al ejecutar 'make', primero crea la BBDD y luego compila el programa.
all: dropdb createdb restore $(TARGET)

# --- REGLAS DE BASE DE DATOS ---
dropdb:
	@echo "Eliminando BBDD..."
	@$(DROPDB) $(DBNAME) 2>/dev/null || true

createdb:
	@echo "Creando BBDD..."
	@$(CREATEDB)

restore:
	@echo "Restaurando datos..."
	@cat test01.sql | $(PSQL) 2>/dev/null || true

# --- REGLAS DE COMPILACIÓN ---
# Compilar el ejecutable final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compilar los archivos .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- REGLAS DE LIMPIEZA Y AYUDAS ---
# Limpieza de ejecutables y archivos temporales
clean:
	rm -f *.o $(TARGET) core

# Análisis de código con Splint
splint:
	splint -nullpass *.c *.h > splint.log