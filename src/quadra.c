#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "quadra.h"

#define TIPO_R 2
#define DEFAULT_WIDTH 1.5
#define DEFAULT_STROKE_COLOR "#E06F2D"
#define DEFAULT_FILL_COLOR "#E08E2D"

typedef struct quadra 
{
    int type;
    double x, y, w, h, sw;
    char *corborda, *corpreench, *cep;

}quadra;



Quadra quadraCreate(char* cep, double x, double y, double w, double h)
{
    quadra *ret= malloc(sizeof(quadra));
    if(ret == NULL){
        printf("Erro: falha ao alocar memoria para ponteiro em quadraCreate\n");
        return NULL;
    }
    
    ret -> cep = (char*)malloc(strlen(cep) + 1);
    if(ret -> cep == NULL){
        printf("Erro: falha ao alocar memoria para cep em quadraCreate\n");
        free(ret);
        return NULL;
    }
    strcpy(ret -> cep, cep);

    ret -> type = TIPO_R;
    ret -> x = x;
    ret -> y = y;
    ret -> w = w;
    ret -> h = h;
    ret -> sw = DEFAULT_WIDTH;
    
    ret -> corborda = (char*)malloc(strlen(DEFAULT_STROKE_COLOR) + 1);
    if(ret -> corborda == NULL){
        printf("Erro: falha ao alocar memoria para cor de borda em quadraCreate\n");
        free(ret -> cep);
        free(ret);
        return NULL;
    }
    strcpy(ret -> corborda, DEFAULT_STROKE_COLOR);

    ret -> corpreench = (char*)malloc(strlen(DEFAULT_FILL_COLOR) + 1);
    if(ret -> corpreench == NULL){
        free(ret -> cep);
        free(ret -> corborda);
        free(ret);
        printf("Erro: falha ao alocar memoria para cor de preenchimento em quadraCreate\n");
        return NULL;
    }
    strcpy(ret -> corpreench, DEFAULT_FILL_COLOR);

    return ret;
}


double quadraGetCoordX(Quadra r) {return ((quadra*)r) -> x;}

double quadraGetCoordY(Quadra r) {return ((quadra*)r) -> y;}

char* quadraGetCep(Quadra r) {return((quadra*)r) -> cep;}

double quadraGetHeight(Quadra r) {return ((quadra*)r) -> h;}

double quadraGetWidth(Quadra r) {return ((quadra*)r) -> w;}

double quadraGetBordaWidth(Quadra r) {return ((quadra*)r) -> sw;}

char* quadraGetCorBorda(Quadra r) {return ((quadra*)r) -> corborda;}

char* quadraGetCorPreench(Quadra r) {return ((quadra*)r) -> corpreench;}

int quadraGetType(Quadra r) {return ((quadra*)r) -> type;}

double quadraCalcArea(Quadra r)
{
    double altura = ((quadra*)r) -> h;
    double largura = ((quadra*)r) -> w;
    
    return altura * largura;
}


void quadraSetCoordX(Quadra r, double x) {((quadra*)r) -> x = x;}

void quadraSetCoordY(Quadra r, double y) {((quadra*)r) -> y = y;}

void quadraSetHeight(Quadra r, double h) {((quadra*)r) -> h = h;}

void quadraSetWidth(Quadra r, double w) {((quadra*)r) -> w = w;}

void quadraSetBordaWidth(Quadra r, double sw) {((quadra*)r) -> sw = sw;}

void quadraSetCorBorda(Quadra r, char* corborda)
{
    if (r == NULL || corborda == NULL) return;
    quadra *quad = (quadra*)r;
    free(quad -> corborda);
    quad -> corborda = (char*)malloc(strlen(corborda) + 1);
    if(quad -> corborda == NULL){
        printf("Erro: falha ao alocar memoria para cor de borda em quadraSetCorBorda\n");
        return;
    }
    strcpy(((quadra*)r) -> corborda, corborda);
}

void quadraSetCorPreench(Quadra r, char* corpreench)
{
    if (r == NULL || corpreench == NULL) return;
    quadra *quad = (quadra*)r;
    free(quad -> corpreench);
    quad -> corpreench = (char*)malloc(strlen(corpreench) + 1);
    if(quad -> corpreench == NULL){
        printf("Erro: falha ao alocar memoria para cor de preenchimento em quadraSetCorPreench\n");
        return;
    }
    strcpy(quad -> corpreench, corpreench);
}


void quadraDestroy(Quadra r)
{
    if (r == NULL) return;
    free(((quadra*)r) -> cep);    
    free(((quadra*)r) -> corborda);
    free(((quadra*)r) -> corpreench);
    free(r);
}