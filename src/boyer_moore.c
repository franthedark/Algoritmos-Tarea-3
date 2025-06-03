#include "boyer_moore.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void preprocessBadChar(const char* pat, int badChar[256]) {
    size_t M = strlen(pat);
    for (int i = 0; i < 256; i++) {
        badChar[i] = -1;
    }
    //guarda ultimo indice para cada caracter del patron
    for (size_t i = 0; i < M; i++) {
        unsigned char c = (unsigned char)pat[i];
        badChar[c] = (int)i;
    }
}

void searchBoyerMoore(const char* pattern, const char* text) {
    size_t M = strlen(pattern);
    size_t N = strlen(text);
    if (M == 0 || N == 0) return;

    int badChar[256];
    preprocessBadChar(pattern, badChar);

    size_t s = 0; //desplazamiento del patron respecto al texto
    while (s <= N - M) {
        int j = (int)M - 1;
        //comparar desde el final del patron hacia atras
        while (j >= 0 && pattern[j] == text[s + j]) {
            j--;
        }
        if (j < 0) {
            printf("Patrón encontrado en posición %zu\n", s);
            //esplazar patron para buscar otra coincidencia
            s += (s + M < N) ? M - badChar[(unsigned char)text[s + M]] : 1;
        } else {
            unsigned char c = (unsigned char)text[s + j];
            int bc = badChar[c];
            int shift = j - bc;
            s += (shift > 0) ? shift : 1;
        }
    }
}