#include "KMP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void computeLPSArray(const char *pat, size_t M, int *lps) {
    if (!pat || !lps || M == 0) return;
    lps[0] = 0;
    size_t len = 0, i = 1;
    while (i < M) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i++] = (int)len;
        } else if (len != 0) {
            len = lps[len - 1];
        } else {
            lps[i++] = 0;
        }
    }
}

void searchKMP(const char *pattern, const char *text) {
    if (!pattern || !text) {
        fprintf(stderr, "searchKMP: patrón o texto NULL\n");
        return;
    }

    size_t M = strlen(pattern);
    size_t N = strlen(text);

    if (M == 0) {
        fprintf(stderr, "searchKMP: patrón vacío\n");
        return;
    }
    if (N == 0) {
        fprintf(stderr, "searchKMP: texto vacío\n");
        return;
    }
    if (M > N) {
        return;  //patron mas largo que el texto
    }

    int *lps = malloc(M * sizeof(int));
    if (!lps) {
        perror("searchKMP: malloc lps falló");
        return;
    }

    computeLPSArray(pattern, M, lps);

    size_t i = 0, j = 0;
    while (i < N) {
        if (pattern[j] == text[i]) {
            i++; j++;
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