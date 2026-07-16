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



static uint32_t hashString(const char* str) 
{
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

static void splitBucket(hashtable *ht, uint32_t dir_index) 
{
    bucket *old_bucket = ht->directory[dir_index];
    
    if (old_bucket->local_depth == ht->global_depth) {
        int old_capacity = 1 << ht->global_depth;
        int new_capacity = 1 << (ht->global_depth + 1);
        
        ht->directory = realloc(ht->directory, sizeof(bucket*) * new_capacity);
        
        for (int i = 0; i < old_capacity; i++) {
            ht->directory[i + old_capacity] = ht->directory[i];
        }
        
        ht->global_depth++;
    }

    bucket *new_bucket = malloc(sizeof(bucket));
    new_bucket->local_depth = old_bucket->local_depth + 1;
    new_bucket->record_count = 0;
    new_bucket->records = malloc(sizeof(record) * ht->bucket_size);
    for (uint32_t i = 0; i < ht->bucket_size; i++) {
        new_bucket->records[i].is_occupied = false;
    }

    old_bucket->local_depth++;

    int diff_bit = 1 << (old_bucket->local_depth - 1);
    int dir_size = 1 << ht->global_depth;
    
    for (int i = 0; i < dir_size; i++) {
        if (ht->directory[i] == old_bucket) {
            if ((i & diff_bit) != 0) {
                ht->directory[i] = new_bucket;
            }
        }
    }

    record *temp_records = malloc(sizeof(record) * ht->bucket_size);
    memcpy(temp_records, old_bucket->records, sizeof(record) * ht->bucket_size);
    
    old_bucket->record_count = 0;
    for (uint32_t i = 0; i < ht->bucket_size; i++) {
        old_bucket->records[i].is_occupied = false;
    }

    for (uint32_t i = 0; i < ht->bucket_size; i++) {
        if (temp_records[i].is_occupied) {
            uint32_t hash_val = hashString(temp_records[i].key);
            uint32_t new_index = hash_val & ((1 << old_bucket->local_depth) - 1);
            
            bucket *target = ((new_index & diff_bit) != 0) ? new_bucket : old_bucket;
            
            for (uint32_t j = 0; j < ht->bucket_size; j++) {
                if (!target->records[j].is_occupied) {
                    target->records[j] = temp_records[i];
                    target->record_count++;
                    break;
                }
            }
        }
    }
    
    free(temp_records);
}



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

bool hashAdd(Hash h, void *data, const char* key) 
{
    hashtable *ht = (hashtable*)h;
    if (ht == NULL){
        printf("Erro: ponteiro para hash table nulo em hashAdd\n");
        return false;
    }
    if(key == NULL || data == NULL){
        printf("Erro: chave e/ou data  nulo(s) em hashAdd\n");
        return false;
    }

    uint32_t hash_val = hashString(key);

    while (true) {
        uint32_t index = hash_val & ((1 << ht->global_depth) - 1);
        bucket *b = ht->directory[index];

        for (uint32_t i = 0; i < ht->bucket_size; i++) {
            if (b->records[i].is_occupied && strcmp(b->records[i].key, key) == 0) {
                printf("Erro: tentativa de insercao com chave duplicada em hashAdd");
                return false;
            }
        }

        if (b->record_count < ht->bucket_size) {
            for (uint32_t i = 0; i < ht->bucket_size; i++) {
                if (!b->records[i].is_occupied) {
                    strncpy(b->records[i].key, key, MAX_KEY_LENGTH - 1);
                    b->records[i].key[MAX_KEY_LENGTH - 1] = '\0';
                    b->records[i].data = data;
                    b->records[i].is_occupied = true;
                    b->record_count++;
                    return true;
                }
            }
        } else {
            splitBucket(ht, index);
            ht->total_expansions++;
        }
    }
}

void *hashRemove(Hash h, const char* key) {
    hashtable *ht = (hashtable*)h;
    
    if (ht == NULL || key == NULL) {
        printf("Erro: ponteiro para hash table nulo em HashRemove\n");
        return NULL;
    }

    uint32_t hash_val = hashString(key);

    uint32_t index = hash_val & ((1 << ht->global_depth) - 1);
    bucket *b = ht->directory[index];

    for (uint32_t i = 0; i < ht->bucket_size; i++) {
        if (b->records[i].is_occupied && strcmp(b->records[i].key, key) == 0) {
            
            void *data_to_return = b->records[i].data;

            b->records[i].is_occupied = false;
            b->records[i].data = NULL; 
            
            b->records[i].key[0] = '\0'; 

            b->record_count--;

            return data_to_return;
        }
    }

    return NULL;
}

bool hashExists(Hash h, const char* key)
{
    hashtable *ht = (hashtable*)h;
    
    if (ht == NULL || key == NULL) {
        printf("Erro: ponteiro para hash table nulo em HashExists\n");
        return false;
    }

    uint32_t hash_val = hashString(key);


    uint32_t index = hash_val & ((1 << ht->global_depth) - 1);
    bucket *b = ht->directory[index];

    for (uint32_t i = 0; i < ht->bucket_size; i++) {
        if (b->records[i].is_occupied && strcmp(b->records[i].key, key) == 0) {
            return true;
        }
    }
    return false;
}

void *hashGetData(Hash h, const char* key)
{
    hashtable *ht = (hashtable*)h;
    
    if (ht == NULL || key == NULL) {
        printf("Erro: ponteiro para hash table nulo em HashGetData\n");
        return NULL;
    }

    uint32_t hash_val = hashString(key);


    uint32_t index = hash_val & ((1 << ht->global_depth) - 1);
    bucket *b = ht->directory[index];

    for (uint32_t i = 0; i < ht->bucket_size; i++) {
        if (b->records[i].is_occupied && strcmp(b->records[i].key, key) == 0) {
            return b -> records[i].data;
        }
    }
    return NULL;
}

