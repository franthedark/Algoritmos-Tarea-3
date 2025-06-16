# Diego Galindo y Francisco Mercado

CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra
OBJDIR = obj
BINDIR = build
TARGET = $(BINDIR)/buscador
INC_DIRS = -Iincs
INDEX_DIR = indexes
DOCS_DIR = docs


SRCS = src/main.c \
       src/KMP.c \
       src/boyer_moore.c \
       src/shift_and.c \
       src/utils.c \
       src/indexer.c \
       src/persistence.c \
       src/index_operations.c \
	   src/cli.c \
	   src/normalization.c \
	   src/similarity.c

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
	$(CC) $(CFLAGS) $(INC_DIRS) -o $@ $(OBJS) -lm

clean:
	rm -rf build obj indexes backups results

# Limpiar también archivos de índices y backups
clean-all: clean clean-corpus
	rm -rf indexes backups results

# ============================================================================
# COMANDOS DE BÚSQUEDA TRADICIONAL
# ============================================================================

run:
	@echo "Bienvenido al buscador de patrones."
	@echo "Programa desarrollado por Diego Galindo y Francisco Mercado."
	@echo "Para ejecutar el programa, use uno de los siguientes comandos:"
	@echo ""
	@echo "=== BÚSQUEDA DE PATRONES ==="
	@echo "  make run-kmp PAT=\"abc\" FILE=texto.txt [OPTS=opciones]"
	@echo "  make run-bm PAT=\"palabra\" FILE=documento.html [OPTS=opciones]"
	@echo "  make run-shiftand PAT=\"patrón\" FILE=archivo.txt [OPTS=opciones]"
	@echo ""
	@echo "El uso de OPTS es opcional y puede ser:"
	@echo "  OPTS=basic         - Normalización básica (defecto)"
	@echo "  OPTS=no-diacritics - Eliminar acentos y tildes"
	@echo "  OPTS=nfc          - Normalización Unicode NFC"
	@echo "  OPTS=nfd          - Normalización Unicode NFD"
	@echo ""
	@echo "(Los archivos de texto deben estar en el directorio docs/. Si no está en ese directorio, especifique la ruta completa.)"
	@echo "Para más información, use make help o consulte el README."

# Ejecución del programa con diferentes algoritmos
run-%: $(TARGET)
	@if [ -z "$(PAT)" ] || [ -z "$(FILE)" ]; then \
		echo "Uso: make run-ALGORITMO PAT=\"patrón\" FILE=archivo OPTS=ajuste"; \
		exit 1; \
	fi
	@if [ -f "docs/$(FILE)" ]; then \
		./$(TARGET) $* "$(PAT)" "docs/$(FILE)" $(OPTS); \
	elif [ -f "$(FILE)" ]; then \
		./$(TARGET) $* "$(PAT)" "$(FILE)" $(OPTS); \
	else \
		echo "Error: Archivo '$(FILE)' no encontrado en el directorio actual ni en docs/"; \
		exit 1; \
	fi

# ============================================================================
# COMANDOS DE GESTIÓN DE ÍNDICES
# ============================================================================

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
search-index: $(TARGET)
	@if [ -z "$(TERM)" ]; then \
		echo "Uso: make search-index TERM=\"término\" [INDEX=archivo.idx]"; \
		echo "Ejemplo: make search-index TERM=\"función\""; \
		exit 1; \
	fi
	@if [ -z "$(INDEX)" ]; then \
		RESOLVED_INDEX="indexes/index.idx"; \
	elif [ -f "$(INDEX)" ]; then \
		RESOLVED_INDEX="$(INDEX)"; \
	elif [ -f "indexes/$(INDEX)" ]; then \
		RESOLVED_INDEX="indexes/$(INDEX)"; \
	else \
		RESOLVED_INDEX="$(INDEX)"; \
	fi; \
	if [ ! -f "$$RESOLVED_INDEX" ]; then \
		echo "Error: Archivo de índice '$$RESOLVED_INDEX' no encontrado"; \
		echo "Ejecuta primero: make create-index DIR=docs"; \
		exit 1; \
	fi; \
	./$(TARGET) index search "$$RESOLVED_INDEX" "$(TERM)"

