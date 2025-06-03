#ifndef BOYER_MOORE_H
#define BOYER_MOORE_H

#include <stddef.h>

/**
 * preprocessbadchar construye la tabla de bad‐character para boyer-moore
 * @param pat patron de busqueda (nulo-terminado)
 * @param badChar arreglo de tamaño 256 para almacenar saltos
 */
void preprocessBadChar(const char* pat, int badChar[256]);

/**
 * searchboyermoore busca patron en texto usando boyer-moore
 * @param pattern patron de busqueda (nulo-terminado)
 * @param text texto donde buscar (nulo-terminado)
 */
void searchBoyerMoore(const char* pattern, const char* text);

#endif