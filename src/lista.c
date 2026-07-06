#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lista.h"


typedef struct nodeL 
{
    item data;         
    struct nodeL *next;
    struct nodeL *prev; 
} NodeL;

struct lista {
    NodeL *head;  
    NodeL *tail;  
    int size;
};


Lista *lista_create()
{
    Lista *l = (Lista*)malloc(sizeof(Lista));
    if(l == NULL){
        printf("Erro na alocacao de memoria ao criar lista.");
        exit(1);
    }
    
    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
    return l;
}

bool lista_isEmpty(Lista *l)
{
    return l->size == 0;
}

void lista_passthrough(Lista *l, void (*acao)(item i, item aux_data), item aux_data)
{
    if (l == NULL || acao == NULL || lista_isEmpty(l)) {
        return;
    }

    NodeL *atual = l->head;

    while (atual != NULL) {
        acao(atual->data, aux_data);
        atual = atual->next;
    }
}



void lista_destroy(Lista *l)
{
    if (l == NULL) return;

    NodeL *atual = l->head;
    
    while(atual != NULL){
        NodeL *temp = atual;
        atual = atual->next;
        free(temp); 
    }
    
    free(l);
}



int lista_getSize(Lista *l)
{
    if(l == NULL) return 0;
    return l->size;
}

item lista_getHead(Lista *l)
{
    if(lista_isEmpty(l)){
        printf("Erro: Tentativa de acesso ao head de lista vazia.");
        exit(1);
    }
    return l->head->data;
}

item lista_getTail(Lista *l)
{
    if(lista_isEmpty(l)){
        printf("Erro: Tentativa de acesso ao tail de lista vazia.");
        exit(1);
    }
    return l->tail->data;
}

item lista_getItem(Lista *l, int index)
{
    if(l == NULL || lista_isEmpty(l)){
        printf("Erro: Tentativa de acesso a lista vazia ou nula em lista_getItem.");
        exit(1);
    }

    if(index < 0 || index >= l->size){
        printf("Erro: Indice %d invalido. A lista possui indices de 0 a %d.", index, l->size - 1);
        exit(1);
    }

    NodeL *atual;

    if (index < l->size / 2) {
        atual = l->head;
        for (int i = 0; i < index; i++) {
            atual = atual->next;
        }
    } 
    else {
        atual = l->tail;
        for (int i = l->size - 1; i > index; i--) {
            atual = atual->prev;
        }
    }

    return atual->data;
}

void lista_removeNode(Lista *l, int index)
{
    if(l == NULL || lista_isEmpty(l)){
        printf("Erro: Tentativa de acesso a lista vazia ou nula em lista_removeNode.");
        exit(1);
    }

    if(index < 0 || index >= l->size){
        printf("Erro: Indice %d invalido. A lista possui indices de 0 a %d.", index, l->size - 1);
        exit(1);
    }

    NodeL *atual;

    if (index < l->size / 2) {
        atual = l->head;
        for (int i = 0; i < index; i++) {
            atual = atual->next;
        }
    } 
    else {
        atual = l->tail;
        for (int i = l->size - 1; i > index; i--) {
            atual = atual->prev;
        }
    }


    if (atual->prev != NULL) {
        atual->prev->next = atual->next;
    } else {
        l->head = atual->next;
    }

    if (atual->next != NULL) {
        atual->next->prev = atual->prev;
    } else {
        l->tail = atual->prev;
    }

    l->size--;

    
    free(atual);
}



void lista_insertHead(Lista *l, item i)
{
    NodeL *novoNode = (NodeL*)malloc(sizeof(NodeL));
    if(novoNode == NULL){
        printf("Erro na alocacao de memoria ao criar novo no para lista.");
        exit(1);
    }
    
    novoNode->data = i;
    novoNode->prev = NULL;
    novoNode->next = l->head;

    if(lista_isEmpty(l)){
        l->head = novoNode;
        l->tail = novoNode;
    } else {
        l->head->prev = novoNode;
        l->head = novoNode;
    }
    l->size++;
}

void lista_insertTail(Lista *l, item i)
{
    NodeL *novoNode = (NodeL*)malloc(sizeof(NodeL));
    if(novoNode == NULL){
        printf("Erro na alocacao de memoria ao criar novo no para lista.");
        exit(1);
    }
    
    novoNode->data = i;
    novoNode->next = NULL;
    novoNode->prev = l->tail;

    if(lista_isEmpty(l)){
        l->head = novoNode;
        l->tail = novoNode;
    } else {
        l->tail->next = novoNode;
        l->tail = novoNode;
    }
    l->size++;
}

void lista_removeHead(Lista *l)
{
    if(lista_isEmpty(l)){
        printf("Erro: Tentativa de remocao de lista vazia.");
        exit(1);
    }

    NodeL *nodeRemover = l->head;

    l->head = nodeRemover->next; 
    l->size--;

    if(lista_isEmpty(l)){
        l->tail = NULL;
    } else {
        l->head->prev = NULL;
    }
    
    free(nodeRemover);
}

void lista_removeTail(Lista *l)
{
    if(lista_isEmpty(l)){
        printf("Erro: Tentativa de remocao de lista vazia.");
        exit(1);
    }

    NodeL *nodeRemover = l->tail;
    
    l->tail = nodeRemover->prev; 
    l->size--;

    if(lista_isEmpty(l)){
        l->head = NULL;
    } else {
        l->tail->next = NULL;
    }
    
    free(nodeRemover);
}


