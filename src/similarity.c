// Diego Galindo, Francisco Mercado
#include "similarity.h"
#include "indexer.h"
#include "utils.h"
#include <math.h>
#include <string.h>
#include <stdlib.h> 
#include <stdint.h>

double jaccard_similarity(const char *doc1, const char *doc2) {
    size_t count1, count2;
    char **tokens1 = tokenize(doc1, &count1);
    char **tokens2 = tokenize(doc2, &count2);
    
    size_t intersection = 0;
    size_t union_size = count1 + count2;
    
    for (size_t i = 0; i < count1; i++) {
        for (size_t j = 0; j < count2; j++) {
            if (strcmp(tokens1[i], tokens2[j]) == 0) {
                intersection++;
                break;
            }
        }
    }
    
    union_size -= intersection;
    double similarity = (union_size == 0) ? 1.0 : (double)intersection / union_size;
    
    freeTokens(tokens1, count1);
    freeTokens(tokens2, count2);
    return similarity;
}

// Función auxiliar para duplicar cadenas
static char* my_strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);
    if (copy) memcpy(copy, str, len);
    return copy;
}

double cosine_similarity(const char *doc1, const char *doc2) {
    size_t count1, count2;
    char **tokens1 = tokenize(doc1, &count1);
    char **tokens2 = tokenize(doc2, &count2);
    
    double dot_product = 0.0;
    double mag1 = 0.0;
    double mag2 = 0.0;
    
    // Construir vocabulario unificado
    size_t vocab_size = count1 + count2;
    char **vocab = malloc(vocab_size * sizeof(char*));
    if (!vocab) {
        freeTokens(tokens1, count1);
        freeTokens(tokens2, count2);
        return 0.0;
    }
    
    size_t vocab_count = 0;
    
    // Añadir tokens del primer documento
    for (size_t i = 0; i < count1; i++) {
        vocab[vocab_count++] = my_strdup(tokens1[i]);
    }
    
    // Añadir tokens del segundo documento (sin duplicados)
    for (size_t i = 0; i < count2; i++) {
        int found = 0;
        for (size_t j = 0; j < vocab_count; j++) {
            if (strcmp(tokens2[i], vocab[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            vocab[vocab_count++] = my_strdup(tokens2[i]);
        }
    }
    
    // Calcular vectores TF
    double *vector1 = calloc(vocab_count, sizeof(double));
    double *vector2 = calloc(vocab_count, sizeof(double));
    
    if (!vector1 || !vector2) {
        for (size_t i = 0; i < vocab_count; i++) free(vocab[i]);
        free(vocab);
        free(vector1);
        free(vector2);
        freeTokens(tokens1, count1);
        freeTokens(tokens2, count2);
        return 0.0;
    }
    
    for (size_t i = 0; i < vocab_count; i++) {
        for (size_t j = 0; j < count1; j++) {
            if (strcmp(vocab[i], tokens1[j]) == 0) {
                vector1[i] += 1.0;
            }
        }
        for (size_t j = 0; j < count2; j++) {
            if (strcmp(vocab[i], tokens2[j]) == 0) {
                vector2[i] += 1.0;
            }
        }
        
        dot_product += vector1[i] * vector2[i];
        mag1 += vector1[i] * vector1[i];
        mag2 += vector2[i] * vector2[i];
    }
    
    mag1 = sqrt(mag1);
    mag2 = sqrt(mag2);
    double similarity = (mag1 == 0 || mag2 == 0) ? 0.0 : dot_product / (mag1 * mag2);
    
    // Liberar memoria
    for (size_t i = 0; i < vocab_count; i++) {
        free(vocab[i]);
    }
    free(vocab);
    free(vector1);
    free(vector2);
    freeTokens(tokens1, count1);
    freeTokens(tokens2, count2);
    
    return similarity;
}