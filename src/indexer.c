// Diego Galindo, Francisco Mercado
#include "indexer.h"
#include "index_operations.h"
#include "persistence.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef DT_REG
#define DT_REG 8
#endif

// Función hash simple para strings
uint32_t hash_function(const char *str, size_t table_size) {
    uint32_t hash = 5381;
    for (size_t i = 0; str[i]; i++) {
        hash = ((hash << 5) + hash) + (unsigned char)str[i];
    }
    return hash % table_size;
}

// Crear un nuevo índice invertido
InvertedIndex* createIndex(size_t initial_capacity) {
    if (initial_capacity == 0) initial_capacity = 1024;
    
    InvertedIndex *index = malloc(sizeof(InvertedIndex));
    if (!index) return NULL;
    
    index->entries = calloc(initial_capacity, sizeof(IndexEntry));
    if (!index->entries) {
        free(index);
        return NULL;
    }
    
    index->size = 0;
    index->capacity = initial_capacity;
    index->next_doc_id = 1;
    
    return index;
}

// Destruir el índice y liberar memoria
void destroyIndex(InvertedIndex *index) {
    if (!index) return;
    
    for (size_t i = 0; i < index->capacity; i++) {
        if (index->entries[i].term) {
            free(index->entries[i].term);
            
            PostingNode *current = index->entries[i].head;
            while (current) {
                PostingNode *next = current->next;
                free(current->posting.positions);
                free(current);
                current = next;
            }
        }
    }
    
    free(index->entries);
    free(index);
}

// Tokenizar texto en palabras
char** tokenize(const char *text, size_t *token_count) {
    if (!text || !token_count) return NULL;
    
    size_t len = strlen(text);
    if (len == 0) {
        *token_count = 0;
        return NULL;
    }
    
    // Crear una copia del texto para modificar
    char *text_copy = malloc(len + 1);
    if (!text_copy) return NULL;
    strcpy(text_copy, text);
    
    // Contar tokens aproximadamente
    size_t estimated_tokens = 1;
    for (size_t i = 0; text_copy[i]; i++) {
        if (isspace((unsigned char)text_copy[i]) || ispunct((unsigned char)text_copy[i])) {
            estimated_tokens++;
        }
    }
    
    char **tokens = malloc(estimated_tokens * sizeof(char*));
    if (!tokens) {
        free(text_copy);
        return NULL;
    }
    
    size_t count = 0;
    char *token = strtok(text_copy, " \t\n\r\f\v.,;:!?()[]{}\"'");
    
    while (token && count < estimated_tokens) {
        // Filtrar tokens muy cortos o que no son palabras
        if (strlen(token) >= 2) {
            // Verificar que el token contiene al menos una letra
            int has_letter = 0;
            for (size_t i = 0; token[i]; i++) {
                if (isalpha((unsigned char)token[i])) {
                    has_letter = 1;
                    break;
                }
            }
            
            if (has_letter) {
                tokens[count] = malloc(strlen(token) + 1);
                if (tokens[count]) {
                    strcpy(tokens[count], token);
                    // Normalizar el token
                    convertir_a_minusculas(tokens[count]);
                    limpiar_palabra(tokens[count]);
                    count++;
                }
            }
        }
        token = strtok(NULL, " \t\n\r\f\v.,;:!?()[]{}\"'");
    }
    
    free(text_copy);
    *token_count = count;
    return tokens;
}

