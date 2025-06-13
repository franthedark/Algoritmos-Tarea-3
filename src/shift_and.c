#include "shift_and.h"
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
        fprintf(stderr, "searchShiftAnd: patrón o texto NULL\n");
        return;
    }
    size_t M = strlen(pattern);
    size_t N = strlen(text);
    if (M == 0) {
        fprintf(stderr, "searchShiftAnd: patrón vacío\n");
        return;
    }
    if (N == 0) {
        fprintf(stderr, "searchShiftAnd: texto vacío\n");
        return;
    }
    if (M > 64) {
        fprintf(stderr,
            "searchShiftAnd: patrón demasiado largo (%zu > 64), no soportado\n",
            M);
        return;
    }

    //reinicia contador
    sa_char_comparisons = 0;

    unsigned long long masks[256];
    buildMask(pattern, masks);

    unsigned long long R = 0ULL;
    unsigned long long matchBit = 1ULL << (M - 1);

    for (size_t i = 0; i < N; i++) {
        //cuenta cada caracter procesado
        sa_char_comparisons++;

        unsigned char c = (unsigned char)text[i];
        R = ((R << 1) | 1ULL) & masks[c];
        if (R & matchBit) {
            printf("Patrón encontrado en posición %zu\n", i - M + 1);
        }
    }

    //imprimir metricas
    printf("[Shift-And] Caracteres procesados: %zu\n", sa_char_comparisons);
}