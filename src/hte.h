#include <stdio.h>
#include <stdbool.h>

#ifndef HTE_H
#define HTE_H

typedef void* Hash;

/*Inicia uma tabela Hash extensível*/
Hash hashCreate();

/*Adiciona um arquivo para a hash table e retorna a chave do arquivo inserido*/
int hashAdd(Hash h, FILE *file);

/*Recebe uma chave e a remove da hash table, retornando o arquivo atribuido*/
FILE *hashRemove(Hash h, int key);

/*Verifica se uma chave está contida em uma tabela hash e retorna verdadeiro ou falso em booleano*/
bool hashExists(Hash h, int key);

/*Recebe uma chave e retorna o arquivo atribuido*/
FILE *hashGetFile(Hash h, int key);

/*Recebe uma hash table e retorna o tamanho*/
int hashGetSize(Hash h);

/*Recebe uma hash table e imprime todos os seus itens*/
void hashPrint(Hash h);

void hashForEach(Hash h, void (*aux)(void* item, void* aux_data), void* aux_data);

/*Finaliza uma tabela Hash extensível*/
void hashDestroy(Hash h);

#endif