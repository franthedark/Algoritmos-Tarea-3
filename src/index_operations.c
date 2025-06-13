#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "index_operations.h"
#include "search_algorithms.h"
#include "utils.h"
#include "indexer.h"
#include "persistence.h"

// Definir DT_REG si no está disponible
#ifndef DT_REG
#define DT_REG 8
#endif

// Función para crear directorio si no existe
int createDirectoryIfNotExists(const char* dir_path) {
    struct stat st = {0};
    if (stat(dir_path, &st) == -1) {
        #ifdef _WIN32
        return _mkdir(dir_path);
        #else
        return mkdir(dir_path, 0755);
        #endif
    }
    return 0; // Ya existe
}

// Función para construir la ruta completa del archivo de índice
char* buildIndexPath(const char* index_file) {
    const char* indices_dir = "indexes";
    
    // Crear directorio indices si no existe
    if (createDirectoryIfNotExists(indices_dir) != 0) {
        fprintf(stderr, "Advertencia: No se pudo crear el directorio 'indices'\n");
    }
    
    // Si el archivo ya incluye una ruta, úsalo tal como está
    if (strchr(index_file, '/') != NULL || strchr(index_file, '\\') != NULL) {
        char* full_path = malloc(strlen(index_file) + 1);
        if (full_path) {
            strcpy(full_path, index_file);
        }
        return full_path;
    }
    
    // Construir ruta completa: indices/archivo.idx
    size_t path_len = strlen(indices_dir) + strlen(index_file) + 2; // +2 para '/' y '\0'
    char* full_path = malloc(path_len);
    
    if (full_path) {
        snprintf(full_path, path_len, "%s/%s", indices_dir, index_file);
    }
    
    return full_path;
}

void printIndexUsage(const char* program_name) {
    fprintf(stderr,
        "Gestión de índices:\n"
        "  %s index create <directorio> [archivo_indice.idx]\n"
        "  %s index search <archivo_indice.idx> <término>\n"
        "  %s index info <archivo_indice.idx>\n"
        "  %s index export <archivo_indice.idx> <archivo_salida.txt>\n"
        "  %s index backup <archivo_indice.idx> <directorio_backup>\n"
        "\n"
        "Nota: Los archivos .idx se guardan automáticamente en la carpeta 'indices/'\n"
        "\n"
        "Ejemplos:\n"
        "  %s index create ./docs index.idx\n"
        "  %s index search index.idx \"término\"\n",
        program_name, program_name, program_name, 
        program_name, program_name, program_name, program_name
    );
}