# Mostrar información del índice
index-info: $(TARGET)
	@if [ -z "$(INDEX)" ]; then \
		RESOLVED_INDEX="indexes/index.idx"; \
	elif [ -f "$(INDEX)" ]; then \
		RESOLVED_INDEX="$(INDEX)"; \
	elif [ -f "indexes/$(INDEX)" ]; then \
		RESOLVED_INDEX="indexes/$(INDEX)"; \
	else \
		RESOLVED_INDEX="$(INDEX)"; \
	fi; \
	if [ ! -f "$$RESOLVED_INDEX" ]; then \
		echo "Error: Archivo de índice '$$RESOLVED_INDEX' no encontrado"; \
		echo "Ejecuta primero: make create-index DIR=docs"; \
		exit 1; \
	fi; \
	./$(TARGET) index info "$$RESOLVED_INDEX"

# Exportar índice a texto
export-index: $(TARGET)
	@if [ -z "$(OUTPUT)" ]; then \
		echo "Uso: make export-index OUTPUT=archivo.txt [INDEX=archivo.idx]"; \
		echo "Ejemplo: make export-index OUTPUT=indice_exportado.txt"; \
		exit 1; \
	fi
	@if [ -z "$(INDEX)" ]; then \
		RESOLVED_INDEX="indexes/index.idx"; \
	elif [ -f "$(INDEX)" ]; then \
		RESOLVED_INDEX="$(INDEX)"; \
	elif [ -f "indexes/$(INDEX)" ]; then \
		RESOLVED_INDEX="indexes/$(INDEX)"; \
	else \
		RESOLVED_INDEX="$(INDEX)"; \
	fi; \
	if [ ! -f "$$RESOLVED_INDEX" ]; then \
		echo "Error: Archivo de índice '$$RESOLVED_INDEX' no encontrado"; \
		exit 1; \
	fi; \
	./$(TARGET) index export "$$RESOLVED_INDEX" "$(OUTPUT)"

# Crear backup del índice
backup-index: $(TARGET)
	@if [ -z "$(INDEX)" ]; then \
		RESOLVED_INDEX="indexes/index.idx"; \
	elif [ -f "$(INDEX)" ]; then \
		RESOLVED_INDEX="$(INDEX)"; \
	elif [ -f "indexes/$(INDEX)" ]; then \
		RESOLVED_INDEX="indexes/$(INDEX)"; \
	else \
		RESOLVED_INDEX="$(INDEX)"; \
	fi; \
	BACKUP_DIR=$${BACKUP_DIR:-backups}; \
	if [ ! -f "$$RESOLVED_INDEX" ]; then \
		echo "Error: Archivo de índice '$$RESOLVED_INDEX' no encontrado"; \
		exit 1; \
	fi; \
	./$(TARGET) index backup "$$RESOLVED_INDEX" "$$BACKUP_DIR"

