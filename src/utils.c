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
    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("loadFile: fseek falló");
        fclose(fp);
        return NULL;
    }
    long size = ftell(fp);
    if (size < 0) {
        perror("loadFile: ftell devolvió error");
        fclose(fp);
        return NULL;
    }
    rewind(fp);

    char* buffer = (char*)malloc((size_t)size + 1);
    if (!buffer) {
        fprintf(stderr, "loadFile: malloc devolvió NULL para %ld bytes\n", size + 1);
        fclose(fp);
        return NULL;
    }
    size_t readBytes = fread(buffer, 1, (size_t)size, fp);
    if (readBytes != (size_t)size) {
        fprintf(stderr, "loadFile: leídos %zu de %ld bytes\n", readBytes, size);
        free(buffer);
        fclose(fp);
        return NULL;
    }
    buffer[size] = '\0';
    fclose(fp);
    return buffer;
}

char* stripHTML(const char* html) {
    size_t len = strlen(html);
    char* output = (char*)malloc(len + 1);
    if (!output) return NULL;

    size_t i = 0, o = 0;
    while (i < len) {
        if (html[i] == '<') {
            while (i < len && html[i] != '>') i++;
            if (i < len && html[i] == '>') i++;
        }
        else if (html[i] == '&') {
            if (strncmp(html + i, "&amp;", 5) == 0)      { output[o++] = '&';   i += 5; }
            else if (strncmp(html + i, "&lt;", 4) == 0)  { output[o++] = '<';   i += 4; }
            else if (strncmp(html + i, "&gt;", 4) == 0)  { output[o++] = '>';   i += 4; }
            else if (strncmp(html + i, "&nbsp;", 6) == 0){ output[o++] = ' ';   i += 6; }
            else { output[o++] = html[i++]; }
        } else {
            output[o++] = html[i++];
        }
    }
    output[o] = '\0';
    return output;
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