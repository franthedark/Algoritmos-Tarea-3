// Diego Galindo, Francisco Mercado
#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <stdbool.h>

//imprime un header de tabla con ncols columnas
void printTableHeader(const char **cols, int nCols);

//imprime una fila de tabla con ncols celdas
void printTableRow(const char **cells, int nCols);

void printTableFooter(int nCols);

//imprime un match resaltado en verde si stdout es tty
void printMatch(size_t position, const char *algorithm);

//imprime un mensaje de error en rojo si stdout es tty
void printError(const char *msg);

#endif