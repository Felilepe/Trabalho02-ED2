#ifndef QUADRA_H
#define QUADRA_H

/**
 * @typedef Quadra
 * @brief Tipo opaco que representa uma quadra (retângulo geométrico) no mapa.
 **/
typedef void* Quadra;

/**
 * @brief Cria e aloca memória para uma nova quadra com os parâmetros fornecidos.
 * @param cep        Identificador único para a quadra.
 * @param x          Coordenada X da âncora do retângulo.
 * @param y          Coordenada Y da âncora do retângulo.
 * @param w          Largura do retângulo.
 * @param h          Altura do retângulo.
 * @return Retorna um ponteiro do tipo Quadra para o retângulo recém-criado.
 **/
Quadra quadraCreate(char* cep, double x, double y, double w, double h);

/**
 * @brief Obtém a área de uma quadra.
 * @param r O retângulo (quadra) do qual a área será calculada.
 * @return Retorna o valor da área.
 **/
double quadraCalcArea(Quadra r);

/**
 * @brief Libera a memória alocada para a quadra.
 * @param r A quadra a ser destruída.
 **/
void quadraDestroy(Quadra r);

// ... (Getters) ...

/**
 * @brief Obtém a coordenada X da âncora de uma quadra.
 * @param r A quadra.
 * @return Retorna o valor da coordenada X.
 **/
double quadraGetCoordX(Quadra r);

/**
 * @brief Obtém a coordenada Y da âncora de uma quadra.
 * @param r A quadra.
 * @return Retorna o valor da coordenada Y.
 **/
double quadraGetCoordY(Quadra r);

/**
 * @brief Obtém a cor de borda de uma quadra.
 * @param r A quadra.
 * @return Retorna a string com a cor da borda.
 **/
char* quadraGetCorBorda(Quadra r);

/**
 * @brief Obtém a cor de preenchimento de uma quadra.
 * @param r A quadra.
 * @return Retorna a string com a cor de preenchimento.
 **/
char* quadraGetCorPreench(Quadra r);

/**
 * @brief Obtém a altura de uma quadra.
 * @param r A quadra.
 * @return Retorna o valor da altura.
 **/
double quadraGetHeight(Quadra r);

/**
 * @brief Obtém a largura de uma quadra.
 * @param r A quadra.
 * @return Retorna o valor da largura.
 **/
double quadraGetWidth(Quadra r);

/**
 * @brief Obtém a espessura da borda de uma quadra.
 * @param r A quadra.
 * @return Retorna o valor da espessura.
 **/
double quadraGetBordaWidth(Quadra r);

/**
 * @brief Obtém o identificador (CEP) de uma quadra.
 * @param r A quadra.
 * @return Retorna a string do ID (CEP).
 **/
char* quadraGetCep(Quadra r);

/**
 * @brief Obtém o tipo do elemento geométrico associado.
 * @param r A quadra.
 * @return Retorna o valor inteiro que representa o tipo.
 **/
int quadraGetType(Quadra r);

// ... (Setters) ...

/**
 * @brief Define a coordenada X da âncora de uma quadra.
 * @param r A quadra.
 * @param x A nova coordenada X.
 **/
void quadraSetCoordX(Quadra r, double x);

/**
 * @brief Define a coordenada Y da âncora de uma quadra.
 * @param r A quadra.
 * @param y A nova coordenada Y.
 **/
void quadraSetCoordY(Quadra r, double y);

/**
 * @brief Define a altura de uma quadra.
 * @param r A quadra.
 * @param h A nova altura.
 **/
void quadraSetHeight(Quadra r, double h);

/**
 * @brief Define a largura de uma quadra.
 * @param r A quadra.
 * @param w A nova largura.
 **/
void quadraSetWidth(Quadra r, double w);

/**
 * @brief Define a espessura da borda de uma quadra.
 * @param r  A quadra.
 * @param sw A nova espessura de borda.
 **/
void quadraSetBordaWidth(Quadra r, double sw);

/**
 * @brief Define a cor da borda de uma quadra.
 * @param r        A quadra.
 * @param corborda String com a nova cor da borda.
 **/
void quadraSetCorBorda(Quadra r, char *corborda);

/**
 * @brief Define a cor de preenchimento de uma quadra.
 * @param r          A quadra.
 * @param corpreench String com a nova cor de preenchimento.
 **/
void quadraSetCorPreench(Quadra r, char *corpreench);

#endif