#include "KMP.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void computeLPSArray(const char* pat, size_t M, int* lps) {
    lps[0] = 0;
    size_t len = 0;
    size_t i = 1;
    while (i < M) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i] = (int)len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

void searchKMP(const char* pattern, const char* text) {
    size_t M = strlen(pattern);
    size_t N = strlen(text);
    if (M == 0 || N == 0) return;

    int* lps = (int*)malloc(sizeof(int) * M);
    if (!lps) {
        fprintf(stderr, "searchKMP: error de malloc para lps[%zu]\n", M);
        return;
    }
    computeLPSArray(pattern, M, lps);

    size_t i = 0, j = 0;
    while (i < N) {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }
        if (j == M) {
            printf("Patrón encontrado en posición %zu\n", i - j);
            j = lps[j - 1];
        } else if (i < N && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    free(lps);
}