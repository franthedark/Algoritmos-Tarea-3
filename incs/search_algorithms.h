#ifndef SEARCH_ALGORITHMS_H
#define SEARCH_ALGORITHMS_H

#include <stddef.h>

/**
 * searchkmp busca patron en texto usando kmp
 * @param pattern patron de busqueda (nulo-terminado)
 * @param text texto donde buscar (nulo-terminado)
 */
void searchKMP(const char* pattern, const char* text);

/**
 * searchboyermoore busca patron en texto usando boyer-moore
 * @param pattern patron de busqueda (nulo-terminado)
 * @param text texto donde buscar (nulo-terminado)
 */
void searchBoyerMoore(const char* pattern, const char* text);

/**
 * searchshiftand busca patron en texto usando shift-and
 * @param pattern patron de busqueda (nulo-terminado)
 * @param text texto donde buscar (nulo-terminado)
 */
void searchShiftAnd(const char* pattern, const char* text);

#endif