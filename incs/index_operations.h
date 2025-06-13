#ifndef INDEX_OPERATIONS_H
#define INDEX_OPERATIONS_H
// Este módulo contiene todas las funciones relacionadas con la gestión de índices: creación, búsqueda, información, exportación y backup.

// Imprime información de uso para los comandos de índice
void printIndexUsage(const char* program_name);

// Verifica si un archivo es de texto basándose en su extensión
int isTextFile(const char* filename);

// Crea un índice a partir de un directorio
int indexDirectory(const char* dir_path, const char* index_file);

int searchInIndex(const char* index_file, const char* term);
int handleIndexCommands(int argc, char* argv[]);

#endif