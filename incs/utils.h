#ifndef UTILS_H
#define UTILS_H

/**
 * loadfile carga el contenido de un archivo en memoria
 * @param filename ruta del archivo
 * @return buffer nulo-terminado con el contenido o null si falla
 *         el caller debe liberar con free()
 */
char* loadFile(const char* filename);

/**
 * striphtml convierte html simple a texto plano
 * @param html cadena nul-terminada con contenido html
 * @return buffer recien asignado (malloc) con solo texto plano
 *         el caller debe liberar con free()
 */
char* stripHTML(const char* html);

/**
 * tolowerinplace convierte en minuscula todos los caracteres ascii
 * @param s: cadena nul-terminada que se modifica en sitio
 */
void toLowerInPlace(char* s);

/**
 * squeezespaces convierte secuencias de espacios (espacio tab newline) en un solo espacio
 * @param s: cadena nul-terminada que se modifica en sitio
 */
void squeezeSpaces(char* s);

#endif