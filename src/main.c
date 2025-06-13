#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search_algorithms.h"
#include "utils.h"
#include "KMP.h"
#include "boyer_moore.h"
#include "index_operations.h"

static int endsWith(const char* str, const char* suffix) {
    size_t n = strlen(str), m = strlen(suffix);
    if (m > n) return 0;
    return strcmp(str + n - m, suffix) == 0;
}

static void printUsage(const char* program_name) {
    fprintf(stderr,
        "Uso:\n"
        "  Búsqueda de patrones:\n"
        "    %s <algoritmo> <patrón> <archivo>\n"
        "    algoritmos disponibles: kmp, bm, shiftand\n"
        "\n",
        program_name
    );
    
    // Mostrar también la ayuda de índices
    printIndexUsage(program_name);
    
    fprintf(stderr,
        "\nEjemplos:\n"
        "  %s bm \"patrón\" documento.html\n",
        program_name
    );
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }
    
    // Verificar si es un comando de índice
    if (strcmp(argv[1], "index") == 0) {
        return handleIndexCommands(argc, argv);
    }
    
    // Funcionalidad original de búsqueda de patrones
    if (argc < 4) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }
    
    const char* alg = argv[1];
    char* pattern = malloc(strlen(argv[2]) + 1);
    if (!pattern) {
        fprintf(stderr, "Error: malloc falló para pattern\n");
        return EXIT_FAILURE;
    }
    strcpy(pattern, argv[2]);
    const char* filename = argv[3];
    
    // Cargar archivo
    char* raw = loadFile(filename);
    if (!raw) {
        free(pattern);
        return EXIT_FAILURE;
    }
    
    // Procesar HTML si es necesario
    char* text;
    if (endsWith(filename, ".html") || endsWith(filename, ".htm")) {
        text = stripHTML(raw);
        free(raw);
        if (!text) {
            fprintf(stderr, "Error: stripHTML devolvió NULL\n");
            free(pattern);
            return EXIT_FAILURE;
        }
    } else {
        text = raw;
    }
    
    // Normalización automática (siempre se aplica)
    convertir_a_minusculas(text);
    limpiar_palabra(text);
    convertir_a_minusculas(pattern);
    limpiar_palabra(pattern);
    
    // Ejecutar algoritmo seleccionado
    printf(">>> Algoritmo: %s | Patrón: \"%s\" | Archivo: %s\n\n",
           alg, argv[2], filename);
    
    if (strcmp(alg, "kmp") == 0) {
        searchKMP(pattern, text);
    } else if (strcmp(alg, "bm") == 0) {
        searchBoyerMooreUnicode(pattern, text);
    } else if (strcmp(alg, "shiftand") == 0) {
        searchShiftAnd(pattern, text);
    } else {
        fprintf(stderr, "Algoritmo '%s' no reconocido. Recuerda que los algoritmos disponibles son:\n- Knuth-Morris-Pratt (kmp)\n- Boyer Moore (bm)\n- Shift-And (shiftand)\n", alg);
        free(text);
        free(pattern);
        return EXIT_FAILURE;
    }
    
    free(text);
    free(pattern);
    return EXIT_SUCCESS;
}