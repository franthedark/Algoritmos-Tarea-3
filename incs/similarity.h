#ifndef SIMILARITY_H
#define SIMILARITY_H

#include <stdint.h>

typedef struct {
    uint32_t doc_id;
    double similarity;
    const char* filename;
} SimilarityResult;


double jaccard_similarity(const char *doc1, const char *doc2);
double cosine_similarity(const char *doc1, const char *doc2);

#endif