// Diego Galindo, Francisco Mercado
#define _GNU_SOURCE  // For strdup function
#include "persistence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>


// Guardar índice en formato binario
int saveIndexToBinary(const InvertedIndex *index, const DocumentCollection *collection, 
                      const char *filename) {
    if (!index || !collection || !filename) return -1;
    
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("saveIndexToBinary: Error al abrir archivo");
        return -1;
    }
    
    // Escribir header
    IndexFileHeader header = {0};
    header.magic = INDEX_FILE_MAGIC;
    header.version = INDEX_FILE_VERSION;
    header.num_terms = (uint32_t)index->size;
    header.num_documents = (uint32_t)collection->count;
    header.next_doc_id = index->next_doc_id;
    header.checksum = 0; // Se calculará al final
    
    fwrite(&header, sizeof(IndexFileHeader), 1, file);
    
    // Escribir términos e índice invertido
    for (size_t i = 0; i < index->capacity; i++) {
        if (index->entries[i].term) {
            TermHeader term_header;
            term_header.term_length = (uint32_t)strlen(index->entries[i].term) + 1;
            term_header.doc_frequency = index->entries[i].doc_frequency;
            
            // Contar postings
            uint32_t posting_count = 0;
            PostingNode *current = index->entries[i].head;
            while (current) {
                posting_count++;
                current = current->next;
            }
            term_header.posting_count = posting_count;
            
            // Escribir header del término
            fwrite(&term_header, sizeof(TermHeader), 1, file);
            
            // Escribir el término
            fwrite(index->entries[i].term, term_header.term_length, 1, file);
            
            // Escribir postings
            current = index->entries[i].head;
            while (current) {
                PostingHeader posting_header;
                posting_header.doc_id = current->posting.doc_id;
                posting_header.position_count = (uint32_t)current->posting.position_count;
                
                fwrite(&posting_header, sizeof(PostingHeader), 1, file);
                fwrite(current->posting.positions, sizeof(size_t), 
                       current->posting.position_count, file);
                
                current = current->next;
            }
        }
    }
    
    // Escribir información de documentos
    for (size_t i = 0; i < collection->count; i++) {
        DocumentHeader doc_header;
        doc_header.doc_id = collection->docs[i].doc_id;
        doc_header.filename_length = (uint32_t)strlen(collection->docs[i].filename) + 1;
        doc_header.title_length = collection->docs[i].title ? 
                                  (uint32_t)strlen(collection->docs[i].title) + 1 : 0;
        doc_header.word_count = collection->docs[i].word_count;
        
        fwrite(&doc_header, sizeof(DocumentHeader), 1, file);
        fwrite(collection->docs[i].filename, doc_header.filename_length, 1, file);
        
        if (doc_header.title_length > 0) {
            fwrite(collection->docs[i].title, doc_header.title_length, 1, file);
        }
    }
    
    // Calcular y escribir checksum (simplificado)
    long file_size = ftell(file);
    header.checksum = (uint64_t)file_size; // Checksum simple basado en tamaño
    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(IndexFileHeader), 1, file);
    
    fclose(file);
    
    printf("Índice guardado en formato binario: %s\n", filename);
    printf("Términos: %u, Documentos: %u, Tamaño: %ld bytes\n", 
           header.num_terms, header.num_documents, file_size);
    
    return 0;
}

