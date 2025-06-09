# Makefile para HTML Parser
# Compatible con C99/C11

# Configuración del compilador
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -g -Iincs
TARGET = html_parser
SRCDIR = src
OBJDIR = obj
BUILDDIR = build
INCDIR = incs
DOCSDIR = docs
SOURCES = main.c html_parser.c
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)
HEADERS = $(INCDIR)/html_parser.h

# Regla principal
all: dirs $(BUILDDIR)/$(TARGET)

# Crear directorios necesarios
dirs:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(DOCSDIR)

# Compilar el ejecutable
$(BUILDDIR)/$(TARGET): $(OBJECTS)
	@echo "Enlazando $(TARGET)..."
	$(CC) $(OBJECTS) -o $(BUILDDIR)/$(TARGET)
	@echo "✓ Compilación exitosa: $(BUILDDIR)/$(TARGET)"

# Compilar archivos objeto
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	@echo "Limpiando archivos temporales..."
	rm -rf $(OBJDIR) $(BUILDDIR)
	@echo "✓ Limpieza completada"

# Ejecutar pruebas básicas
test: $(BUILDDIR)/$(TARGET)
	@echo "Ejecutando pruebas básicas..."
	@if [ -f "$(DOCSDIR)/test.html" ]; then \
		echo "Probando con $(DOCSDIR)/test.html..."; \
		$(BUILDDIR)/$(TARGET) $(DOCSDIR)/test.html $(DOCSDIR)/test_output.txt; \
	else \
		echo "Creando archivo de prueba en $(DOCSDIR)/..."; \
		echo '<html><head><title>Prueba</title></head><body><h1>Hola Mundo</h1><p>Este es un <strong>test</strong>.</p></body></html>' > $(DOCSDIR)/test.html; \
		$(BUILDDIR)/$(TARGET) $(DOCSDIR)/test.html $(DOCSDIR)/test_output.txt; \
		echo "Resultado:"; \
		cat $(DOCSDIR)/test_output.txt; \
	fi

