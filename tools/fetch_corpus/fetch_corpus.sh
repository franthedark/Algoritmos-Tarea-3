#!/bin/bash

# Configuración
BASE_DIR=$(dirname "$0")/../..
CORPUS_DIR="$BASE_DIR/docs/corpus"
TOOLS_DIR="$BASE_DIR/tools"
STRIP_HTML_BIN="$TOOLS_DIR/strip_html/strip_html"

# Crear directorios necesarios
mkdir -p "$CORPUS_DIR" "$TOOLS_DIR/strip_html"

# Compilar herramienta strip_html
echo "Compilando strip_html..."
cat > "$TOOLS_DIR/strip_html/strip_html.c" << 'EOL'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void decodeEntity(const char* start, char** out) {
    // Implementación simplificada para entidades comunes
    if (strncmp(start, "amp;", 4) == 0) {
        *(*out)++ = '&';
    } else if (strncmp(start, "lt;", 3) == 0) {
        *(*out)++ = '<';
    } else if (strncmp(start, "gt;", 3) == 0) {
        *(*out)++ = '>';
    } else if (strncmp(start, "quot;", 5) == 0) {
        *(*out)++ = '"';
    } else if (strncmp(start, "apos;", 5) == 0) {
        *(*out)++ = '\'';
    } else {
        // Entidad no reconocida, mantener original
        *(*out)++ = '&';
        while (*start != ';' && *start != '\0') {
            *(*out)++ = *start++;
        }
        if (*start == ';') {
            *(*out)++ = ';';
        }
    }
}

char* stripHTML(const char* html) {
    size_t len = strlen(html);
    char* output = malloc(len + 1);
    if (!output) return NULL;
    char* out = output;
    int in_tag = 0;
    int in_comment = 0;
    int in_script = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (!in_tag && !in_comment && !in_script) {
            if (html[i] == '<') {
                if (i + 4 < len && strncmp(html + i, "<!--", 4) == 0) {
                    in_comment = 1;
                    i += 3;
                } else if (i + 7 < len && strncmp(html + i, "<script", 7) == 0) {
                    in_script = 1;
                    i += 6;
                } else {
                    in_tag = 1;
                }
            } else if (html[i] == '&') {
                const char* start = html + i + 1;
                decodeEntity(start, &out);
                while (html[i] != ';' && html[i] != '\0') i++;
            } else {
                *out++ = html[i];
            }
        } else if (in_comment) {
            if (i + 3 < len && strncmp(html + i, "-->", 3) == 0) {
                in_comment = 0;
                i += 2;
            }
        } else if (in_script) {
            if (i + 9 < len && strncmp(html + i, "</script>", 9) == 0) {
                in_script = 0;
                i += 8;
            }
        } else if (in_tag) {
            if (html[i] == '>') {
                in_tag = 0;
            }
        }
    }
    
    *out = '\0';
    return output;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <archivo_entrada> <archivo_salida>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error al abrir archivo de entrada");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *html = malloc(length + 1);
    if (!html) {
        fclose(file);
        perror("Error de memoria");
        return 1;
    }
    
    fread(html, 1, length, file);
    html[length] = '\0';
    fclose(file);

    char *clean = stripHTML(html);
    if (!clean) {
        free(html);
        perror("Error al limpiar HTML");
        return 1;
    }

    FILE *out = fopen(argv[2], "w");
    if (!out) {
        free(html);
        free(clean);
        perror("Error al crear archivo de salida");
        return 1;
    }
    
    fprintf(out, "%s", clean);
    fclose(out);
    
    free(html);
    free(clean);
    return 0;
}
EOL

gcc -std=c11 -O2 "$TOOLS_DIR/strip_html/strip_html.c" -o "$STRIP_HTML_BIN"

# Descargar y procesar corpus
echo "Descargando corpus..."

# Proyecto Gutenberg
echo "> Descargando Proyecto Gutenberg..."
wget -q -O "$CORPUS_DIR/alice.txt" https://www.gutenberg.org/cache/epub/19033/pg19033.txt
wget -q -O "$CORPUS_DIR/sherlock.txt" https://www.gutenberg.org/files/48320/48320-0.txt


# SMS Spam Collection
echo "> Descargando SMS Spam Collection..."
wget -q -O "$CORPUS_DIR/smsspamcollection.zip" https://archive.ics.uci.edu/static/public/228/sms+spam+collection.zip
unzip -q -d "$CORPUS_DIR" "$CORPUS_DIR/smsspamcollection.zip"
mv "$CORPUS_DIR/SMSSpamCollection" "$CORPUS_DIR/sms_spam.csv"
rm "$CORPUS_DIR/smsspamcollection.zip" "$CORPUS_DIR/readme"

# Wikipedia (opcional)
echo "> Descargando muestra de Wikipedia..."
wget -q -O "$CORPUS_DIR/wikipedia-sample.html" "https://en.wikipedia.org/wiki/Special:Random"
"$STRIP_HTML_BIN" "$CORPUS_DIR/wikipedia-sample.html" "$CORPUS_DIR/wikipedia-sample.txt"
rm "$CORPUS_DIR/wikipedia-sample.html"

# Generar README
echo "Generando README..."
cat > "$CORPUS_DIR/README.md" << 'EOL'
# Corpus de Prueba

Este directorio contiene conjuntos de datos para pruebas:

## 1. Proyecto Gutenberg
- `alice.txt`: Alicia en el País de las Maravillas (Lewis Carroll)
- `sherlock.txt`: Las Aventuras de Sherlock Holmes (Arthur Conan Doyle)
- **Licencia**: Dominio Público

## 2. SMS Spam Collection
- `sms_spam.csv`: 5,574 SMS etiquetados como spam/ham
- **Origen**: [UCI Machine Learning Repository](https://archive.ics.uci.edu/dataset/228/sms+spam+collection)
- **Licencia**: Creative Commons Attribution 4.0 International

## 3. Wikipedia Sample
- `wikipedia-sample.txt`: Artículo aleatorio de Wikipedia
- **Licencia**: Creative Commons Attribution-ShareAlike 3.0

## 4. Samples de texto
- Archivos de texto de relleno para evaluar tipografías y evaluar algoritmos en distintos largos de texto.

> **Nota**: Estos archivos son para uso educativo y de pruebas.
EOL

echo "¡Corpus descargado y procesado en $CORPUS_DIR!"