// Cargar índice desde formato binario
int loadIndexFromBinary(InvertedIndex **index, DocumentCollection **collection, 
                        const char *filename) {
    if (!index || !collection || !filename) return -1;
    
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("loadIndexFromBinary: Error al abrir archivo");
        return -1;
    }
    
    // Leer header
    IndexFileHeader header;
    if (fread(&header, sizeof(IndexFileHeader), 1, file) != 1) {
        fprintf(stderr, "Error al leer header del archivo\n");
        fclose(file);
        return -1;
    }
    
    // Validar formato
    if (header.magic != INDEX_FILE_MAGIC) {
        fprintf(stderr, "Archivo no es un índice válido (magic number incorrecto)\n");
        fclose(file);
        return -1;
    }
    
    if (header.version != INDEX_FILE_VERSION) {
        fprintf(stderr, "Versión del archivo no soportada: %u\n", header.version);
        fclose(file);
        return -1;
    }
    
    // Crear estructuras
    *index = createIndex(header.num_terms * 2); // Factor de carga del 50%
    if (!*index) {
        fclose(file);
        return -1;
    }
    
    *collection = createDocumentCollection(header.num_documents);
    if (!*collection) {
        destroyIndex(*index);
        fclose(file);
        return -1;
    }
    
    (*index)->next_doc_id = header.next_doc_id;
    
    // Leer términos
    for (uint32_t t = 0; t < header.num_terms; t++) {
        TermHeader term_header;
        if (fread(&term_header, sizeof(TermHeader), 1, file) != 1) {
            fprintf(stderr, "Error al leer header de término %u\n", t);
            goto error_cleanup;
        }
        
        char *term = malloc(term_header.term_length);
        if (!term || fread(term, term_header.term_length, 1, file) != 1) {
            free(term);
            fprintf(stderr, "Error al leer término %u\n", t);
            goto error_cleanup;
        }
        
        // Leer postings
        for (uint32_t p = 0; p < term_header.posting_count; p++) {
            PostingHeader posting_header;
            if (fread(&posting_header, sizeof(PostingHeader), 1, file) != 1) {
                free(term);
                fprintf(stderr, "Error al leer posting %u del término %u\n", p, t);
                goto error_cleanup;
            }
            
            // Leer posiciones
            size_t *positions = malloc(sizeof(size_t) * posting_header.position_count);
            if (!positions || fread(positions, sizeof(size_t), posting_header.position_count, file) != posting_header.position_count) {
                free(positions);
                free(term);
                fprintf(stderr, "Error al leer posiciones del posting %u del término %u\n", p, t);
                goto error_cleanup;
            }
            
            // Agregar al índice
            for (uint32_t pos = 0; pos < posting_header.position_count; pos++) {
                addTermToIndex(*index, term, posting_header.doc_id, positions[pos]);
            }
            
            free(positions);
        }
        
        free(term);
    }
    
    // Cargar documentos
    for (uint32_t d = 0; d < header.num_documents; d++) {
        DocumentHeader doc_header;
        if (fread(&doc_header, sizeof(DocumentHeader), 1, file) != 1) {
            fprintf(stderr, "Error al leer header de documento %u\n", d);
            goto error_cleanup;
        }
        
        // Leer filename
        char *filename_str = malloc(doc_header.filename_length);
        if (!filename_str || fread(filename_str, doc_header.filename_length, 1, file) != 1) {
            free(filename_str);
            fprintf(stderr, "Error al leer filename de documento %u\n", d);
            goto error_cleanup;
        }
        
        // Leer title (si existe)
        char *title = NULL;
        if (doc_header.title_length > 0) {
            title = malloc(doc_header.title_length);
            if (!title || fread(title, doc_header.title_length, 1, file) != 1) {
                free(filename_str);
                free(title);
                fprintf(stderr, "Error al leer título de documento %u\n", d);
                goto error_cleanup;
            }
        }
        
        // Agregar documento a la colección (asegurar que no exceda la capacidad)
        if ((*collection)->count < (*collection)->capacity) {
            DocumentInfo doc;
            doc.doc_id = doc_header.doc_id;
            doc.filename = strdup(filename_str);
            doc.title = title ? strdup(title) : NULL;
            doc.word_count = doc_header.word_count;
            
            (*collection)->docs[(*collection)->count++] = doc;
        }
        
        free(filename_str);
        if (title) free(title);
    }
    
    fclose(file);
    
    printf("Índice cargado desde formato binario: %s\n", filename);
    printf("Términos: %u, Documentos: %u\n", header.num_terms, header.num_documents);
    
    return 0;
    
error_cleanup:
    destroyIndex(*index);
    destroyDocumentCollection(*collection);
    *index = NULL;
    *collection = NULL;
    fclose(file);
    return -1;
}


// Validar archivo de índice
int validateIndexFile(const char *filename) {
    if (!filename) return -1;
    
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("validateIndexFile: Error al abrir archivo");
        return -1;
    }
    
    IndexFileHeader header;
    if (fread(&header, sizeof(IndexFileHeader), 1, file) != 1) {
        fprintf(stderr, "Error al leer header\n");
        fclose(file);
        return -1;
    }
    
    fclose(file);
    
    if (header.magic != INDEX_FILE_MAGIC) {
        fprintf(stderr, "Magic number incorrecto\n");
        return -1;
    }
    
    if (header.version != INDEX_FILE_VERSION) {
        fprintf(stderr, "Versión no soportada: %u\n", header.version);
        return -1;
    }
    
    printf("Archivo de índice válido\n");
    return 0;
}

