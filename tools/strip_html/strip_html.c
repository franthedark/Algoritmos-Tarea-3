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
