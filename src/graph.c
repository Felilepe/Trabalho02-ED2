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

typedef struct 
{
    Hash dij_hash;
    Lista *pq_lista;
} InitContext;


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

static void fillDijTempHashAndList(void *item, void* aux_item)
{
    vertex *v = (vertex*)item;
    InitContext *ctx = (InitContext*)aux_item;

    DijkstraCell* cell = malloc(sizeof(DijkstraCell));
    if (cell == NULL) return;

    cell->distance = INFINITY;
    cell->predecessor_id = NULL;
    cell->visited = false;

    hashAdd(ctx->dij_hash, cell, v->id);
    
    lista_insertTail(ctx->pq_lista, (void*)v->id);
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




Lista *graphExecuteDijkstra(Graph g, bool use_time, const char* source_id, const char* target_id)
{
    graph *gc = (graph*)g;
    if (gc == NULL || source_id == NULL || target_id == NULL) return NULL;

    InitContext ctx;
    ctx.dij_hash = hashCreate(4); 
    ctx.pq_lista = lista_create();

    graphForEach(g, fillDijTempHashAndList, &ctx);

    DijkstraCell *source_cell = (DijkstraCell*) hashGetData(ctx.dij_hash, source_id);
    if (source_cell != NULL) {
        source_cell->distance = 0.0;
    } else {
        printf("Erro: Origem não encontrada.\n");
        return NULL;
    }

    while (!lista_isEmpty(ctx.pq_lista)) {
        int tamanho = lista_getSize(ctx.pq_lista);
        double menor_dist = INFINITY;
        int index_menor = -1;
        const char* id_menor = NULL;

        for (int i = 0; i < tamanho; i++) {
            const char* id_atual = (const char*) lista_getItem(ctx.pq_lista, i);
            DijkstraCell *cell = (DijkstraCell*) hashGetData(ctx.dij_hash, id_atual);
            
            if (cell != NULL && !cell->visited && cell->distance < menor_dist) {
                menor_dist = cell->distance;
                index_menor = i;
                id_menor = id_atual;
            }
        }

        if (index_menor == -1 || menor_dist == INFINITY) {
            break; 
        }

        lista_removeNode(ctx.pq_lista, index_menor); 
        DijkstraCell *menor_cell = (DijkstraCell*) hashGetData(ctx.dij_hash, id_menor);
        menor_cell->visited = true;

        if (strcmp(id_menor, target_id) == 0) {
            break;
        }

        vertex *v_atual = (vertex*) hashGetData(gc->vertices, id_menor);
        if (v_atual->adjacent_vertices != NULL) {
            int num_vizinhos = lista_getSize(v_atual->adjacent_vertices);
            
            for (int i = 0; i < num_vizinhos; i++) {
                edge *aresta = (edge*) lista_getItem(v_atual->adjacent_vertices, i);
                const char* vizinho_id = aresta->target_id;
                
                DijkstraCell *vizinho_cell = (DijkstraCell*) hashGetData(ctx.dij_hash, vizinho_id);
                
                if (vizinho_cell == NULL || vizinho_cell->visited) continue;

                double peso_aresta = aresta->weight; // Pode precisar de alterar isto com base no parâmetro 'use_time'

                if (menor_cell->distance + peso_aresta < vizinho_cell->distance) {
                    vizinho_cell->distance = menor_cell->distance + peso_aresta;
                    vizinho_cell->predecessor_id = id_menor;
                }
            }
        }
    }

    Lista *caminho_final = lista_create();
    const char* curr_id = target_id;
    
    DijkstraCell *target_cell = (DijkstraCell*) hashGetData(ctx.dij_hash, target_id);
    if (target_cell == NULL || target_cell->distance == INFINITY) {
        lista_destroy(caminho_final);
        caminho_final = NULL;
    } else {
        while (curr_id != NULL) {
            lista_insertHead(caminho_final, (void*)curr_id);
            if (strcmp(curr_id, source_id) == 0) break;
            
            DijkstraCell *cell = (DijkstraCell*) hashGetData(ctx.dij_hash, curr_id);
            curr_id = cell->predecessor_id;
        }
    }

    lista_destroy(ctx.pq_lista);
    
    hashForEach(ctx.dij_hash, (void (*)(void*, void*))free, NULL);
    hashDestroy(ctx.dij_hash);

    return caminho_final;
}