# Actualizar índice agregando nuevos documentos
update-index: $(TARGET)
	@if [ -z "$(NEW_DOCS)" ]; then \
		echo "Uso: make update-index NEW_DOCS=doc_o_dir INDEX=archivo.idx"; \
		echo "Ejemplo 1 (archivo): make update-index NEW_DOCS=nuevo.txt INDEX=index.idx"; \
		echo "Ejemplo 2 (directorio): make update-index NEW_DOCS=nuevos_docs/ INDEX=index.idx"; \
		exit 1; \
	fi; \
	# Resolver ruta de documentos; \
	if [ -d "$(NEW_DOCS)" ] || [ -f "$(NEW_DOCS)" ]; then \
		RESOLVED_NEW_DOCS="$(NEW_DOCS)"; \
	elif [ -d "docs/$(NEW_DOCS)" ] || [ -f "docs/$(NEW_DOCS)" ]; then \
		RESOLVED_NEW_DOCS="docs/$(NEW_DOCS)"; \
	else \
		echo "Error: No se encontró '$(NEW_DOCS)' ni en el directorio actual ni en docs/"; \
		exit 1; \
	fi; \
	# Resolver ruta del índice; \
	if [ -z "$(INDEX)" ]; then \
		RESOLVED_INDEX="indexes/index.idx"; \
	elif [ -f "$(INDEX)" ]; then \
		RESOLVED_INDEX="$(INDEX)"; \
	elif [ -f "indexes/$(INDEX)" ]; then \
		RESOLVED_INDEX="indexes/$(INDEX)"; \
	else \
		RESOLVED_INDEX="$(INDEX)"; \
	fi; \
	./$(TARGET) index update "$$RESOLVED_INDEX" "$$RESOLVED_NEW_DOCS"

# ============================================================================
# COMANDOS DE ANÁLISIS DE SIMILITUD
# ============================================================================

# Comparar dos archivos directamente
similarity: $(TARGET)
	@if [ -z "$(FILE1)" ] || [ -z "$(FILE2)" ]; then \
		echo "Uso: make similarity FILE1=archivo1 FILE2=archivo2"; \
		exit 1; \
	fi
	@# Buscar archivos en docs/ si no existen en ruta actual
	@FILE1_PATH=; \
	if [ -f "$(FILE1)" ]; then \
		FILE1_PATH="$(FILE1)"; \
	elif [ -f "docs/$(FILE1)" ]; then \
		FILE1_PATH="docs/$(FILE1)"; \
	else \
		echo "Error: Archivo '$(FILE1)' no encontrado en directorio actual ni en docs/"; \
		exit 1; \
	fi; \
	FILE2_PATH=; \
	if [ -f "$(FILE2)" ]; then \
		FILE2_PATH="$(FILE2)"; \
	elif [ -f "docs/$(FILE2)" ]; then \
		FILE2_PATH="docs/$(FILE2)"; \
	else \
		echo "Error: Archivo '$(FILE2)' no encontrado en directorio actual ni en docs/"; \
		exit 1; \
	fi; \
	./$(TARGET) similarity "$$FILE1_PATH" "$$FILE2_PATH"

# Comparar dos documentos en un índice (por ID)
index-similarity: $(TARGET)
	@if [ -z "$(INDEX)" ] || [ -z "$(DOC1)" ] || [ -z "$(DOC2)" ]; then \
		echo "Uso: make index-similarity INDEX=index.idx DOC1=id1 DOC2=id2"; \
		exit 1; \
	fi
	@./$(TARGET) index similarity "$(INDEX)" "$(DOC1)" "$(DOC2)"

# Encontrar documentos similares en el índice
index-similarity-indexed: $(TARGET)
	@if [ -z "$(INDEX)" ] || [ -z "$(DOC)" ]; then \
		echo "Uso: make index-similarity-indexed INDEX=index.idx DOC=id TOP_K=5"; \
		exit 1; \
	fi
	@TOP_K=$${TOP_K:-5}; \
	./$(TARGET) index similarity-indexed "$(INDEX)" "$(DOC)" "$$TOP_K"

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

# ============================================================================
# COMANDOS DE HERRAMIENTAS Y UTILIDADES
# ============================================================================


# Benchmark automático
.PHONY: benchmark
benchmark:
	@./tools/benchmark.sh

graph:
	@python3 tools/graficar_benchmark.py

fetch-corpus:
	@echo "Descargando corpus de prueba..."
	@bash tools/fetch_corpus/fetch_corpus.sh

FILES_TO_CLEAN = docs/corpus/alice.txt docs/corpus/sherlock.txt docs/corpus/sms_spam.csv docs/corpus/wikipedia-sample.txt docs/corpus/README.md tools/strip_html/strip_html

