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
    size_t M = strlen(pattern), N = strlen(text);
    if (M == 0) {
        fprintf(stderr, "searchKMP: patrón vacío\n");
        return;
    }
    if (N == 0) {
        fprintf(stderr, "searchKMP: texto vacío\n");
        return;
    }
    if (M > N) return;

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

void buildDFA(const char *pat, size_t M, int R, int *dfa) {
    if (!pat || !dfa || M == 0 || R <= 0) return;
    size_t nStates = M + 1;
    for (int c = 0; c < R; c++)
        dfa[c * nStates + 0] = 0;
    dfa[(unsigned char)pat[0] * nStates + 0] = 1;
    int X = 0;
    for (size_t j = 1; j < M; j++) {
        for (int c = 0; c < R; c++)
            dfa[c * nStates + j] = dfa[c * nStates + X];
        dfa[(unsigned char)pat[j] * nStates + j] = (int)(j + 1);
        X = dfa[(unsigned char)pat[j] * nStates + X];
    }
    //ultimo estado
    for (int c = 0; c < R; c++)
        dfa[c * nStates + M] = dfa[c * nStates + X];
}

void searchKMP_DFA(const char *pattern, const char *text) {
    if (!pattern || !text) {
        fprintf(stderr, "searchKMP_DFA: patrón o texto NULL\n");
        return;
    }
    size_t M = strlen(pattern), N = strlen(text);
    if (M == 0) {
        fprintf(stderr, "searchKMP_DFA: patrón vacío\n");
        return;
    }
    if (N == 0) {
        fprintf(stderr, "searchKMP_DFA: texto vacío\n");
        return;
    }
    if (M > N) return;

    int R = 256;
    size_t nStates = M + 1;
    int *dfa = malloc(R * nStates * sizeof(int));
    if (!dfa) {
        perror("searchKMP_DFA: malloc dfa falló");
        return;
    }
    buildDFA(pattern, M, R, dfa);

    int state = 0;
    for (size_t i = 0; i < N; i++) {
        unsigned char c = (unsigned char)text[i];
        state = dfa[c * nStates + state];
        if (state == (int)M) {
            printf("Patrón encontrado en posición %zu\n", i - M + 1);
        }
    }
    free(dfa);
}