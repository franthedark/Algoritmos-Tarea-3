#include "boyer_moore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

void preprocessBadChar(const char *pat, int badChar[256]) {
    for (int c = 0; c < 256; c++)
        badChar[c] = -1;
    for (size_t i = 0; pat[i]; i++)
        badChar[(unsigned char)pat[i]] = (int)i;
}

static void computeSuffixes(const char *pat, size_t M, size_t *suffix) {
    suffix[M-1] = M;
    size_t g = M-1, f = 0;
    for (ptrdiff_t i = (ptrdiff_t)M - 2; i >= 0; i--) {
        if ((size_t)i > g && suffix[i + M - 1 - f] < (size_t)(i - g)) {
            suffix[i] = suffix[i + M - 1 - f];
        } else {
            if ((size_t)i < g) g = (size_t)i;
            f = (size_t)i;
            while ((ptrdiff_t)g >= 0 && pat[g] == pat[g + M - 1 - f]) g--;
            suffix[i] = f - g;
        }
    }
}

void preprocessGoodSuffix(const char *pat, size_t M, size_t *shiftGS) {
    size_t *suffix = malloc((M+1) * sizeof(size_t));
    if (!suffix) return;
    computeSuffixes(pat, M, suffix);

    for (size_t i = 0; i <= M; i++)
        shiftGS[i] = M;

    for (size_t i = 0; i < M; i++) {
        size_t slen = suffix[i];
        if (slen > 0)
            shiftGS[M - slen] = M - 1 - i + slen;
    }

    size_t j = 0;
    for (ptrdiff_t i = (ptrdiff_t)M - 1; i >= 0; i--) {
        if (suffix[i] == (size_t)(i + 1)) {
            for (; j < (size_t)(M - 1 - i); j++) {
                if (shiftGS[j] == M)
                    shiftGS[j] = M - 1 - i;
            }
        }
    }

    free(suffix);
}

void searchBoyerMoore(const char *pattern, const char *text) {
    if (!pattern || !text) {
        fprintf(stderr, "searchBoyerMoore: patrón o texto NULL\n");
        return;
    }
    size_t M = strlen(pattern), N = strlen(text);
    if (M == 0) {
        fprintf(stderr, "searchBoyerMoore: patrón vacío\n");
        return;
    }
    if (N == 0) {
        fprintf(stderr, "searchBoyerMoore: texto vacío\n");
        return;
    }
    if (M > N) return;

    int badChar[256];
    preprocessBadChar(pattern, badChar);

    size_t *shiftGS = malloc((M+1) * sizeof(size_t));
    if (!shiftGS) {
        perror("searchBoyerMoore: malloc shiftGS falló");
        return;
    }
    preprocessGoodSuffix(pattern, M, shiftGS);

    size_t s = 0;
    while (s <= N - M) {
        ptrdiff_t j = (ptrdiff_t)M - 1;
        while (j >= 0 && pattern[j] == text[s + j])
            j--;
        if (j < 0) {
            printf("Patrón encontrado en posición %zu\n", s);
            s += shiftGS[0];
        } else {
            size_t bcShift = (size_t)(j - badChar[(unsigned char)text[s + j]]);
            size_t gsShift = shiftGS[j + 1];
            s += (bcShift > gsShift ? bcShift : gsShift);
        }
    }

    free(shiftGS);
}