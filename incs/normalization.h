#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include "utils.h"

// Estructura para opciones de normalización
typedef struct {
    int remove_diacritics;
    int use_unicode_advanced;
    unicode_normalization_form norm_form;
    int use_basic_normalization;
} NormalizationOptions;

// Función para parsear las opciones de normalización desde argumentos de línea de comandos
NormalizationOptions parseNormalizationOptions(int argc, char* argv[]);

// Función para aplicar la normalización al texto y patrón según las opciones
void applyNormalization(char* text, char* pattern, const NormalizationOptions* opts);

#endif // NORMALIZATION_H