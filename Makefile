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
       src/utils.c \
       src/indexer.c \
       src/persistence.c \
       src/index_operations.c

OBJS = $(patsubst src/%.c,$(OBJDIR)/%.o,$(SRCS))

all: setup $(TARGET)

setup:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)
	@mkdir -p indexes
	@mkdir -p backups

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INC_DIRS) -o $@ $(OBJS)

clean:
	rm -rf build obj

# Limpiar también archivos de índices y backups
clean-all: clean
	rm -rf indexes
	rm -rf backups

# ============================================================================
# COMANDOS DE BÚSQUEDA TRADICIONAL
# ============================================================================

run:
	@echo "Bienvenido al buscador de patrones."
	@echo "Para ejecutar el programa, usa uno de los siguientes comandos:"
	@echo ""
	@echo "=== BÚSQUEDA DE PATRONES ==="
	@echo "  make run-kmp PAT=\"abc\" FILE=texto.txt"
	@echo "  make run-bm PAT=\"palabra\" FILE=documento.html"
	@echo "  make run-shiftand PAT=\"patrón\" FILE=archivo.txt"
	@echo ""
	@echo "=== GESTIÓN DE ÍNDICES ==="
	@echo "  make create-index DIR=docs"
	@echo "  make search-index TERM=\"palabra\""
	@echo "  make index-info"
	@echo "  make export-index OUTPUT=indice.txt"
	@echo "  make backup-index"
	@echo ""
	@echo "(Los archivos .txt deben estar en el directorio docs/)"
	@echo "Programa desarrollado por Diego Galindo y Francisco Mercado."
	@echo "Para más información, use make help o consulte el README."

# Ejecución del programa con diferentes algoritmos
run-%: $(TARGET)
	@if [ -z "$(PAT)" ] || [ -z "$(FILE)" ]; then \
		echo "Uso: make run-ALGORITMO PAT=\"patrón\" FILE=archivo"; \
		exit 1; \
	fi
	@if [ -f "docs/$(FILE)" ]; then \
		./$(TARGET) $* "$(PAT)" "docs/$(FILE)"; \
	elif [ -f "$(FILE)" ]; then \
		./$(TARGET) $* "$(PAT)" "$(FILE)"; \
	else \
		echo "Error: Archivo '$(FILE)' no encontrado en el directorio actual ni en docs/"; \
		exit 1; \
	fi

# ============================================================================
# COMANDOS DE GESTIÓN DE ÍNDICES
# ============================================================================

# Crear índice desde un directorio
# Uso: make create-index DIR=docs [INDEX=nombre.idx]
create-index: $(TARGET)
	@if [ -z "$(DIR)" ]; then \
		echo "Uso: make create-index DIR=directorio [INDEX=archivo.idx]"; \
		echo "Ejemplo: make create-index DIR=docs INDEX=mi_indice.idx"; \
		exit 1; \
	fi
	@if [ ! -d "$(DIR)" ]; then \
		echo "Error: El directorio '$(DIR)' no existe"; \
		exit 1; \
	fi
	@INDEX_FILE=$${INDEX:-indexes/index.idx}; \
	echo "Creando índice desde $(DIR) -> $$INDEX_FILE"; \
	./$(TARGET) index create "$(DIR)" "$$INDEX_FILE"

# Buscar término en el índice
# Uso: make search-index TERM="palabra" [INDEX=archivo.idx]
search-index: $(TARGET)
	@if [ -z "$(TERM)" ]; then \
		echo "Uso: make search-index TERM=\"término\" [INDEX=archivo.idx]"; \
		echo "Ejemplo: make search-index TERM=\"función\""; \
		exit 1; \
	fi
	@INDEX_FILE=$${INDEX:-indexes/index.idx}; \
	if [ ! -f "$$INDEX_FILE" ]; then \
		echo "Error: Archivo de índice '$$INDEX_FILE' no encontrado"; \
		echo "Ejecuta primero: make create-index DIR=docs"; \
		exit 1; \
	fi; \
	./$(TARGET) index search "$$INDEX_FILE" "$(TERM)"

# Mostrar información del índice
# Uso: make index-info [INDEX=archivo.idx]
index-info: $(TARGET)
	@INDEX_FILE=$${INDEX:-indexes/index.idx}; \
	if [ ! -f "$$INDEX_FILE" ]; then \
		echo "Error: Archivo de índice '$$INDEX_FILE' no encontrado"; \
		echo "Ejecuta primero: make create-index DIR=docs"; \
		exit 1; \
	fi; \
	./$(TARGET) index info "$$INDEX_FILE"

# Exportar índice a texto
# Uso: make export-index OUTPUT=archivo.txt [INDEX=archivo.idx]
export-index: $(TARGET)
	@if [ -z "$(OUTPUT)" ]; then \
		echo "Uso: make export-index OUTPUT=archivo.txt [INDEX=archivo.idx]"; \
		echo "Ejemplo: make export-index OUTPUT=indice_exportado.txt"; \
		exit 1; \
	fi
	@INDEX_FILE=$${INDEX:-indexes/index.idx}; \
	if [ ! -f "$$INDEX_FILE" ]; then \
		echo "Error: Archivo de índice '$$INDEX_FILE' no encontrado"; \
		exit 1; \
	fi; \
	./$(TARGET) index export "$$INDEX_FILE" "$(OUTPUT)"

