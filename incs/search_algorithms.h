// Diego Galindo, Francisco Mercado
#ifndef SEARCH_ALGORITHMS_H
#define SEARCH_ALGORITHMS_H

#include <stddef.h>

// busca patron en texto usando kmp
void searchKMP(const char* pattern, const char* text);

//  busca patron en texto usando boyer-moore
void searchBoyerMoore(const char* pattern, const char* text);

// busca patron en texto usando shift-and
void searchShiftAnd(const char* pattern, const char* text);

#endif