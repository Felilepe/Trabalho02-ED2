#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>

// Definimos 'item' da mesma forma que em fila.h e pilha.h
// para consistência.
#ifndef ITEM_T_DEFINED
typedef void *item;
#define ITEM_T_DEFINED
#endif

// Opaque pointer para a struct da lista
typedef struct lista Lista;

/****************************************************
*@brief Cria uma lista vazia.
*@return Um ponteiro para a lista inicializada.
****************************************************/
Lista *lista_create();

/********************************************************************
*@brief Verifica se a lista está vazia.
*@param l Ponteiro para a lista que será checada.
*@return True se a lista estiver vazia, caso contrário False.
********************************************************************/
bool lista_isEmpty(Lista *l);

/*****************************************************************************
*@brief Percorre a lista e executa uma função 'acao' para cada item.
* (Modelado a partir de fila_passthrough)
*@param l Ponteiro para a lista.
*@param acao A função de callback a ser executada em cada item.
*@param aux_data Um ponteiro auxiliar para ser passado para a função 'acao'.
*****************************************************************************/
void lista_passthrough(Lista *l, void (*acao)(item i, item aux_data), item aux_data);

/******************************************************************
*@brief Libera toda a memória alocada pelos nós da lista.
*@note NÃO libera a memória dos 'itens' (dados) armazenados.
*@param l Ponteiro para a lista a ser destruída.
******************************************************************/
void lista_destroy(Lista *l);

/******************************************************************
*@brief Pega o número de itens atualmente na lista.
*@param l Ponteiro para a lista.
*@return O número de itens (int) na lista.
******************************************************************/
int lista_getSize(Lista *l);

/******************************************************************
*@brief Obtém o item armazenado no início (cabeça) da lista.
*@param l Ponteiro para a lista.
*@return O item (void*).
*@note Encerra o programa se a lista estiver vazia.
******************************************************************/
item lista_getHead(Lista *l);

/******************************************************************
*@brief Obtém o item armazenado no final (cauda) da lista.
*@param l Ponteiro para a lista.
*@return O item (void*).
*@note Encerra o programa se a lista estiver vazia.
******************************************************************/
item lista_getTail(Lista *l);

/******************************************************************
*@brief Obtém um item em uma posição específica (índice).
*@param l Ponteiro para a lista.
*@param index O índice do elemento desejado (0 a size-1).
*@return O item (void*) na posição especificada.
*@note Encerra o programa se o índice for inválido.
******************************************************************/
item lista_getItem(Lista *l, int index);

/*****************************************************************
 *@brief Adiciona um item no início da lista.
 *@param l Ponteiro para a lista.
 *@param i O item (void*) a ser adicionado.
 *****************************************************************/
void lista_insertHead(Lista *l, item i);

/*****************************************************************
 *@brief Adiciona um item no fim da lista.
 *@param l Ponteiro para a lista.
 *@param i O item (void*) a ser adicionado.
 *****************************************************************/
void lista_insertTail(Lista *l, item i);

/**********************************************************************************
*@brief Remove e retorna o item do início da lista.
*@param l Ponteiro para a lista.
*@return Retorna o ponteiro para o item (void*) removido.
*@note Apenas libera o nó da lista, não o dado 'item'.
*@note Causa 'exit(1)' se a lista estiver vazia.
**********************************************************************************/
void lista_removeHead(Lista *l);

/**********************************************************************************
*@brief Remove e retorna o item do fim da lista.
*@param l Ponteiro para a lista.
*@return Retorna o ponteiro para o item (void*) removido.
*@note Apenas libera o nó da lista, não o dado 'item'.
*@note Causa 'exit(1)' se a lista estiver vazia.
**********************************************************************************/
void lista_removeTail(Lista *l);

/**********************************************************************************
*@brief Remove um nó da lista em uma posição específica.
*@param l Ponteiro para a lista.
*@param index O índice do nó a ser removido (0 a size-1).
*@note Apenas libera o nó da lista, não o dado 'item'.
*@note Encerra o programa se o índice for inválido.
**********************************************************************************/
void lista_removeNode(Lista *l, int index);

#endif