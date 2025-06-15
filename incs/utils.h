#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

// Enumeración para formas de normalización Unicode
typedef enum {
    UNICODE_NFC, // Forma canónica compuesta
    UNICODE_NFD  // Forma canónica descompuesta
} unicode_normalization_form;

void unicode_case_fold(char* str);
void unicode_normalize(char* str, unicode_normalization_form form);
void remove_diacritics(char* str, int enable_removal);
void unicode_normalize_full(char* str, int remove_diacritics_flag);
char* loadFile(const char* filename);
void toLowerInPlace(char* s);
void squeezeSpaces(char* s);
void convertir_a_minusculas(char *palabra);
void limpiar_palabra(char *palabra);
char* stripHTML(const char* html);

#endif