// Liberar array de tokens
void freeTokens(char **tokens, size_t count) {
    if (!tokens) return;
    for (size_t i = 0; i < count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

// Añadir un término al índice
void addTermToIndex(InvertedIndex *index, const char *term, uint32_t doc_id, size_t position) {
    if (!index || !term || strlen(term) == 0) return;
    
    uint32_t hash = hash_function(term, index->capacity);
    size_t original_hash = hash;
    
    // Resolución de colisiones por sondeo lineal
    while (index->entries[hash].term) {
        if (strcmp(index->entries[hash].term, term) == 0) {
            // Término ya existe, buscar el documento en la lista de postings
            PostingNode *current = index->entries[hash].head;
            PostingNode *prev = NULL;
            
            while (current && current->posting.doc_id < doc_id) {
                prev = current;
                current = current->next;
            }
            
            if (current && current->posting.doc_id == doc_id) {
                // Documento ya existe, añadir posición
                if (current->posting.position_count >= current->posting.position_cap) {
                    current->posting.position_cap *= 2;
                    current->posting.positions = realloc(current->posting.positions,
                        current->posting.position_cap * sizeof(size_t));
                }
                current->posting.positions[current->posting.position_count++] = position;
            } else {
                // Nuevo documento para este término
                PostingNode *new_node = malloc(sizeof(PostingNode));
                if (!new_node) return;
                
                new_node->posting.doc_id = doc_id;
                new_node->posting.position_cap = 4;
                new_node->posting.positions = malloc(4 * sizeof(size_t));
                if (!new_node->posting.positions) {
                    free(new_node);
                    return;
                }
                new_node->posting.positions[0] = position;
                new_node->posting.position_count = 1;
                
                // Insertar en orden por doc_id
                if (!prev) {
                    new_node->next = index->entries[hash].head;
                    index->entries[hash].head = new_node;
                } else {
                    new_node->next = current;
                    prev->next = new_node;
                }
                
                index->entries[hash].doc_frequency++;
            }
            return;
        }
        
        hash = (hash + 1) % index->capacity;
        if (hash == original_hash) {
            // Tabla llena, necesita redimensionar
            fprintf(stderr, "Warning: Hash table full\n");
            return;
        }
    }
    
    // Nuevo término
    index->entries[hash].term = malloc(strlen(term) + 1);
    if (!index->entries[hash].term) return;
    strcpy(index->entries[hash].term, term);
    
    PostingNode *new_node = malloc(sizeof(PostingNode));
    if (!new_node) {
        free(index->entries[hash].term);
        index->entries[hash].term = NULL;
        return;
    }
    
    new_node->posting.doc_id = doc_id;
    new_node->posting.position_cap = 4;
    new_node->posting.positions = malloc(4 * sizeof(size_t));
    if (!new_node->posting.positions) {
        free(new_node);
        free(index->entries[hash].term);
        index->entries[hash].term = NULL;
        return;
    }
    new_node->posting.positions[0] = position;
    new_node->posting.position_count = 1;
    new_node->next = NULL;
    
    index->entries[hash].head = new_node;
    index->entries[hash].doc_frequency = 1;
    index->size++;
}

// Añadir un documento al índice
uint32_t addDocument(InvertedIndex *index, DocumentCollection *collection, 
                     const char *filename, const char *content, const char *title) {
    if (!index || !collection || !filename || !content) return 0;
    
    uint32_t doc_id = index->next_doc_id++;
    
    // Añadir documento a la colección
    if (collection->count >= collection->capacity) {
        collection->capacity *= 2;
        collection->docs = realloc(collection->docs, 
            collection->capacity * sizeof(DocumentInfo));
        if (!collection->docs) return 0;
    }
    
    DocumentInfo *doc = &collection->docs[collection->count];
    doc->doc_id = doc_id;
    doc->filename = malloc(strlen(filename) + 1);
    if (!doc->filename) return 0;
    strcpy(doc->filename, filename);
    
    if (title) {
        doc->title = malloc(strlen(title) + 1);
        if (doc->title) strcpy(doc->title, title);
    } else {
        doc->title = NULL;
    }
    
    collection->count++;
    
    // Tokenizar y añadir al índice
    size_t token_count;
    char **tokens = tokenize(content, &token_count);
    if (!tokens) return doc_id;
    
    doc->word_count = token_count;
    
    for (size_t i = 0; i < token_count; i++) {
        if (tokens[i] && strlen(tokens[i]) > 0) {
            addTermToIndex(index, tokens[i], doc_id, i);
        }
    }
    
    freeTokens(tokens, token_count);
    return doc_id;
}

// Buscar un término en el índice
PostingNode* searchTerm(InvertedIndex *index, const char *term) {
    if (!index || !term) return NULL;
    
    // Normalizar término de búsqueda
    char *normalized_term = malloc(strlen(term) + 1);
    if (!normalized_term) return NULL;
    strcpy(normalized_term, term);
    convertir_a_minusculas(normalized_term);
    limpiar_palabra(normalized_term);
    
    uint32_t hash = hash_function(normalized_term, index->capacity);
    size_t original_hash = hash;
    
    while (index->entries[hash].term) {
        if (strcmp(index->entries[hash].term, normalized_term) == 0) {
            free(normalized_term);
            return index->entries[hash].head;
        }
        
        hash = (hash + 1) % index->capacity;
        if (hash == original_hash) break;
    }
    
    free(normalized_term);
    return NULL;
}

DocumentCollection* createDocumentCollection(size_t initial_capacity) {
    if (initial_capacity == 0) initial_capacity = 64;
    
    DocumentCollection *collection = malloc(sizeof(DocumentCollection));
    if (!collection) return NULL;
    
    collection->docs = malloc(initial_capacity * sizeof(DocumentInfo));
    if (!collection->docs) {
        free(collection);
        return NULL;
    }
    
    collection->count = 0;
    collection->capacity = initial_capacity;
    
    return collection;
}

void destroyDocumentCollection(DocumentCollection *collection) {
    if (!collection) return;
    
    for (size_t i = 0; i < collection->count; i++) {
        free(collection->docs[i].filename);
        free(collection->docs[i].title);
    }
    
    free(collection->docs);
    free(collection);
}

DocumentInfo* getDocumentById(DocumentCollection *collection, uint32_t doc_id) {
    if (!collection) return NULL;
    
    for (size_t i = 0; i < collection->count; i++) {
        if (collection->docs[i].doc_id == doc_id) {
            return &collection->docs[i];
        }
    }
    
    return NULL;
}

// Función para determinar si una ruta es directorio
int isDirectory(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) return 0;
    return S_ISDIR(statbuf.st_mode);
}

// Actualizar índice con nuevos documentos
int updateIndex(const char* index_file, const char* new_docs) {
    // Construir ruta completa
    char* full_index_path = buildIndexPath(index_file);
    char* full_docs_path = buildDocsPath(new_docs);
    if (!full_index_path || !full_docs_path) {
        fprintf(stderr, "Error construyendo rutas necesarias.\n");
        return EXIT_FAILURE;
    }
    
    // Cargar índice existente
    InvertedIndex* index = NULL;
    DocumentCollection* collection = NULL;
    if (loadIndexFromBinary(&index, &collection, full_index_path) != 0) {
        fprintf(stderr, "Error cargando índice existente\n");
        free(full_index_path);
        return EXIT_FAILURE;
    }
    
    // Procesar nuevos documentos
    int files_added = 0;
    
    if (isDirectory(new_docs)) {
        // Procesar directorio completo
        DIR* dir = opendir(new_docs);
        if (!dir) {
            perror("Error abriendo directorio");
            free(full_index_path);
            destroyIndex(index);
            destroyDocumentCollection(collection);
            return EXIT_FAILURE;
        }
        
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG && isTextFile(entry->d_name)) {
                char filepath[1024];
                snprintf(filepath, sizeof(filepath), "%s/%s", new_docs, entry->d_name);
                
                // Procesar archivo individual
                if (processSingleFile(index, collection, filepath)) {
                    files_added++;
                }
            }
        }
        closedir(dir);
    } else {
        // Procesar archivo individual
        files_added = processSingleFile(index, collection, new_docs) ? 1 : 0;
    }
    
    // Guardar índice actualizado
    if (files_added > 0) {
        if (saveIndexToBinary(index, collection, full_index_path) != 0) {
            fprintf(stderr, "Error guardando índice actualizado\n");
        } else {
            printf("Índice actualizado. Archivos añadidos: %d\n", files_added);
        }
    }
    
    // Limpieza
    destroyIndex(index);
    destroyDocumentCollection(collection);
    free(full_index_path);
    free(full_docs_path);
    return EXIT_SUCCESS;
}

