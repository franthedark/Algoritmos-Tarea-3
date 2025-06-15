#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "utils.h"
#include "KMP.h"
#include "boyer_moore.h"
#include "shift_and.h"
#include "index_operations.h"
#include "normalization.h"

static int endsWith(const char* str, const char* suffix) {
    size_t n = strlen(str), m = strlen(suffix);
    if (m > n) return 0;
    return strcmp(str + n - m, suffix) == 0;
}

static void printUsage(const char* prog) {
    printError("Uso:");
    fprintf(stderr,
        "  Búsqueda de patrones:\n"
        "    %s <algoritmo> <patrón> <archivo>\n"
        "    algoritmos disponibles: kmp, bm, shiftand\n\n",
        prog
    );
    printIndexUsage(prog);
    fprintf(stderr,
        "\nEjemplo:\n"
        "  %s bm \"patrón\" documento.html\n",
        prog
    );
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // comandos de indice
    if (strcmp(argv[1], "index") == 0) {
        return handleIndexCommands(argc, argv);
    }

    // busqueda de patrones
    if (argc < 4) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* alg = argv[1];
    const char* patArg = argv[2];
    const char* filename = argv[3];

    // copia patron a un buffer modificable
    char* pattern = malloc(strlen(patArg) + 1);
    if (!pattern) {
        printError("malloc falló para pattern");
        return EXIT_FAILURE;
    }
    strcpy(pattern, patArg);

    // lee archivo completo
    char* raw = loadFile(filename);
    if (!raw) {
        free(pattern);
        return EXIT_FAILURE;
    }

    // si es html elimina etiquetas y entidades
    char* text;
    if (endsWith(filename, ".html") || endsWith(filename, ".htm")) {
        text = stripHTML(raw);
        free(raw);
        if (!text) {
            printError("stripHTML devolvió NULL");
            free(pattern);
            return EXIT_FAILURE;
        }
    } else {
        text = raw;
    }

    // ======================================================================
    // NUEVO: ANALIZAR Y APLICAR NORMALIZACIÓN (REEMPLAZA EL PREPROCESAMIENTO)
    // ======================================================================
    
    // Analizar opciones de normalización (--nfc, --no-diacritics, etc.)
    NormalizationOptions norm_opts = parseNormalizationOptions(argc, argv);
    
    // Aplicar normalización al texto y patrón
    applyNormalization(text, pattern, &norm_opts);
    
    // Mostrar información de depuración
    printf(">>> Algoritmo: %s | Patrón original: \"%s\" | Archivo: %s\n", 
           alg, patArg, filename);
    printf(">>> Patrón normalizado: \"%s\"\n\n", pattern);

    // encabezado de tabla
    const char* cols[] = { "Algoritmo", "Posición" };
    printTableHeader(cols, 2);
    // ejecuta el algoritmo seleccionado
    if (strcmp(alg, "kmp") == 0) {
        searchKMP(pattern, text);
    }
    else if (strcmp(alg, "bm") == 0) {
        searchBoyerMooreUnicode(pattern, text);
    }
    else if (strcmp(alg, "shiftand") == 0) {
        searchShiftAnd(pattern, text);
    }
    else {
        printError("Algoritmo no reconocido:");
        fprintf(stderr, "  %s\n", alg);
        printError("Opciones válidas: kmp, bm, shiftand");
        free(text);
        free(pattern);
        return EXIT_FAILURE;
    }
    free(text);
    free(pattern);
    return EXIT_SUCCESS;
}