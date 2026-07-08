#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#ifndef HTE_H
#define HTE_H

typedef void* Hash;

/*Inicia uma tabela Hash extensível*/
Hash hashCreate(uint32_t bucket_size);

/*Adiciona um arquivo para a hash table e retorna a chave do arquivo inserido*/
bool hashAdd(Hash h, void *data, const char* key);

/*Recebe uma chave e a remove da hash table, retornando o arquivo atribuido*/
void *hashRemove(Hash h, const char* key);

/*Verifica se uma chave está contida em uma tabela hash. Retorna a data armazenada se existe ou NULL se não existe*/
bool hashExists(Hash h, const char* key);

/*Recebe uma chave e retorna o arquivo atribuido*/
void *hashGetData(Hash h, const char* key);

/*Recebe uma hash table e retorna o tamanho*/
int hashGetSize(Hash h);

/*Recebe uma hash table e imprime todos os seus itens*/
void hashPrint(Hash h);

bool hashUpdate(Hash h, const char* key, const void* new_data);

void hashForEach(Hash h, void (*aux)(void* item, void* aux_data), void* aux_data);

/*Finaliza uma tabela Hash extensível*/
void hashDestroy(Hash h);

#endif