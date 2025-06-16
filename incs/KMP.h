// Diego Galindo, Francisco Mercado
#ifndef KMP_H
#define KMP_H

#include <stddef.h>

//preprocesa la función lps (fallback) para kmp
void computeLPSArray(const char *pat, size_t M, int *lps);

//busqueda KMP clasica (lps)
void searchKMP(const char *pattern, const char *text);

//construye el autómata determinista para kmp
//dfa debe apuntar a un bloque de int de tamaño (r * (m+1))
void buildDFA(const char *pat, size_t M, int R, int *dfa);

//busqueda usando el dfa precalculado
void searchKMP_DFA(const char *pattern, const char *text);

#endif