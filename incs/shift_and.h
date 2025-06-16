// Diego Galindo, Francisco Mercado
#ifndef SHIFT_AND_H
#define SHIFT_AND_H

#include <stddef.h>

//construye la mascara de bits para cada caracter del patron
void buildMask(const char *pat, unsigned long long masks[256]);

//busca todas las ocurrencias con shift-and (solo patrones <= 64)
//imprime cada posicion encontrada
void searchShiftAnd(const char *pattern, const char *text);

#endif