// Imprimir información del archivo de índice
void printIndexFileInfo(const char *filename) {
    if (!filename) return;
    
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("printIndexFileInfo: Error al abrir archivo");
        return;
    }
    
    IndexFileHeader header;
    if (fread(&header, sizeof(IndexFileHeader), 1, file) != 1) {
        fprintf(stderr, "Error al leer header\n");
        fclose(file);
        return;
    }
    
    // Obtener tamaño del archivo
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fclose(file);
    
    printf("=== Información del archivo de índice ===\n");
    printf("Archivo: %s\n", filename);
    printf("Magic: 0x%08X\n", header.magic);
    printf("Versión: %u\n", header.version);
    printf("Términos: %u\n", header.num_terms);
    printf("Documentos: %u\n", header.num_documents);
    printf("Próximo doc ID: %u\n", header.next_doc_id);
    printf("Checksum: %lu\n", header.checksum);
    printf("Tamaño del archivo: %ld bytes\n", file_size);
    
    // Obtener información del sistema de archivos
    struct stat st;
    if (stat(filename, &st) == 0) {
        printf("Última modificación: %s", ctime(&st.st_mtime));
    }
}

// Crear backup del índice
int createIndexBackup(const InvertedIndex *index, const DocumentCollection *collection,
                      const char *backup_dir) {
    if (!index || !collection || !backup_dir) return -1;
    
    // Crear directorio de backup si no existe
    struct stat st = {0};
    if (stat(backup_dir, &st) == -1) {
        if (mkdir(backup_dir, 0755) == -1) {
            perror("Error al crear directorio de backup");
            return -1;
        }
    }
    
    // Generar nombre de archivo con timestamp
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char backup_filename[512];
    snprintf(backup_filename, sizeof(backup_filename), 
             "%s/index_backup_%04d%02d%02d_%02d%02d%02d.idx",
             backup_dir, 
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    
    int result = saveIndexToBinary(index, collection, backup_filename);
    if (result == 0) {
        printf("Backup creado: %s\n", backup_filename);
    }
    
    return result;
}

// Exportar índice a formato texto
int exportIndexToText(const InvertedIndex *index, const DocumentCollection *collection,
                      const char *filename) {
    if (!index || !collection || !filename) return -1;
    
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("exportIndexToText: Error al abrir archivo");
        return -1;
    }
    
    fprintf(file, "=== ÍNDICE INVERTIDO ===\n");
    fprintf(file, "Términos únicos: %zu\n", index->size);
    fprintf(file, "Documentos: %zu\n", collection->count);
    fprintf(file, "Próximo doc ID: %u\n\n", index->next_doc_id);
    
    fprintf(file, "=== DOCUMENTOS ===\n");
    for (size_t i = 0; i < collection->count; i++) {
        fprintf(file, "ID: %u | %s", collection->docs[i].doc_id, collection->docs[i].filename);
        if (collection->docs[i].title) {
            fprintf(file, " | %s", collection->docs[i].title);
        }
        fprintf(file, " | %zu palabras\n", collection->docs[i].word_count);
    }
    
    fprintf(file, "\n=== TÉRMINOS ===\n");
    for (size_t i = 0; i < index->capacity; i++) {
        if (index->entries[i].term) {
            fprintf(file, "\n%s (df=%u):\n", index->entries[i].term, index->entries[i].doc_frequency);
            
            PostingNode *current = index->entries[i].head;
            while (current) {
                fprintf(file, "  Doc %u: ", current->posting.doc_id);
                for (size_t j = 0; j < current->posting.position_count; j++) {
                    fprintf(file, "%zu", current->posting.positions[j]);
                    if (j < current->posting.position_count - 1) fprintf(file, ", ");
                }
                fprintf(file, "\n");
                current = current->next;
            }
        }
    }
    
    fclose(file);
    printf("Índice exportado a texto: %s\n", filename);
    return 0;
}