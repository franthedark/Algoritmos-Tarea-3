#include "boyer_moore.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint32_t* decodeUTF8(const char *utf8, size_t *outLen) {
    size_t cap = 16, len = 0;
    uint32_t *arr = malloc(cap * sizeof(uint32_t));
    if (!arr) return NULL;
    const unsigned char *p = (const unsigned char*)utf8;
    while (*p) {
        uint32_t cp = 0;
        if (*p < 0x80) {
            cp = *p++;
        } else if ((*p & 0xE0) == 0xC0 && (p[1]&0xC0)==0x80) {
            cp = ((*p & 0x1F)<<6) | (p[1]&0x3F); p += 2;
        } else if ((*p & 0xF0) == 0xE0 && (p[1]&0xC0)==0x80 && (p[2]&0xC0)==0x80) {
            cp = ((*p & 0x0F)<<12)|((p[1]&0x3F)<<6)|(p[2]&0x3F); p+=3;
        } else if ((*p & 0xF8) == 0xF0 && (p[1]&0xC0)==0x80 && (p[2]&0xC0)==0x80 && (p[3]&0xC0)==0x80) {
            cp = ((*p & 0x07)<<18)|((p[1]&0x3F)<<12)|((p[2]&0x3F)<<6)|(p[3]&0x3F); p+=4;
        } else {
            //byte invalido saltar
            p++;
            continue;
        }
        if (len+1 >= cap) {
            cap *= 2;
            arr = realloc(arr, cap * sizeof(uint32_t));
            if (!arr) return NULL;
        }
        arr[len++] = cp;
    }
    *outLen = len;
    return arr;
}

BMMapEntry* preprocessBadCharUnicode(const uint32_t *pat, size_t M, size_t *mapSize) {
    BMMapEntry *map = malloc(M * sizeof(BMMapEntry));
    if (!map) return NULL;
    size_t sz = 0;
    for (size_t i = 0; i < M; i++) {
        uint32_t c = pat[i];
        // buscar si ya existe
        ptrdiff_t idx = -1;
        for (size_t k = 0; k < sz; k++) {
            if (map[k].cp == c) { idx = (ptrdiff_t)k; break; }
        }
        if (idx >= 0) {
            map[idx].last = (ptrdiff_t)i;
        } else {
            map[sz].cp   = c;
            map[sz].last = (ptrdiff_t)i;
            sz++;
        }
    }
    *mapSize = sz;
    return map;
}

static void computeSuffixesUnicode(const uint32_t *pat, size_t M, size_t *suffix) {
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

void preprocessGoodSuffixUnicode(const uint32_t *pat, size_t M, size_t *shiftGS) {
    size_t *suffix = malloc((M+1)*sizeof(size_t));
    if (!suffix) return;
    computeSuffixesUnicode(pat, M, suffix);
    for (size_t i = 0; i <= M; i++) shiftGS[i] = M;
    for (size_t i = 0; i < M; i++) {
        size_t sl = suffix[i];
        if (sl > 0) shiftGS[M - sl] = M - 1 - i + sl;
    }
    size_t j = 0;
    for (ptrdiff_t i = (ptrdiff_t)M - 1; i >= 0; i--) {
        if (suffix[i] == (size_t)(i + 1)) {
            for (; j < (size_t)(M - 1 - i); j++)
                if (shiftGS[j] == M)
                    shiftGS[j] = M - 1 - i;
        }
    }
    free(suffix);
}

void searchBoyerMooreUnicode(const char *patternUTF8, const char *textUTF8) {
    if (!patternUTF8 || !textUTF8) {
        fprintf(stderr, "searchBMUnicode: patrón o texto NULL\n");
        return;
    }
    size_t M, N;
    uint32_t *pat = decodeUTF8(patternUTF8, &M);
    uint32_t *txt = decodeUTF8(textUTF8, &N);
    if (!pat || !txt) {
        free(pat); free(txt);
        fprintf(stderr, "searchBMUnicode: fallo al decodificar UTF-8\n");
        return;
    }
    if (M == 0 || N == 0 || M > N) {
        free(pat); free(txt);
        return;
    }

    //bad-character dinamico
    size_t mapSize;
    BMMapEntry *bmMap = preprocessBadCharUnicode(pat, M, &mapSize);
    if (!bmMap) { free(pat); free(txt); return; }

    //good-suffix
    size_t *shiftGS = malloc((M+1)*sizeof(size_t));
    if (!shiftGS) { free(pat); free(txt); free(bmMap); return; }
    preprocessGoodSuffixUnicode(pat, M, shiftGS);

    //busqueda en code-points
    for (size_t s = 0; s <= N - M; ) {
        ptrdiff_t j = (ptrdiff_t)M - 1;
        //comparar desde final
        while (j >= 0 && pat[j] == txt[s + j]) j--;
        if (j < 0) {
            printf("Patrón encontrado en posición %zu (byte-offset aproximado)\n", s);
            s += shiftGS[0];
        } else {
            //buscar ultima aparicion de txt[s+j]
            uint32_t c = txt[s + j];
            ptrdiff_t last = -1;
            for (size_t k = 0; k < mapSize; k++) {
                if (bmMap[k].cp == c) { last = bmMap[k].last; break; }
            }
            size_t bcShift = (size_t)(j - (last < 0 ? -1 : last));
            size_t gsShift = shiftGS[j + 1];
            s += (bcShift > gsShift ? bcShift : gsShift);
        }
    }

    free(pat);
    free(txt);
    free(bmMap);
    free(shiftGS);
}