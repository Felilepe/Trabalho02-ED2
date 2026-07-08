#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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



static Vertex vertexCreate(double x, double y, const char* id, Data data)
{
    vertex *v = malloc(sizeof(vertex));
    if(v == NULL){
        printf("Erro: Falha na alocação de memória ao criar vértice");
        return NULL;
    }

    v ->adjacent_vertices = lista_create();
    v -> data = data;
    v -> id = NULL;
    v -> x = x;
    v -> y = y;

    v -> id = (char*)malloc(strlen(id) + 1);
    if(v -> id == NULL){
        printf("Erro: Falha ao atribuir id a vertice criada");
    }
    strcpy(v -> id, id);

    return v;
}

static void vertexDestroy(Vertex v)
{
    vertex *vc = (vertex*)v;
    free(vc -> id);
    lista_destroy(vc ->adjacent_vertices);
    free(vc);
}

static Edge edgeCreate(double weight, const char* source_id, const char* target_id, Data data, char* label)
{
    edge *e = malloc(sizeof(edge));
    if(e == NULL){
        printf("Erro: falha ao alocar memoria para nova aresta\n");
        return NULL;
    }

    e -> data = data;
    e -> weight = weight;
    e -> source_id = (char*)malloc(strlen(source_id) + 1);
    if(e -> source_id == NULL){
        printf("Erro: Falha ao atribuir source_id a aresta criada\n");
    }
    strcpy(e -> source_id, source_id);
    e -> target_id = (char*)malloc(strlen(target_id) + 1);
    if(e -> target_id == NULL){
        printf("Erro: Falha ao atribuir target_id a aresta criada\n");
    }
    strcpy(e -> target_id, target_id);
    e -> label = (char*)malloc(strlen(label) + 1);
    if(e -> label == NULL){
        printf("Erro: Falha ao atribuir label a aresta criada\n");
    }
    strcpy(e -> label, label);

    return e;
}



Graph graphCreate(int n)
{
    graph *g = malloc(sizeof(graph));
    if(g == NULL){
        printf("Erro: Falha na alocação de memória em graphCreate\n");
        return NULL;
    }
    
    g -> max_vertices = n;
    g -> vertice_count = 0;
    g -> edge_count = 0;
    g -> vertices = hashCreate(8);

    return (Graph)g;
}



bool graphAddVertex(Graph g, Data d, const char* id)
{
    graph *gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: Ponteiro de grafo NULL em graphAddVertex\n");
        return false;
    }
    if(id == NULL){
        printf("Erro: id nulo em graphAddVertex\n");
        return false;
    }

    Vertex v = vertexCreate(0.0, 0.0, id, d);

    hashAdd(gc -> vertices, v, id);

    if(hashExists(gc -> vertices, id)){
        return true;
    }

    vertexDestroy(v);
    return false;
}

bool graphConnectVertices(Graph g, Data d, double weight, const char* source_id, const char* target_id, const char *label)
{
    graph *gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: pointeiro para grafo nulo em graphConnectVertices\n");
        return false;
    }
    if(source_id == NULL || target_id == NULL){
        printf("Erro: id de uma das vertices eh nulo em graphConnectVertices\n");
        return false;
    }

    vertex *source = hashGetData(gc -> vertices, source_id);
    vertex *target = hashGetData(gc -> vertices, target_id);
    if(source == NULL || target == NULL){
        printf("Erro: umas das vertices eh nula em graphConnectVertices\n");
        return false;
    }

    Edge e = edgeCreate(weight, source_id, target_id, d, label);
    if(e == NULL){
        printf("Erro: falha ao criar aresta nova em graphConnectvertices\n");
        return false;
    }

    lista_insertTail(source -> adjacent_vertices, e);
    return true;
}

bool graphIsAdjacent(Graph g, const char* source_id, const char* target_id)
{
    graph *gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro para grafo nulo em graphIsAdjacent\n");
        return false;
    }

    vertex* source = hashGetData(gc -> vertices, source_id);
    vertex* target = hashGetData(gc -> vertices, target_id);
    if(source == NULL || target == NULL){
        printf("Erro: vertices source e/ou target nulo(s) em graphIsAdjacent\n");
        return false;
    }
    
    return lista_Exists(source -> adjacent_vertices, target, NULL);
}



Vertex graphGetVertex(Graph g, const char* id)
{
    graph *gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphGetVertex\n");
        return -1;
    }

    return hashGetData(gc -> vertices, id);
}

Edge graphGetEdge(Graph g, const char* source_id, const char* target_id)
{
    graph *gc = (graph*)g;
    if (gc == NULL) {
        printf("Erro: ponteiro para grafo nulo em graphGetEdge\n");
        return NULL;
    }

    vertex *source = hashGetData(gc->vertices, source_id);
    if (source == NULL) {
        printf("Erro: vertice nula em graphGetEdge");
        return NULL; 
    }

    int size = lista_getSize(source->adjacent_vertices);

    for (int i = 0; i < size; i++) {
        edge *e = (edge*)lista_getItem(source->adjacent_vertices, i);
        
        if (strcmp(e->target_id, target_id) == 0) {
            return (Edge)e; // Aresta encontrada!
        }
    }

    return NULL;
}

int graphGetVertexCount(Graph g)
{
    graph* gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphGetVertexCount\n");
        return -1;
    }
    return gc -> vertice_count;
}

int graphGetEdgeCount(Graph g)
{
    graph* gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphGetEdgeCount\n");
        return -1;
    }
    return gc -> edge_count;   
}

Data graphGetVertexData(Graph g, const char* id)
{
    graph* gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphGetVertexData\n");
        return NULL;
    }
    vertex *v = hashGetData(gc -> vertices, id);
    if(v == NULL)
    return v -> data;
}

Data graphGetEdgeData(Graph g, const char* source_id, const char* target_id)
{
    graph *gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphGetEdgeData\n");
        return NULL;
    }

    edge* e = graphGetEdge(g, source_id, target_id);
    if(e == NULL){
        printf("Erro: falha ao pegar aresta em graphGetEdgeData");
        return NULL;
    }
    
    return e -> data;
}

double graphGetEdgeWeight(Graph g, const char* source_id, const char* target_id)
{
    graph *gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphGetEdgeWeight\n");
        return -1;
    }

    edge* e = graphGetEdge(g, source_id, target_id);
    if(e == NULL){
        printf("Erro: falha ao pegar aresta em graphGetEdgeWeight");
        return -1;
    }
    
    return e -> weight;
}

Lista *graphGetVertexNeighbors(Graph g, const char* id)
{
    graph *gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphGetVertexNeighbors\n");
        return NULL;
    }
    
    vertex *source = hashGetData(gc->vertices, id);
    if (source == NULL) {
        printf("Erro: vertice nula em graphVertexNeighbors");
        return NULL; 
    }

    return source -> adjacent_vertices;
}