int isTextFile(const char* filename) {
    const char* extensions[] = {
        ".txt", ".html", ".htm", ".md", ".c", ".h", 
        ".cpp", ".py", ".js", NULL
    };
    
    size_t filename_len = strlen(filename);
    
    for (int i = 0; extensions[i] != NULL; i++) {
        size_t ext_len = strlen(extensions[i]);
        if (filename_len >= ext_len && 
            strcmp(filename + filename_len - ext_len, extensions[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int indexDirectory(const char* dir_path, const char* index_file) {
    printf("Creando índice desde directorio: %s\n", dir_path);
    
    // Construir ruta completa para el archivo de índice
    char* full_index_path = buildIndexPath(index_file);
    if (!full_index_path) {
        fprintf(stderr, "Error: No se pudo construir la ruta del índice\n");
        return EXIT_FAILURE;
    }
    
    printf("Archivo de índice se guardará en: %s\n", full_index_path);
    
    InvertedIndex* index = createIndex(10000);
    DocumentCollection* collection = createDocumentCollection(1000);
    
    if (!index || !collection) {
        fprintf(stderr, "Error: No se pudo crear el índice o la colección\n");
        free(full_index_path);
        return EXIT_FAILURE;
    }
    
    DIR* dir = opendir(dir_path);
    if (!dir) {
        perror("Error al abrir directorio");
        destroyIndex(index);
        destroyDocumentCollection(collection);
        free(full_index_path);
        return EXIT_FAILURE;
    }
    
    struct dirent* entry;
    int files_processed = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && isTextFile(entry->d_name)) {
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
            
            printf("Procesando: %s\n", filepath);
            
            // Read file content first
            FILE* file = fopen(filepath, "r");
            if (!file) {
                fprintf(stderr, "Advertencia: No se pudo abrir %s\n", filepath);
                continue;
            }
            
            // Get file size
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            // Read content
            char* content = malloc(file_size + 1);
            if (!content) {
                fclose(file);
                fprintf(stderr, "Advertencia: Error de memoria para %s\n", filepath);
                continue;
            }
            
            size_t read_size = fread(content, 1, file_size, file);
            content[read_size] = '\0';
            fclose(file);
            
            // Fix: addDocument needs 5 parameters (index, collection, filepath, content, title)
            uint32_t doc_id = addDocument(index, collection, filepath, content, entry->d_name);
            free(content);
            
            if (doc_id != 0) {
                files_processed++;
            } else {
                fprintf(stderr, "Advertencia: No se pudo procesar %s\n", filepath);
            }
        }
    }
    
    closedir(dir);
    
    if (files_processed == 0) {
        printf("No se encontraron archivos de texto para indexar\n");
        destroyIndex(index);
        destroyDocumentCollection(collection);
        free(full_index_path);
        return EXIT_FAILURE;
    }
    
    printf("\nArchivos procesados: %d\n", files_processed);
    printf("Términos únicos: %zu\n", index->size);
    
    // Guardar índice usando la ruta completa
    if (saveIndexToBinary(index, collection, full_index_path) != 0) {
        fprintf(stderr, "Error al guardar el índice\n");
        destroyIndex(index);
        destroyDocumentCollection(collection);
        free(full_index_path);
        return EXIT_FAILURE;
    }
    
    destroyIndex(index);
    destroyDocumentCollection(collection);
    
    printf("Índice creado exitosamente: %s\n", full_index_path);
    free(full_index_path);
    return EXIT_SUCCESS;
}

int searchInIndex(const char* index_file, const char* term) {
    // Construir ruta completa para buscar el archivo de índice
    char* full_index_path = buildIndexPath(index_file);
    if (!full_index_path) {
        fprintf(stderr, "Error: No se pudo construir la ruta del índice\n");
        return EXIT_FAILURE;
    }
    
    InvertedIndex* index = NULL;
    DocumentCollection* collection = NULL;
    
    printf("Cargando índice: %s\n", full_index_path);
    
    if (loadIndexFromBinary(&index, &collection, full_index_path) != 0) {
        fprintf(stderr, "Error al cargar el índice desde: %s\n", full_index_path);
        free(full_index_path);
        return EXIT_FAILURE;
    }
    
    free(full_index_path);
    
    printf("Buscando término: \"%s\"\n\n", term);
    
    // Normalizar término de búsqueda
    char* normalized_term = malloc(strlen(term) + 1);
    if (!normalized_term) {
        fprintf(stderr, "Error de memoria\n");
        destroyIndex(index);
        destroyDocumentCollection(collection);
        return EXIT_FAILURE;
    }
    
    strcpy(normalized_term, term);
    convertir_a_minusculas(normalized_term);
    limpiar_palabra(normalized_term);
    
    // Buscar en el índice
    PostingNode* results = searchTerm(index, normalized_term);
    
    if (!results) {
        printf("No se encontraron resultados para: \"%s\"\n", term);
    } else {
        printf("=== Resultados de búsqueda ===\n");
        printf("Término: \"%s\" (normalizado: \"%s\")\n", term, normalized_term);
        printf("Documentos encontrados:\n\n");
        
        PostingNode* current = results;
        int doc_count = 0;
        
        while (current) {
            doc_count++;
            printf("Documento %d:\n", doc_count);
            
            // Access PostingList members directly (posting is not a pointer)
            printf("  ID: %u\n", current->posting.doc_id);
            
            // Buscar información del documento en la colección
            for (size_t i = 0; i < collection->count; i++) {
                if (collection->docs && collection->docs[i].doc_id == current->posting.doc_id) {
                    printf("  Archivo: %s\n", collection->docs[i].filename);
                    if (collection->docs[i].title) {
                        printf("  Título: %s\n", collection->docs[i].title);
                    }
                    break;
                }
            }
            
            printf("  Ocurrencias: %zu\n", current->posting.position_count);
            
            printf("  Posiciones: ");
            for (size_t j = 0; j < current->posting.position_count && j < 10; j++) {
                if (current->posting.positions) {
                    printf("%zu", current->posting.positions[j]);
                    if (j < current->posting.position_count - 1 && j < 9) printf(", ");
                }
            }
            if (current->posting.position_count > 10) {
                printf(" ... (%zu más)", current->posting.position_count - 10);
            }
            printf("\n\n");
            
            current = current->next;
        }
    }
    
    free(normalized_term);
    // Note: PostingNode is typically managed by the index, so we don't free it here
    destroyIndex(index);
    destroyDocumentCollection(collection);
    
    return EXIT_SUCCESS;
}

int handleIndexCommands(int argc, char* argv[]) {
    if (argc < 3) {
        printIndexUsage(argv[0]);
        return EXIT_FAILURE;
    }
    
    const char* command = argv[2];
    
    if (strcmp(command, "create") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Error: Falta directorio para indexar\n");
            printIndexUsage(argv[0]);
            return EXIT_FAILURE;
        }
        
        const char* directory = argv[3];
        const char* index_file = (argc >= 5) ? argv[4] : "index.idx";
        
        return indexDirectory(directory, index_file);
        
    } else if (strcmp(command, "search") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Error: Faltan argumentos para búsqueda\n");
            printIndexUsage(argv[0]);
            return EXIT_FAILURE;
        }
        
        const char* index_file = argv[3];
        const char* term = argv[4];
        
        return searchInIndex(index_file, term);
        
    } else if (strcmp(command, "info") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Error: Falta archivo de índice\n");
            printIndexUsage(argv[0]);
            return EXIT_FAILURE;
        }
        
        const char* index_file = argv[3];
        char* full_index_path = buildIndexPath(index_file);
        
        if (!full_index_path) {
            fprintf(stderr, "Error: No se pudo construir la ruta del índice\n");
            return EXIT_FAILURE;
        }
        
        int result = EXIT_FAILURE;
        if (validateIndexFile(full_index_path) == 0) {
            printIndexFileInfo(full_index_path);
            result = EXIT_SUCCESS;
        }
        
        free(full_index_path);
        return result;
        
    } else if (strcmp(command, "export") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Error: Faltan argumentos para exportar\n");
            printIndexUsage(argv[0]);
            return EXIT_FAILURE;
        }
        
        const char* index_file = argv[3];
        const char* output_file = argv[4];
        
        char* full_index_path = buildIndexPath(index_file);
        if (!full_index_path) {
            fprintf(stderr, "Error: No se pudo construir la ruta del índice\n");
            return EXIT_FAILURE;
        }
        
        InvertedIndex* index = NULL;
        DocumentCollection* collection = NULL;
        
        if (loadIndexFromBinary(&index, &collection, full_index_path) != 0) {
            fprintf(stderr, "Error al cargar el índice desde: %s\n", full_index_path);
            free(full_index_path);
            return EXIT_FAILURE;
        }
        
        free(full_index_path);
        
        int result = exportIndexToText(index, collection, output_file);
        
        destroyIndex(index);
        destroyDocumentCollection(collection);
        
        return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
        
    } else if (strcmp(command, "backup") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Error: Faltan argumentos para backup\n");
            printIndexUsage(argv[0]);
            return EXIT_FAILURE;
        }
        
        const char* index_file = argv[3];
        const char* backup_dir = argv[4];
        
        char* full_index_path = buildIndexPath(index_file);
        if (!full_index_path) {
            fprintf(stderr, "Error: No se pudo construir la ruta del índice\n");
            return EXIT_FAILURE;
        }
        
        InvertedIndex* index = NULL;
        DocumentCollection* collection = NULL;
        
        if (loadIndexFromBinary(&index, &collection, full_index_path) != 0) {
            fprintf(stderr, "Error al cargar el índice desde: %s\n", full_index_path);
            free(full_index_path);
            return EXIT_FAILURE;
        }
        
        free(full_index_path);
        
        int result = createIndexBackup(index, collection, backup_dir);
        
        destroyIndex(index);
        destroyDocumentCollection(collection);
        
        return (result == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
        
    } else {
        fprintf(stderr, "Comando de índice no reconocido: %s\n", command);
        printIndexUsage(argv[0]);
        return EXIT_FAILURE;
    }
}