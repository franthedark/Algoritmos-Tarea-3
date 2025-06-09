#ifndef KMP_H
#define KMP_H

#include <stddef.h>

//construye la tabla lps (longest prefix suffix) para el patron
void computeLPSArray(const char *pat, size_t M, int *lps);

//busca todas las ocurrencias de un patron en un texto usando kmp
void searchKMP(const char *pattern, const char *text);

#endif