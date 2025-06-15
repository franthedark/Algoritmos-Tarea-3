#ifndef INDEXER_H
#define INDEXER_H

#include <stdint.h>
#include <stddef.h>

// Estructura para almacenar las posiciones de un término en un documento
typedef struct {
    uint32_t doc_id;
    size_t *positions;      // Array de posiciones donde aparece el término
    size_t position_count;  // Número de posiciones
    size_t position_cap;    // Capacidad del array de posiciones
} PostingList;

// Nodo de la lista enlazada de postings para un término
typedef struct PostingNode {
    PostingList posting;
    struct PostingNode *next;
} PostingNode;

// Entrada en la tabla hash del índice invertido
typedef struct {
    char *term;            // El término (clave)
    PostingNode *head;     // Lista enlazada de postings
    uint32_t doc_frequency; // Número de documentos que contienen este término
} IndexEntry;

// Estructura principal del índice invertido
typedef struct {
    IndexEntry *entries;   // Tabla hash
    size_t size;          // Tamaño actual (número de términos únicos)
    size_t capacity;      // Capacidad de la tabla hash
    uint32_t next_doc_id; // Próximo ID de documento a asignar
} InvertedIndex;

// Información de un documento indexado
typedef struct {
    uint32_t doc_id;
    char *filename;
    size_t word_count;    // Número total de palabras en el documento
    char *title;          // Título del documento (opcional)
} DocumentInfo;

// Colección de documentos indexados
typedef struct {
    DocumentInfo *docs;
    size_t count;
    size_t capacity;
} DocumentCollection;

// Funciones principales del indexer
InvertedIndex* createIndex(size_t initial_capacity);
void destroyIndex(InvertedIndex *index);

// Tokenización
char** tokenize(const char *text, size_t *token_count);
void freeTokens(char **tokens, size_t count);

// Indexación
uint32_t addDocument(InvertedIndex *index, DocumentCollection *collection, 
                     const char *filename, const char *content, const char *title);
void addTermToIndex(InvertedIndex *index, const char *term, uint32_t doc_id, size_t position);

// Búsqueda
PostingNode* searchTerm(InvertedIndex *index, const char *term);

// Utilidades
uint32_t hash_function(const char *str, size_t table_size);

// Gestión de documentos
DocumentCollection* createDocumentCollection(size_t initial_capacity);
void destroyDocumentCollection(DocumentCollection *collection);
DocumentInfo* getDocumentById(DocumentCollection *collection, uint32_t doc_id);

#endif