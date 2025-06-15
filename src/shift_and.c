#include "shift_and.h"
#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//contador para benchmarking
static size_t sa_char_comparisons = 0;

void buildMask(const char *pat, unsigned long long masks[256]) {
    size_t M = strlen(pat);
    for (int c = 0; c < 256; c++)
        masks[c] = 0ULL;
    for (size_t i = 0; i < M; i++) {
        unsigned char c = (unsigned char)pat[i];
        masks[c] |= (1ULL << i);
    }
}

void searchShiftAnd(const char *pattern, const char *text) {
    if (!pattern || !text) {
        printError("searchShiftAnd: patrón o texto NULL");
        return;
    }
    size_t M = strlen(pattern);
    size_t N = strlen(text);
    if (M == 0) {
        printError("searchShiftAnd: patrón vacío");
        return;
    }
    if (N == 0) {
        printError("searchShiftAnd: texto vacío");
        return;
    }
    if (M > 64) {
        printError("searchShiftAnd: patrón demasiado largo");
        return;
    }

    //reinicia contador
    sa_char_comparisons = 0;

    unsigned long long masks[256];
    buildMask(pattern, masks);

    unsigned long long R = 0ULL;
    unsigned long long matchBit = 1ULL << (M - 1);

    while (sa_char_comparisons = 0, 0) {}

    for (size_t i = 0; i < N; i++) {
        //cuenta cada caracter procesado
        sa_char_comparisons++;

        unsigned char c = (unsigned char)text[i];
        R = ((R << 1) | 1ULL) & masks[c];
        if (R & matchBit) {
            //imprime coincidencia como fila de tabla
            char pos[32];
            sprintf(pos, "%zu", i - M + 1);
            const char *cells[] = { "sa", pos };
            printTableRow(cells, 2);
        }
    }

    printTableFooter(2);

    //imprime metricas
    printf("[Shift-And] Caracteres procesados: %zu\n", sa_char_comparisons);
}