# Crear backup del índice
# Uso: make backup-index [INDEX=archivo.idx] [BACKUP_DIR=directorio]
backup-index: $(TARGET)
	@INDEX_FILE=$${INDEX:-indexes/index.idx}; \
	BACKUP_DIR=$${BACKUP_DIR:-backups}; \
	if [ ! -f "$$INDEX_FILE" ]; then \
		echo "Error: Archivo de índice '$$INDEX_FILE' no encontrado"; \
		exit 1; \
	fi; \
	./$(TARGET) index backup "$$INDEX_FILE" "$$BACKUP_DIR"

# ============================================================================
# COMANDOS DE CONVENIENCIA
# ============================================================================

# Crear índice de prueba con documentos de ejemplo
demo-index: $(TARGET)
	@echo "Creando índice de demostración..."
	@if [ ! -d "docs" ]; then \
		echo "Creando directorio docs/ con archivos de prueba..."; \
		mkdir -p docs; \
		echo "Este es un archivo de prueba con palabras clave como algoritmo y búsqueda." > docs/prueba1.txt; \
		echo "Otro documento que contiene términos sobre indexación y recuperación de información." > docs/prueba2.txt; \
		echo "Un tercer archivo con contenido sobre estructuras de datos y algoritmos de búsqueda." > docs/prueba3.txt; \
	fi
	@./$(TARGET) index create docs indexes/demo.idx
	@echo "Índice de demostración creado: indexes/demo.idx"
	@echo "Prueba con: make search-demo TERM=\"algoritmo\""

# Buscar en el índice de demostración
search-demo: $(TARGET)
	@if [ -z "$(TERM)" ]; then \
		echo "Uso: make search-demo TERM=\"término\""; \
		echo "Ejemplo: make search-demo TERM=\"algoritmo\""; \
		exit 1; \
	fi
	@if [ ! -f "indexes/demo.idx" ]; then \
		echo "Índice de demostración no encontrado. Ejecuta: make demo-index"; \
		exit 1; \
	fi
	@./$(TARGET) index search indexes/demo.idx "$(TERM)"

# Mostrar todos los índices disponibles
list-indexes:
	@echo "=== ÍNDICES DISPONIBLES ==="
	@if [ -d "indexes" ] && [ -n "$$(ls -A indexes 2>/dev/null)" ]; then \
		ls -la indexes/*.idx 2>/dev/null || echo "No hay archivos .idx en indexes/"; \
	else \
		echo "No hay índices creados aún."; \
		echo "Usa: make create-index DIR=docs"; \
	fi

# Mostrar backups disponibles
list-backups:
	@echo "=== BACKUPS DISPONIBLES ==="
	@if [ -d "backups" ] && [ -n "$$(ls -A backups 2>/dev/null)" ]; then \
		ls -la backups/; \
	else \
		echo "No hay backups creados aún."; \
	fi

# Ayuda completa
help:
	@echo "=== BUSCADOR DE PATRONES E INDEXADOR ==="
	@echo ""
	@echo "COMPILACIÓN:"
	@echo "  make              - Compilar el proyecto"
	@echo "  make clean        - Limpiar archivos compilados"
	@echo "  make clean-all    - Limpiar todo (incluye índices y backups)"
	@echo ""
	@echo "BÚSQUEDA DE PATRONES:"
	@echo "  make run-kmp PAT=\"patrón\" FILE=archivo.txt"
	@echo "  make run-bm PAT=\"patrón\" FILE=archivo.html"
	@echo "  make run-shiftand PAT=\"patrón\" FILE=archivo.txt"
	@echo ""
	@echo "GESTIÓN DE ÍNDICES:"
	@echo "  make create-index DIR=docs [INDEX=nombre.idx]"
	@echo "  make search-index TERM=\"palabra\" [INDEX=archivo.idx]"
	@echo "  make index-info [INDEX=archivo.idx]"
	@echo "  make export-index OUTPUT=salida.txt [INDEX=archivo.idx]"
	@echo "  make backup-index [INDEX=archivo.idx] [BACKUP_DIR=directorio]"
	@echo ""
	@echo "UTILIDADES:"
	@echo "  make demo-index   - Crear índice de demostración"
	@echo "  make search-demo TERM=\"palabra\" - Buscar en demo"
	@echo "  make list-indexes - Mostrar índices disponibles"
	@echo "  make list-backups - Mostrar backups disponibles"
	@echo "  make help         - Mostrar esta ayuda"

.PHONY: all clean clean-all setup run run-% create-index search-index index-info export-index backup-index demo-index search-demo list-indexes list-backups help