# Ejecutar el programa con archivo específico (busca en docs por defecto)
# Uso: make run FILE=archivo.html
run: $(BUILDDIR)/$(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "Error: Especifica un archivo HTML con FILE=archivo.html"; \
		echo "Ejemplo: make run FILE=mi_documento.html"; \
		echo "El archivo se buscará en la carpeta $(DOCSDIR)/"; \
		exit 1; \
	fi
	@INPUT_PATH="$(FILE)"; \
	if [ ! -f "$$INPUT_PATH" ] && [ -f "$(DOCSDIR)/$(FILE)" ]; then \
		INPUT_PATH="$(DOCSDIR)/$(FILE)"; \
	fi; \
	if [ ! -f "$$INPUT_PATH" ]; then \
		echo "Error: El archivo '$(FILE)' no existe ni en la ruta actual ni en $(DOCSDIR)/"; \
		exit 1; \
	fi; \
	echo "Procesando archivo: $$INPUT_PATH"; \
	$(BUILDDIR)/$(TARGET) "$$INPUT_PATH"

# Ejecutar y guardar resultado en archivo (salida en docs/)
# Uso: make run-save FILE=archivo.html OUTPUT=salida.txt
run-save: $(BUILDDIR)/$(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "Error: Especifica un archivo HTML con FILE=archivo.html"; \
		echo "Ejemplo: make run-save FILE=mi_documento.html OUTPUT=resultado.txt"; \
		echo "El archivo HTML se buscará en $(DOCSDIR)/ y el resultado se guardará en $(DOCSDIR)/"; \
		exit 1; \
	fi
	@INPUT_PATH="$(FILE)"; \
	if [ ! -f "$$INPUT_PATH" ] && [ -f "$(DOCSDIR)/$(FILE)" ]; then \
		INPUT_PATH="$(DOCSDIR)/$(FILE)"; \
	fi; \
	if [ ! -f "$$INPUT_PATH" ]; then \
		echo "Error: El archivo '$(FILE)' no existe ni en la ruta actual ni en $(DOCSDIR)/"; \
		exit 1; \
	fi; \
	OUTPUT_FILE="$${OUTPUT:-$$(basename $(FILE) .html)_clean.txt}"; \
	OUTPUT_PATH="$(DOCSDIR)/$$OUTPUT_FILE"; \
	echo "Procesando $$INPUT_PATH -> $$OUTPUT_PATH"; \
	$(BUILDDIR)/$(TARGET) "$$INPUT_PATH" "$$OUTPUT_PATH"; \
	echo "Resultado guardado en: $$OUTPUT_PATH"

# Procesar todos los archivos HTML en docs/
process-all: $(BUILDDIR)/$(TARGET)
	@echo "Procesando todos los archivos HTML en $(DOCSDIR)/..."
	@HTML_COUNT=$$(find $(DOCSDIR) -name "*.html" -type f | wc -l); \
	if [ $$HTML_COUNT -eq 0 ]; then \
		echo "No se encontraron archivos HTML en $(DOCSDIR)/"; \
		exit 1; \
	fi; \
	echo "Encontrados $$HTML_COUNT archivos HTML"; \
	for html_file in $(DOCSDIR)/*.html; do \
		if [ -f "$$html_file" ]; then \
			base_name=$$(basename "$$html_file" .html); \
			output_file="$(DOCSDIR)/$${base_name}_clean.txt"; \
			echo "Procesando: $$html_file -> $$output_file"; \
			$(BUILDDIR)/$(TARGET) "$$html_file" "$$output_file"; \
		fi; \
	done; \
	echo "✓ Procesamiento masivo completado"

# Listar archivos en docs/
list-docs:
	@echo "=== Contenido de $(DOCSDIR)/ ==="
	@if [ -d "$(DOCSDIR)" ]; then \
		echo "Archivos HTML:"; \
		find $(DOCSDIR) -name "*.html" -type f -exec basename {} \; 2>/dev/null | sort || echo "  (ninguno)"; \
		echo ""; \
		echo "Archivos TXT:"; \
		find $(DOCSDIR) -name "*.txt" -type f -exec basename {} \; 2>/dev/null | sort || echo "  (ninguno)"; \
		echo ""; \
		echo "Total de archivos: $$(find $(DOCSDIR) -type f | wc -l)"; \
	else \
		echo "La carpeta $(DOCSDIR) no existe"; \
	fi

# Ejecutar con ejemplo rápido (guarda en docs/)
demo: $(BUILDDIR)/$(TARGET)
	@echo "Creando archivo HTML de demostración en $(DOCSDIR)/..."
	@echo '<!DOCTYPE html>' > $(DOCSDIR)/demo.html
	@echo '<html lang="es">' >> $(DOCSDIR)/demo.html
	@echo '<head>' >> $(DOCSDIR)/demo.html
	@echo '    <meta charset="UTF-8">' >> $(DOCSDIR)/demo.html
	@echo '    <title>Página de Ejemplo</title>' >> $(DOCSDIR)/demo.html
	@echo '    <style>body { font-family: Arial; }</style>' >> $(DOCSDIR)/demo.html
	@echo '</head>' >> $(DOCSDIR)/demo.html
	@echo '<body>' >> $(DOCSDIR)/demo.html
	@echo '    <header>' >> $(DOCSDIR)/demo.html
	@echo '        <h1>Título Principal</h1>' >> $(DOCSDIR)/demo.html
	@echo '        <nav><a href="#seccion1">Sección 1</a> | <a href="#seccion2">Sección 2</a></nav>' >> $(DOCSDIR)/demo.html
	@echo '    </header>' >> $(DOCSDIR)/demo.html
	@echo '    <main>' >> $(DOCSDIR)/demo.html
	@echo '        <section id="seccion1">' >> $(DOCSDIR)/demo.html
	@echo '            <h2>Primera Sección</h2>' >> $(DOCSDIR)/demo.html
	@echo '            <p>Este es un párrafo con <strong>texto en negrita</strong> y <em>cursiva</em>.</p>' >> $(DOCSDIR)/demo.html
	@echo '            <ul>' >> $(DOCSDIR)/demo.html
	@echo '                <li>Elemento de lista 1</li>' >> $(DOCSDIR)/demo.html
	@echo '                <li>Elemento de lista 2</li>' >> $(DOCSDIR)/demo.html
	@echo '                <li>Elemento de lista 3</li>' >> $(DOCSDIR)/demo.html
	@echo '            </ul>' >> $(DOCSDIR)/demo.html
	@echo '        </section>' >> $(DOCSDIR)/demo.html
	@echo '        <section id="seccion2">' >> $(DOCSDIR)/demo.html
	@echo '            <h2>Segunda Sección</h2>' >> $(DOCSDIR)/demo.html
	@echo '            <blockquote>Esta es una cita importante con &quot;comillas&quot; y &amp; símbolos.</blockquote>' >> $(DOCSDIR)/demo.html
	@echo '            <p>Texto con <code>código inline</code> y <a href="https://example.com">enlace</a>.</p>' >> $(DOCSDIR)/demo.html
	@echo '        </section>' >> $(DOCSDIR)/demo.html
	@echo '    </main>' >> $(DOCSDIR)/demo.html
	@echo '    <footer>' >> $(DOCSDIR)/demo.html
	@echo '        <p>&copy; 2025 - Ejemplo de HTML Parser</p>' >> $(DOCSDIR)/demo.html
	@echo '    </footer>' >> $(DOCSDIR)/demo.html
	@echo '</body>' >> $(DOCSDIR)/demo.html
	@echo '</html>' >> $(DOCSDIR)/demo.html
	@echo "Ejecutando demostración..."
	@$(BUILDDIR)/$(TARGET) $(DOCSDIR)/demo.html $(DOCSDIR)/demo_resultado.txt
	@echo ""
	@echo "=== ARCHIVOS GENERADOS ==="
	@echo "- $(DOCSDIR)/demo.html (archivo HTML original)"
	@echo "- $(DOCSDIR)/demo_resultado.txt (texto extraído)"

# Limpiar solo archivos de docs/
clean-docs:
	@echo "Limpiando archivos generados en $(DOCSDIR)/ y archivos output..."
	@if [ -d "$(DOCSDIR)" ]; then \
		find $(DOCSDIR) -name "*.txt" -type f -delete 2>/dev/null; \
		find $(DOCSDIR) -name "*.html" -type f -delete 2>/dev/null; \
		echo "✓ Archivos temporales de $(DOCSDIR)/ eliminados"; \
	else \
		echo "La carpeta $(DOCSDIR) no existe"; \
	fi

# Mostrar ayuda
help:
	@echo "Makefile para HTML Parser"
	@echo ""
	@echo "Estructura del proyecto:"
	@echo "  $(SRCDIR)/          - Código fuente (.c)"
	@echo "  $(INCDIR)/          - Headers (.h)"
	@echo "  $(DOCSDIR)/         - Documentos HTML y archivos TXT generados"
	@echo "  $(OBJDIR)/          - Archivos objeto (generado)"
	@echo "  $(BUILDDIR)/        - Ejecutable (generado)"
	@echo ""
	@echo "Objetivos disponibles:"
	@echo "  all           - Compilar el programa (por defecto)"
	@echo "  clean         - Limpiar archivos objeto y ejecutable"
	@echo "  clean-docs    - Limpiar solo archivos generados en $(DOCSDIR)/"
	@echo "  distclean     - Limpieza completa (incluye archivos generados)"
	@echo "  test          - Ejecutar pruebas básicas"
	@echo "  demo          - Ejecutar demostración completa"
	@echo "  list-docs     - Listar archivos en $(DOCSDIR)/"
	@echo "  process-all   - Procesar todos los HTML en $(DOCSDIR)/"
	@echo ""
	@echo "Ejecución con archivos específicos:"
	@echo "  run           - Procesar un archivo HTML"
	@echo "  run-save      - Procesar y guardar resultado"
	@echo ""
	@echo "Ejemplos de uso:"
	@echo "  make run FILE=documento.html"
	@echo "  make run FILE=$(DOCSDIR)/pagina.html"
	@echo "  make run-save FILE=documento.html OUTPUT=resultado.txt"
	@echo "  make process-all"
	@echo "  make demo"
	@echo "  make list-docs"

# Compilación con debug
debug: CFLAGS += -DDEBUG -g3 -O0
debug: dirs $(BUILDDIR)/$(TARGET)
	@echo "✓ Versión debug compilada"

# Compilación release
release: CFLAGS += -DNDEBUG -O3 -s
release: dirs $(BUILDDIR)/$(TARGET)
	@echo "✓ Versión release compilada"

# Mostrar información del proyecto
info:
	@echo "Información del proyecto:"
	@echo "  Nombre: HTML Parser"
	@echo "  Compilador: $(CC)"
	@echo "  Flags: $(CFLAGS)"
	@echo "  Directorio fuente: $(SRCDIR)/"
	@echo "  Directorio headers: $(INCDIR)/"
	@echo "  Directorio documentos: $(DOCSDIR)/"
	@echo "  Archivos fuente: $(SOURCES)"
	@echo "  Archivos header: $(HEADERS)"
	@echo "  Ejecutable: $(BUILDDIR)/$(TARGET)"
	@echo ""
	@make list-docs

# Crear estructura de carpetas completa
setup:
	@echo "Creando estructura del proyecto..."
	@mkdir -p $(SRCDIR) $(INCDIR) $(OBJDIR) $(BUILDDIR) $(DOCSDIR)
	@echo "✓ Estructura de carpetas creada:"
	@echo "  $(SRCDIR)/     - para archivos .c"
	@echo "  $(INCDIR)/     - para archivos .h"
	@echo "  $(DOCSDIR)/    - para archivos HTML y TXT generados"
	@echo "  $(OBJDIR)/     - para archivos objeto (temporal)"
	@echo "  $(BUILDDIR)/   - para el ejecutable"

# Declarar objetivos que no son archivos
.PHONY: all clean distclean clean-docs install uninstall test help debug release info dirs run run-save demo list-docs process-all setup