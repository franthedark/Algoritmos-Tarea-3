// Diego Galindo, Francisco Mercado
#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include "utils.h"

// Para opciones de normalización
typedef struct {
    int remove_diacritics;
    int use_unicode_advanced;
    unicode_normalization_form norm_form;
    int use_basic_normalization;
} NormalizationOptions;

// Para parsear las opciones de normalización desde argumentos de línea de comandos
NormalizationOptions parseNormalizationOptions(int argc, char* argv[]);

// Para aplicar la normalización al texto y patrón según las opciones
void applyNormalization(char* text, char* pattern, const NormalizationOptions* opts);

#endif 