CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra
OBJDIR = obj
BINDIR = build
TARGET = $(BINDIR)/buscador
INC_DIRS = -Iincs

SRCS = src/main.c \
       src/KMP.c \
       src/boyer_moore.c \
       src/shift_and.c \
       src/utils.c

OBJS = $(patsubst src/%.c,$(OBJDIR)/%.o,$(SRCS))

all: setup $(TARGET)

setup:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INC_DIRS) -o $@ $(OBJS)

clean:
	rm -rf build obj

run:
	@echo "Bienvenido al buscador de patrones."
	@echo "Para ejecutar el programa, usa uno de los siguientes comandos:"
	@echo "  make run-kmp PAT=\"abc\" FILE=texto.txt"
	@echo ""
	@echo "Programa desarrollado por Diego Galindo y Francisco Mercado."
	@echo "Para más información, consulta el README.md."

# Ejecución del programa con diferentes algoritmos
# Uso: make run-ALGORITMO PAT="patrón" FILE=archivo [FLAGS=opciones]
# Donde ALGORITMO puede ser kmp, bm, sa, o todas
run-%: $(TARGET)
	@if [ -z "$(PAT)" ] || [ -z "$(FILE)" ]; then \
		echo "Uso: make run-ALGORITMO PAT=\"patrón\" FILE=archivo [FLAGS=opciones]"; \
		exit 1; \
	fi
	./$(TARGET) $* "$(PAT)" "$(FILE)" $(FLAGS)

.PHONY: all clean setup run-%