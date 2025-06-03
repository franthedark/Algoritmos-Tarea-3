#ifndef KMP_H
#define KMP_H

#include <stddef.h>

/**
 * computelpsarray construye la tabla lps (longest prefix suffix) para el patron
 * @param pat patrón de busqueda (nulo-terminado)
 * @param M longitud del patron
 * @param lps arreglo prealocado de tamaño m para almacenar los valores lps
 */
void computeLPSArray(const char* pat, size_t M, int* lps);

/**
 * searchkmp busca todas las ocurrencias de pattern en text usando KMP
 * @param pattern patron de busqueda (nulo-terminado)
 * @param text texto donde buscar (nulo-terminado)
 * imprime en stdout cada posicion donde se encontro el patron
 */
void searchKMP(const char* pattern, const char* text);

#endif