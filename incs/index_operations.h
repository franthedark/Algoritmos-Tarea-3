#ifndef INDEX_OPERATIONS_H
#define INDEX_OPERATIONS_H

/**
 * @file index_operations.h
 * @brief Operaciones de gestión de índices
 * 
 * Este módulo contiene todas las funciones relacionadas con la gestión
 * de índices: creación, búsqueda, información, exportación y backup.
 */

/**
 * Imprime información de uso para los comandos de índice
 * @param program_name Nombre del programa
 */
void printIndexUsage(const char* program_name);

/**
 * Verifica si un archivo es de texto basándose en su extensión
 * @param filename Nombre del archivo
 * @return 1 si es archivo de texto, 0 en caso contrario
 */
int isTextFile(const char* filename);

/**
 * Crea un índice a partir de un directorio
 * @param dir_path Ruta del directorio a indexar
 * @param index_file Archivo donde guardar el índice
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
int indexDirectory(const char* dir_path, const char* index_file);

/**
 * Busca un término en un índice existente
 * @param index_file Archivo del índice
 * @param term Término a buscar
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
int searchInIndex(const char* index_file, const char* term);

/**
 * Maneja todos los comandos relacionados con índices
 * @param argc Número de argumentos
 * @param argv Array de argumentos
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
int handleIndexCommands(int argc, char* argv[]);

#endif // INDEX_OPERATIONS_H