#ifndef SHIFT_AND_H
#define SHIFT_AND_H

#include <stddef.h>

/**
 * buildmask construye el bit‐mask para cada caracter de pattern
 * @param pat patrón de busqueda (nulo-terminado)
 * @param masks arreglo donde masks[c] es la mascara de bits para el caracter c
 */
void buildMask(const char* pat, unsigned long long masks[256]);

/**
 * searchshiftand busca patron en texto usando shift-and
 * @param pattern patron de busqueda (nulo-terminado longitud <= 64)
 * @param text texto donde buscar (nulo-terminado)
 */
void searchShiftAnd(const char* pattern, const char* text);

#endif