// Función auxiliar para procesar un archivo
int processSingleFile(InvertedIndex* index, DocumentCollection* collection, const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "Advertencia: No se pudo abrir %s\n", filepath);
        return 0;
    }
    
    // Verificar si el archivo existe
    if (access(filepath, F_OK) == -1) {
        // Intentar con docs/
        char* docs_path = buildDocsPath(filepath);
        if (access(docs_path, F_OK) != -1) {
            filepath = docs_path;  // Usar la ruta en docs/
        } else {
            fprintf(stderr, "Archivo no encontrado: %s\n", filepath);
            free(docs_path);
            return 0;
        }
        free(docs_path);
    }

    // Leer contenido
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        return 0;
    }
    
    size_t bytes_read = fread(content, 1, file_size, file);
    content[bytes_read] = '\0';
    
    // Conversión explícita de tipos
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Advertencia: Lectura incompleta en %s (%zu/%ld bytes)\n", 
                filepath, bytes_read, file_size);
    }
    
    fclose(file);
    
    // Obtener nombre base del archivo
    const char* filename = strrchr(filepath, '/');
    if (!filename) filename = strrchr(filepath, '\\');
    if (filename) filename++;
    else filename = filepath;
    
    // Añadir documento
    uint32_t doc_id = addDocument(index, collection, filepath, content, filename);
    free(content);
    
    return (doc_id != 0) ? 1 : 0;
}