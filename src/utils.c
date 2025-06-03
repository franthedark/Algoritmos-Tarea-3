#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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