// Diego Galindo, Francisco Mercado
#ifndef BOYER_MOORE_H
#define BOYER_MOORE_H

#include <stddef.h>
#include <stdint.h>

//decodifica utf-8 en un array de code-points (liberar con free)
uint32_t* decodeUTF8(const char *utf8, size_t *outLen);

//bad-character mapa dinámico de code-points a ultimo indice
typedef struct {
    uint32_t cp;    //code-point Unicode
    ptrdiff_t last; //ultima posicion en el patron
} BMMapEntry;

//construye el mapa bad-character para el patron unicode
BMMapEntry* preprocessBadCharUnicode(const uint32_t *pat, size_t M, size_t *mapSize);

//good-suffix sobre code-points
void preprocessGoodSuffixUnicode(const uint32_t *pat, size_t M, size_t *shiftGS);

//busqueda boyer–moore UTF-8
void searchBoyerMooreUnicode(const char *patternUTF8, const char *textUTF8);

#endif