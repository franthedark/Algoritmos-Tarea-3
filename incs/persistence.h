#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "indexer.h"

// Constantes para el formato del archivo
#define INDEX_FILE_MAGIC 0x494E4458  // "INDX" en little endian
#define INDEX_FILE_VERSION 1

// Estructura del header del archivo binario
typedef struct {
    uint32_t magic;           // Número mágico para validar el archivo
    uint32_t version;         // Versión del formato
    uint32_t num_terms;       // Número de términos en el índice
    uint32_t num_documents;   // Número de documentos
    uint32_t next_doc_id;     // Próximo ID de documento
    uint64_t checksum;        // Checksum simple para integridad
} IndexFileHeader;

// Estructura para serializar términos
typedef struct {
    uint32_t term_length;     // Longitud del término (incluyendo \0)
    uint32_t doc_frequency;   // Número de documentos que contienen el término
    uint32_t posting_count;   // Número de postings para este término
} TermHeader;

// Estructura para serializar postings
typedef struct {
    uint32_t doc_id;          // ID del documento
    uint32_t position_count;  // Número de posiciones
} PostingHeader;

// Estructura para serializar información de documentos
typedef struct {
    uint32_t doc_id;          // ID del documento
    uint32_t filename_length; // Longitud del nombre del archivo
    uint32_t title_length;    // Longitud del título (0 si no hay título)
    uint64_t word_count;      // Número de palabras en el documento
} DocumentHeader;

// Funciones principales de persistencia
int saveIndexToBinary(const InvertedIndex *index, const DocumentCollection *collection, 
                      const char *filename);
int loadIndexFromBinary(InvertedIndex **index, DocumentCollection **collection, 
                        const char *filename);

int saveIndexToJSON(const InvertedIndex *index, const DocumentCollection *collection, 
                    const char *filename);
int loadIndexFromJSON(InvertedIndex **index, DocumentCollection **collection, 
                      const char *filename);

// Funciones auxiliares
uint64_t calculateChecksum(const void *data, size_t size);
int validateIndexFile(const char *filename);
void printIndexFileInfo(const char *filename);

// Funciones de backup y recuperación
int createIndexBackup(const InvertedIndex *index, const DocumentCollection *collection, 
                      const char *backup_dir);
int restoreIndexFromBackup(InvertedIndex **index, DocumentCollection **collection, 
                           const char *backup_file);

// Funciones de exportación
int exportIndexToText(const InvertedIndex *index, const DocumentCollection *collection, 
                      const char *filename);
int exportTermStatistics(const InvertedIndex *index, const char *filename);

#endif // PERSISTENCE_H