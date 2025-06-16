// Diego Galindo, Francisco Mercado
#include <stdio.h>
#include <string.h>
#include "normalization.h"
#include "utils.h"

NormalizationOptions parseNormalizationOptions(int argc, char* argv[]) {
    NormalizationOptions opts = {
        .remove_diacritics = 0,
        .use_unicode_advanced = 0,
        .norm_form = UNICODE_NFC,
        .use_basic_normalization = 1  // Por defecto usar normalización básica
    };
    
    for (int i = 4; i < argc; i++) {
        if (strcmp(argv[i], "no-diacritics") == 0) {
            opts.remove_diacritics = 1;
            opts.use_unicode_advanced = 1;
            opts.use_basic_normalization = 0;
        } else if (strcmp(argv[i], "nfc") == 0) {
            opts.norm_form = UNICODE_NFC;
            opts.use_unicode_advanced = 1;
            opts.use_basic_normalization = 0;
        } else if (strcmp(argv[i], "nfd") == 0) {
            opts.norm_form = UNICODE_NFD;
            opts.use_unicode_advanced = 1;
            opts.use_basic_normalization = 0;
        } else if (strcmp(argv[i], "basic") == 0) {
            opts.use_basic_normalization = 1;
            opts.use_unicode_advanced = 0;
        }
    }
    
    return opts;
}

void applyNormalization(char* text, char* pattern, const NormalizationOptions* opts) {
    if (opts->use_unicode_advanced) {
        // Usar normalización Unicode avanzada
        printf(">>> Aplicando normalización Unicode avanzada...\n");
        
        // Normalizar texto
        if (opts->norm_form == UNICODE_NFC || opts->norm_form == UNICODE_NFD) {
            unicode_normalize(text, opts->norm_form);
            unicode_normalize(pattern, opts->norm_form);
        }
        
        // Aplicar case-folding Unicode
        unicode_case_fold(text);
        unicode_case_fold(pattern);
        
        // Eliminar diacríticos si se solicita
        if (opts->remove_diacritics) {
            remove_diacritics(text, 1);
            remove_diacritics(pattern, 1);
            printf(">>> Diacríticos eliminados\n");
        }
        
        // Limpiar espacios
        squeezeSpaces(text);
        squeezeSpaces(pattern);
        
    } else {
        // Usar normalización básica (comportamiento original)
        printf(">>> Aplicando normalización básica...\n");
        convertir_a_minusculas(text);
        limpiar_palabra(text);
        convertir_a_minusculas(pattern);
        limpiar_palabra(pattern);
    }
}