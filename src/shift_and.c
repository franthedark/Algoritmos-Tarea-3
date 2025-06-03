#include "shift_and.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//solo funciona si pattern.length <= 64
static void buildMask(const char* pat, unsigned long long masks[256]) {
    size_t M = strlen(pat);
    for (int c = 0; c < 256; c++) {
        masks[c] = 0ULL;
    }
    for (size_t i = 0; i < M; i++) {
        unsigned char c = (unsigned char)pat[i];
        masks[c] |= (1ULL << i);
    }
}

void searchShiftAnd(const char* pattern, const char* text) {
    size_t M = strlen(pattern);
    size_t N = strlen(text);
    if (M == 0 || N == 0 || M > 64) {
        //si el patron es más largo de 64, shift-and trivial no sirve
        return;
    }

    unsigned long long masks[256];
    buildMask(pattern, masks);

    unsigned long long R = 0ULL;
    unsigned long long matchBit = 1ULL << (M - 1);

    for (size_t i = 0; i < N; i++) {
        unsigned char c = (unsigned char)text[i];
        //avanzar r shift left or con mascara invertida & con mascara del caracter
        R = ((R << 1) | 1ULL) & masks[c];
        if (R & matchBit) {
            //coincidencia completa del patron terminando en i
            printf("Patrón encontrado en posición %zu\n", i - M + 1);
        }
    }
}