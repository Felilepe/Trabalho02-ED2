#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

typedef struct 
{
    Data target; 
    bool found;  
} DataSearchCtx;

typedef struct 
{
    double distance;
    const char* predecessor_id;
    bool visited;
} DijkstraCell;


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

static void checkDataInVertex(void* item, void* aux_data) 
{
    vertex *v = (vertex*)item;
    DataSearchCtx *ctx = (DataSearchCtx*)aux_data;

    if (ctx->found) return;

    if (v->data == ctx->target) {
        ctx->found = true;
        return;
    }

    if (v->adjacent_vertices != NULL) {
        int size = lista_getSize(v->adjacent_vertices);
        for (int i = 0; i < size; i++) {
            edge *e = (edge*)lista_getItem(v->adjacent_vertices, i);
            if (e->data == ctx->target) {
                ctx->found = true;
                return;
            }
        }
    }
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

int graphGetMaxVertexCount(Graph g)
{
    graph* gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphGetVertexCount\n");
        return -1;
    }
    
    return gc -> max_vertices;
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



bool graphSetVertexMaxCount(Graph g, int n)
{
    graph *gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphSetVertexMaxCount\n");
        return false;
    }

    gc -> max_vertices = n;
    if(gc -> max_vertices == n) return true;
    return false;
}

bool graphSetEdgeWeight(Graph g, const char* source_id, const char* target_id, double peso)
{
    edge *e = graphGetEdge(g, source_id, target_id);
    if(e == NULL){
        printf("Erro: aresta nula em graphSetEdgeWeight\n");
        return false;
    }
    e -> weight = peso;
    if(e -> weight == peso) return true;
    return false;
}

bool graphSetVertexAxisX (Graph g, const char* id, double x)
{
    vertex *v = graphGetVertex(g, id);
    if(v == NULL){
        printf("Erro: vertice nula em graphSetVertexAxisX");
        return false;
    }

    v -> x = x;
    if(v -> x == x) return true;
    return false;
}

bool graphSetVertexAxisY(Graph g, const char* id, double y)
{
    vertex *v = graphGetVertex(g, id);
    if(v == NULL){
        printf("Erro: vertice nula egm graphSetVertexAxisY");
        return false;
    }

    v -> y = y;
    if(v -> y == y) return true;
    return false;
}



bool graphRemoveVertex(Graph g, const char* id)
{
    graph* gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphRemoveVertex\n");
        return false;
    }

    hashRemove(gc -> vertices, id);
    return !hashExists(gc -> vertices, id);
}

bool graphRemoveEdge(Graph g, const char* source_id, const char* target_id)
{
    vertex *v = graphGetVertex(g, source_id);
    if(v == NULL){
        printf("Erro: vertice nula em graphRemoveEdge");
        return false;
    }
    for(int i = 0; i < lista_getSize(v -> adjacent_vertices); i++){
        edge *e = lista_getItem(v ->adjacent_vertices, i);
        if(e -> source_id == source_id && e -> target_id == target_id){
            lista_removeNode(v ->adjacent_vertices, i);
            return true;
        }
    }
    return false;
}



bool graphDataExists(Graph g, Data n)
{
    graph *gc = (graph*)g;
    if (gc == NULL || n == NULL || gc->vertice_count == 0) {
        return false;
    }

    DataSearchCtx ctx;
    ctx.target = n;
    ctx.found = false;

    hashForEach(gc->vertices, checkDataInVertex, &ctx);

    return ctx.found;
}

void graphForEach(Graph g, void (*aux)(void* item, void* aux_data), void* aux_data)
{
    graph* gc = (graph*)g;
    
    if(gc == NULL || aux == NULL){
        printf("Erro: grafo ou acao nula em graphForEach");
        return;
    }

    if(gc -> vertice_count == 0){
        printf("Erro: nao ha vertices no grafo em graphForEach");
        return;
    }

    hashForEach(gc -> vertices, aux, aux_data);
}




Lista *graphExecuteDijkstra(Graph g, bool use_time, const char* source_id, const char* end_id, double *cost)
{
    if(source_id == NULL && end_id == NULL){
        printf("Erro: id de inicio e/ou fim nulo(s) em graphExecuteDijkstra");
        return NULL;
    }

    Hash dijkstratemp = hashCreate(graphGet)
}
