#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Tabla de normalización (acentos a ASCII)
void buildNormalizationTable(unsigned char table[256]) {
    // Mapeo de caracteres especiales
    const char* from = "ÁÀÂÄáàâäÉÈÊËéèêëÍÌÎÏíìîïÓÒÔÖóòôöÚÙÛÜúùûüÑñÇç";
    const char* to   = "AAAAaaaaEEEEeeeeIIIIiiiiOOOOooooUUUUuuuuNnCc";
    
    // Inicializar tabla como identidad
    for (int i = 0; i < 256; i++) {
        table[i] = i;
    }
    
    // Convertir mayúsculas ASCII a minúsculas
    for (int c = 'A'; c <= 'Z'; c++) {
        table[c] = c - 'A' + 'a';
    }
    
    // Mapear caracteres especiales
    for (size_t i = 0; from[i] != '\0'; i++) {
        table[(unsigned char)from[i]] = to[i];
    }
}

// Normaliza texto (minúsculas + sin acentos)
void normalizeString(char* str) {
    static unsigned char table[256];
    static int initialized = 0;
    
    if (!initialized) {
        // Inicializar tabla
        for (int i = 0; i < 256; i++) {
            table[i] = i;
        }
        
        // Minúsculas para ASCII
        for (int c = 'A'; c <= 'Z'; c++) {
            table[c] = c - 'A' + 'a';
        }
        
        // Caracteres especiales
        const char* from = "ÁÀÂÄÃáàâäãÉÈÊËéèêëÍÌÎÏíìîïÓÒÔÖÕóòôöõÚÙÛÜúùûüÑñÇç";
        const char* to   = "AAAAAaaaaaEEEEeeeeIIIIiiiiOOOOOoooooUUUUuuuuNnCc";
        
        for (size_t i = 0; from[i] != '\0'; i++) {
            table[(unsigned char)from[i]] = to[i];
        }
        initialized = 1;
    }
    
    for (size_t i = 0; str[i]; i++) {
        str[i] = (char)table[(unsigned char)str[i]];
    }
}

char* loadFile(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("loadFile: no se pudo abrir el archivo");
        return NULL;
    }
    
    // Check for UTF-8 BOM (3 bytes: EF BB BF)
    unsigned char bom[3];
    size_t bom_size = fread(bom, 1, 3, fp);
    int has_bom = (bom_size == 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF);
    
    // Get file size
    if (fseek(fp, 0, SEEK_END)) {
        perror("loadFile: fseek falló");
        fclose(fp);
        return NULL;
    }
    long file_size = ftell(fp);
    if (file_size < 0) {
        perror("loadFile: ftell devolvió error");
        fclose(fp);
        return NULL;
    }
    
    // Calculate actual content size
    size_t content_size = (size_t)file_size;
    if (has_bom) {
        if (file_size >= 3) {
            content_size -= 3;
            fseek(fp, 3, SEEK_SET);  // Skip BOM
        } else {
            // File smaller than BOM
            content_size = 0;
        }
    } else {
        rewind(fp);
    }

    char* buffer = malloc(content_size + 1);
    if (!buffer) {
        fprintf(stderr, "loadFile: malloc devolvió NULL para %zu bytes\n", content_size + 1);
        fclose(fp);
        return NULL;
    }
    
    size_t readBytes = fread(buffer, 1, content_size, fp);
    fclose(fp);
    
    if (readBytes != content_size) {
        fprintf(stderr, "loadFile: leídos %zu de %zu bytes\n", readBytes, content_size);
        free(buffer);
        return NULL;
    }
    
    buffer[content_size] = '\0';
    return buffer;
}

void toLowerInPlace(char* s) {
    for (size_t i = 0; s[i]; i++)
        s[i] = (char)tolower((unsigned char)s[i]);
}

void squeezeSpaces(char* s) {
    size_t i = 0, o = 0;
    int in_space = 0;
    while (s[i]) {
        if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t' || s[i] == '\r') {
            if (!in_space) {
                s[o++] = ' ';
                in_space = 1;
            }
            i++;
        } else {
            in_space = 0;
            s[o++] = s[i++];
        }
    }
    if (o > 0 && s[o - 1] == ' ') o--;
    s[o] = '\0';
}