clean-corpus:
	@if ls $(FILES_TO_CLEAN) >/dev/null 2>&1; then \
		rm -f $(FILES_TO_CLEAN); \
		echo "Corpus limpiado."; \
	else \
		: ; \
	fi

help:
	@echo "=== BUSCADOR DE PATRONES E INDEXADOR ==="
	@echo ""
	@echo "COMPILACIÓN:"
	@echo "  make              - Compilar el proyecto"
	@echo "  make clean        - Limpiar archivos compilados"
	@echo "  make clean-all    - Limpiar todo (incluye índices y backups)"
	@echo ""
	@echo "BÚSQUEDA DE PATRONES:"
	@echo "  make run-kmp PAT=\"patrón\" FILE=archivo.txt OPTS=opciones"
	@echo "  make run-bm PAT=\"patrón\" FILE=archivo.html OPTS=opciones"
	@echo "  make run-shiftand PAT=\"patrón\" FILE=archivo.csv OPTS=opciones"
	@echo ""
	@echo "OPCIONES DE NORMALIZACIÓN:"
	@echo "  OPTS=basic        - Normalización básica (defecto)"
	@echo "  OPTS=no-diacritics - Eliminar acentos y tildes"
	@echo "  OPTS=nfc           - Normalización Unicode NFC (Composición)"
	@echo "  OPTS=nfd          - Normalización Unicode NFD (Descomposición)"
	@echo ""
	@echo "EJEMPLOS:"
	@echo "  make run-bm PAT=\"café\" FILE=texto.txt OPTS=nfc"
	@echo "  make run-kmp PAT=\"Además\" FILE=doc.html OPTS=no-diacritics"
	@echo ""
	@echo "GESTIÓN DE ÍNDICES:"
	@echo "  make create-index DIR=docs INDEX=nombre.idx"
	@echo "  make search-index TERM=\"palabra\" INDEX=archivo.idx"
	@echo "  make index-info INDEX=archivo.idx"
	@echo "  make export-index OUTPUT=salida.txt INDEX=archivo.idx"
	@echo "  make backup-index INDEX=archivo.idx"
	@echo "  make update-index NEW_DOCS=doc_o_dir INDEX=archivo.idx"
	@echo ""
	@echo "ANÁLISIS DE SIMILITUD:"
	@echo "  make similarity FILE1=archivo1 FILE2=archivo2"
	@echo "  make index-similarity INDEX=index.idx DOC1=id1 DOC2=id2"
	@echo "  make index-similarity-indexed INDEX=index.idx DOC=id TOP_K=5"
	@echo ""
	@echo "EJEMPLOS:"
	@echo "  make similarity FILE1=docs/alice.txt FILE2=docs/wonderland.txt"
	@echo "  make index-similarity INDEX=index.idx DOC1=1 DOC2=5"
	@echo "  make index-similarity-indexed INDEX=index.idx DOC=3 TOP_K=8"
	@echo ""
	@echo "UTILIDADES:"
	@echo "  make demo-index   - Crear índice de demostración"
	@echo "  make search-demo TERM=\"palabra\" - Buscar en demo"
	@echo "  make list-indexes - Mostrar índices disponibles"
	@echo "  make list-backups - Mostrar backups disponibles"
	@echo "  make help         - Mostrar esta ayuda"
	@echo ""
	@echo "HERRAMIENTAS ADICIONALES:"
	@echo "  make fetch-corpus - Descargar corpus de prueba"
	@echo "  make clean-corpus - Limpiar corpus descargado"
	@echo "  make benchmark    - Ejecutar benchmark automático"
	@echo "  make graph        - Graficar resultados del benchmark"
	@echo ""

.PHONY: all clean clean-all setup run run-% create-index search-index index-info export-index backup-index demo-index search-demo list-indexes list-backups help fetch-corpus clean-corpus benchmark graph update-index index-similarity index-similarity-indexed similarity