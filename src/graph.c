#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "lista.h"
#include "hte.h"

typedef struct vertex
{
    double x, y;
    const char* id;
    Data data;
    Lista *adjacent_vertices;
}vertex;

typedef struct edge
{
    double weight;
    const char* source_id;
    const char* target_id;
    Data data;
    char* label;
}edge;

typedef struct graph
{
    Hash vertices;
    int max_vertices;
    int vertice_count;
    int edge_count;
}graph;



Graph graphCreate(int n)
{
    graph *g = malloc(sizeof(graph));
    if(g == NULL){
        printf("Erro: Falha na alocação de memória em graphCreate");
        return NULL;
    }
    
    g -> max_vertices = n;
    g -> vertice_count = 0;
    g -> edge_count = 0;
    g -> vertices = hashCreate(8);

    return (Graph)g;
}