void convertir_a_minusculas(char *palabra) {
    int j = 0;
    for (int i = 0; palabra[i]; ) {
        unsigned char c = palabra[i];
        if (c == 0xC3) { // UTF-8 accented character
            unsigned char c2 = palabra[i+1];
            if (c2) {
                i++;
                switch(c2) {
                    case 0xA1: case 0xA0: case 0xA4: case 0xA2: case 0xA3: case 0xA5: 
                        palabra[j++] = 'a'; break;  // á, à, ä, etc.
                    case 0xA9: case 0xA8: case 0xAB: case 0xAA: 
                        palabra[j++] = 'e'; break;  // é, è, ë, etc.
                    case 0xAD: case 0xAC: case 0xAF: case 0xAE: 
                        palabra[j++] = 'i'; break;  // í, ì, ï, etc.
                    case 0xB3: case 0xB2: case 0xB6: case 0xB4: case 0xB5: 
                        palabra[j++] = 'o'; break;  // ó, ò, ö, etc.
                    case 0xBA: case 0xB9: case 0xBC: case 0xBB: 
                        palabra[j++] = 'u'; break;  // ú, ù, ü, etc.
                    case 0xB1: 
                        palabra[j++] = 'n'; break;  // ñ
                    default: 
                        // Skip unrecognized UTF-8
                        i++;
                        continue;
                }
                i++;
            } else {
                i++;
            }
        } else if (c >= 128) { // Skip other non-ASCII characters
            i++;
        } else { // Handle ASCII characters
            palabra[j++] = tolower(c);
            i++;
        }
    }
    palabra[j] = '\0';
}

void limpiar_palabra(char *palabra) {
    int j = 0;
    for (int i = 0; palabra[i]; i++) {
        unsigned char c = palabra[i];
        if (isalnum(c) || c == ' ') {
            palabra[j++] = c;
        }
    }
    palabra[j] = '\0';
}

static void decodeEntity(const char* start, char** output) {
    if (*start == '#') {
        start++;
        int base = 10;
        if (*start == 'x' || *start == 'X') {
            start++;
            base = 16;
        }
        
        char* end;
        long code = strtol(start, &end, base);
        
        if (*end == ';' && code != 0) {
            // Convert to UTF-8 (simplified)
            if (code < 0x80) {
                *(*output)++ = (char)code;
            } else if (code < 0x800) {
                *(*output)++ = 0xC0 | (code >> 6);
                *(*output)++ = 0x80 | (code & 0x3F);
            } else if (code < 0x10000) {
                *(*output)++ = 0xE0 | (code >> 12);
                *(*output)++ = 0x80 | ((code >> 6) & 0x3F);
                *(*output)++ = 0x80 | (code & 0x3F);
            } else {
                *(*output)++ = 0xF0 | (code >> 18);
                *(*output)++ = 0x80 | ((code >> 12) & 0x3F);
                *(*output)++ = 0x80 | ((code >> 6) & 0x3F);
                *(*output)++ = 0x80 | (code & 0x3F);
            }
            return;
        }
    } else {
        // Named entities
        const char *entity = start;
        size_t len = strcspn(start, ";");
        
        if (strncmp(entity, "amp", len) == 0) *(*output)++ = '&';
        else if (strncmp(entity, "lt", len) == 0) *(*output)++ = '<';
        else if (strncmp(entity, "gt", len) == 0) *(*output)++ = '>';
        else if (strncmp(entity, "quot", len) == 0) *(*output)++ = '"';
        else if (strncmp(entity, "apos", len) == 0) *(*output)++ = '\'';
        else if (strncmp(entity, "nbsp", len) == 0) *(*output)++ = ' ';
        else if (strncmp(entity, "copy", len) == 0) { *(*output)++ = 0xC2; *(*output)++ = 0xA9; }
        // Add more entities as needed...
        else {
            // Unknown entity - copy as-is
            *(*output)++ = '&';
            while (*start && *start != ';') *(*output)++ = *start++;
            if (*start == ';') *(*output)++ = ';';
            return;
        }
    }
    
    // Skip to semicolon
    while (*start && *start != ';') start++;
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
                // Check for comments or scripts
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
                // Skip processed entity
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