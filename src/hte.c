#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "hte.h"

#define MAX_KEY_LENGTH      32   

typedef struct {
    char key[MAX_KEY_LENGTH];
    void* data;
    size_t data_size;            
    bool is_occupied;
} record;

typedef struct {
    int local_depth;
    uint32_t record_count;
    record *records;
} bucket;

typedef struct {
    int global_depth;
    uint32_t bucket_size;
    int total_expansions;
    bucket **directory;

} hashtable;



Hash hashCreate(uint32_t bucket_size)
{
    hashtable *h = malloc(sizeof(hashtable));
    if(h == NULL){
        printf("Erro: falha ao alocar memoria em ashCreate\n");
        return NULL;
    }

    h -> bucket_size = bucket_size;
    h -> global_depth = 0;
    h -> total_expansions = 0;
    h->directory = malloc(sizeof(bucket*) * 1);
    if (h->directory == NULL) {
        printf("Erro: falha ao alocar memoria para diretorio em hashCreate\n");
        free(h);
        return NULL;
    }

    bucket *initial_bucket = malloc(sizeof(bucket));
    if (initial_bucket == NULL) {
        printf("Erro: falha ao alocar memoria para bucket inicial em hashCreate\n");
        free(h->directory);
        free(h);
        return NULL;
    }

    initial_bucket->local_depth = 0;
    initial_bucket->record_count = 0;

    initial_bucket->records = malloc(sizeof(record) * bucket_size);
    if (initial_bucket->records == NULL) {
        printf("Erro: falha ao alocar memoria para records do bucket inicial em hashCreate\n");
        free(initial_bucket);
        free(h->directory);
        free(h);
        return NULL;
    }

    for (uint32_t i = 0; i < bucket_size; i++) {
        initial_bucket->records[i].is_occupied = false;
    }

    h->directory[0] = initial_bucket;

    return h;
}
