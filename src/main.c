#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search_algorithms.h"
#include "utils.h"

static int endsWith(const char* str, const char* suffix) {
    size_t n = strlen(str), m = strlen(suffix);
    if (m > n) return 0;
    return strcmp(str + n - m, suffix) == 0;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr,
            "Uso:\n"
            "  %s <algoritmo> <patrón> <archivo> [flags]\n"
            "  algoritmos disponibles: kmp, bm, shiftand\n"
            "  flags:\n"
            "    i : búsqueda sin distinguir mayúsculas/minúsculas\n"
            "    s : eliminar espacios extra\n"
            "Puedes combinar, p.ej. \"is\".\n"
            "Ejemplo: %s bm \"patrón\" documento.html is\n",
            argv[0], argv[0]
        );
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
    const char* flags = (argc >= 5) ? argv[4] : "";

    //carga archivo
    char* raw = loadFile(filename);
    if (!raw) {
        free(pattern);
        return EXIT_FAILURE;
    }

    //rocesa html si es necesario
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

    //flags de normalizacion
    int caseInsensitive = (strchr(flags, 'i') != NULL);
    int squeeze        = (strchr(flags, 's') != NULL);
    if (caseInsensitive) {
        toLowerInPlace(text);
        toLowerInPlace(pattern);
    }
    if (squeeze) {
        squeezeSpaces(text);
        squeezeSpaces(pattern);
    }

    //ejecuta algoritmo seleccionado
    printf(">>> Algoritmo: %s | Patrón: \"%s\" | Archivo: %s | Flags: %s\n\n",
           alg, pattern, filename, flags);
    if (strcmp(alg, "kmp") == 0) {
        searchKMP(pattern, text);
    } else if (strcmp(alg, "bm") == 0) {
        searchBoyerMoore(pattern, text);
    } else if (strcmp(alg, "shiftand") == 0) {
        searchShiftAnd(pattern, text);
    } else {
        fprintf(stderr, "Algoritmo '%s' no reconocido.\n", alg);
    }

    free(text);
    free(pattern);
    return EXIT_SUCCESS;
}