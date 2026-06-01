#ifndef SVG_H
#define SVG_H

#include <stdio.h>
#include "quadra.h"

/**
 * @brief Abre um arquivo .svg e escreve o cabeçalho base do SVG.
 * @param path Caminho completo e nome do arquivo a ser criado.
 * @return Ponteiro válido de arquivo (FILE*) para o SVG aberto, ou NULL em erro.
 **/
FILE *startSVG(const char *path);

/**
 * @brief Escreve as tags de fechamento do SVG e fecha o arquivo.
 * @param file_name Ponteiro (FILE*) do arquivo SVG a ser fechado.
 **/
void stopSVG(FILE *file_name);

/**
 * @brief Insere a tag de um elemento do tipo Quadra (retângulo) no arquivo SVG.
 * @param file_name Ponteiro (FILE*) para o arquivo SVG.
 * @param r         A quadra a ser desenhada.
 **/
void svgInsertQuadra(FILE *file_name, Quadra r);

/**
 * @brief Desenha uma cruz vermelha indicativa em uma dada coordenada.
 * @param file_name Ponteiro para o arquivo SVG.
 * @param x         Coordenada X central da cruz.
 * @param y         Coordenada Y central da cruz.
 **/
void svgMarcaRedCross(FILE *file_name, double x, double y);

/**
 * @brief Desenha um círculo preto indicativo em uma dada coordenada.
 * @param file_name Ponteiro para o arquivo SVG.
 * @param x         Coordenada X central do círculo.
 * @param y         Coordenada Y central do círculo.
 **/
void svgMarcaBlackCircle(FILE *file_name, double x, double y);

/**
 * @brief Desenha um quadrado vermelho rotulado com um CPF em uma coordenada.
 * @param file_name Ponteiro para o arquivo SVG.
 * @param x         Coordenada X central do quadrado.
 * @param y         Coordenada Y central do quadrado.
 * @param cpf       String de texto contendo o CPF a ser exibido.
 **/
void svgMarcaRedSquare(FILE *file_name, double x, double y, char* cpf);

/**
 * @brief Escreve o texto com o identificador (CEP) centralizado na quadra no SVG.
 * @param svg Ponteiro para o arquivo SVG.
 * @param q   A quadra onde o texto será ancorado.
 **/
void svgMarcaCep(FILE *svg, Quadra q);

/**
 * @brief Insere no SVG a indicação textual de contagem de elementos em uma face da quadra.
 * @param svg      Ponteiro para o arquivo SVG.
 * @param q        A quadra referenciada.
 * @param face     Caractere indicando a face ('N', 'S', 'L', 'O').
 * @param contagem Número a ser desenhado próximo à face correspondente.
 **/
void svgMarcaContagemFace(FILE *svg, Quadra q, char face, int contagem);

/**
 * @brief Insere no SVG a indicação textual da contagem total na quadra.
 * @param svg   Ponteiro para o arquivo SVG.
 * @param q     A quadra referenciada.
 * @param total Número total a ser exibido.
 **/
void svgMarcaContagemTotal(FILE *svg, Quadra q, int total);

#endif