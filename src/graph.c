#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "graph.h"
#include "lista.h"
#include "hte.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct vertex
{
    double x, y;
    const char* id;
    Data data;
    Lista *edges;
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
    const char *id_removido;
    graph *gc;
} RemoveVertexCtx;

typedef struct 
{
    double distance;
    const edge* predecessor_edge;
    bool visited;
} DijkstraCell;

typedef struct 
{
    Hash dij_hash;
    Lista *pq_lista;
} InitContext;

typedef struct 
{
    int index;
    int lowlink;
    bool onStack;
} TarjanCell;

typedef struct 
{
    Hash tarjan_hash;       
    Lista *stack;           
    Lista *scc_list;        
    int index_counter;      
    Graph g;               
    bool (*edge_filter)(void*, void*); 
    void *filter_context;   
} TarjanContext;

static Vertex vertexCreate(double x, double y, const char* id, Data data)
{
    vertex *v = malloc(sizeof(vertex));
    if(v == NULL){
        printf("Erro: Falha na alocação de memória ao criar vértice");
        return NULL;
    }

    v ->edges = lista_create();
    v -> data = data;
    v -> id = NULL;
    v -> x = x;
    v -> y = y;

    v -> id = (char*)malloc(strlen(id) + 1);
    if(v -> id == NULL){
        printf("Erro: Falha ao atribuir id a vertice criada");
        free(v -> edges);
        free(v);
        return NULL;
    }
    strcpy(v -> id, id);

    return v;
}

static Edge edgeCreate(double weight, const char* source_id, const char* target_id, Data data, char* label)
{
    edge *e = malloc(sizeof(edge));
    if(e == NULL){
        printf("Erro: falha ao alocar memoria para nova aresta\n");
        return NULL;
    }
    if(label == NULL){
        printf("Erro: ponteiro label nulo. Nao foi possivel criar aresta\n");
        free(e);
        return NULL;
    }

    e -> data = data;
    e -> weight = weight;
    e -> source_id = (char*)malloc(strlen(source_id) + 1);
    if(e -> source_id == NULL){
        printf("Erro: Falha ao atribuir source_id a aresta criada\n");
        free(e);
        return NULL;
    }
    strcpy(e -> source_id, source_id);
    e -> target_id = (char*)malloc(strlen(target_id) + 1);
    if(e -> target_id == NULL){
        printf("Erro: Falha ao atribuir target_id a aresta criada\n");
        free(e -> source_id);
        free(e);
        return NULL;
    }
    strcpy(e -> target_id, target_id);
    e -> label = (char*)malloc(strlen(label) + 1);
    if(e -> label == NULL){
        printf("Erro: Falha ao atribuir label a aresta criada\n");
        return NULL;
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

    if (v->edges != NULL) {
        int size = lista_getSize(v->edges);
        for (int i = 0; i < size; i++) {
            edge *e = (edge*)lista_getItem(v->edges, i);
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
    cell->predecessor_edge = NULL;
    cell->visited = false;

    hashAdd(ctx->dij_hash, cell, v->id);
    
    lista_insertTail(ctx->pq_lista, (void*)v->id);
}

static void initTarjanCell(void *item, void* aux_item) 
{
    vertex *v = (vertex*)item;
    Hash tarjan_hash = (Hash)aux_item;

    TarjanCell* cell = malloc(sizeof(TarjanCell));
    if (cell != NULL) {
        cell->index = -1; 
        cell->lowlink = -1;
        cell->onStack = false;
        hashAdd(tarjan_hash, cell, v->id);
    }
}

static void tarjanDFS(const char* v_id, TarjanContext *ctx) 
{
    TarjanCell *v_cell = (TarjanCell*) hashGetData(ctx->tarjan_hash, v_id);
    v_cell->index = ctx->index_counter;
    v_cell->lowlink = ctx->index_counter;
    ctx->index_counter++;

    lista_insertHead(ctx->stack, (void*)v_id);
    v_cell->onStack = true;

    Lista *vizinhos = graphGetVertexNeighbors(ctx->g, v_id);
    if (vizinhos != NULL) {
        int num_vizinhos = lista_getSize(vizinhos);
        
        for (int i = 0; i < num_vizinhos; i++) {
            edge *aresta = (edge*) lista_getItem(vizinhos, i);
            
            if (ctx->edge_filter != NULL) {
                if (!ctx->edge_filter(aresta->data, ctx->filter_context)) {
                    continue; 
                }
            }

            const char* w_id = aresta->target_id;
            TarjanCell *w_cell = (TarjanCell*) hashGetData(ctx->tarjan_hash, w_id);
            
            if (w_cell->index == -1) {
                tarjanDFS(w_id, ctx);
                v_cell->lowlink = MIN(v_cell->lowlink, w_cell->lowlink);
            } else if (w_cell->onStack) {
                v_cell->lowlink = MIN(v_cell->lowlink, w_cell->index);
            }
        }
    }

    if (v_cell->lowlink == v_cell->index) {
        Lista *scc_atual = lista_create();
        const char* w_id = NULL;
        
        do {
            w_id = (const char*) lista_getItem(ctx->stack, 0); 
            lista_removeHead(ctx->stack);
            
            TarjanCell *w_cell = (TarjanCell*) hashGetData(ctx->tarjan_hash, w_id);
            w_cell->onStack = false;
            
            lista_insertTail(scc_atual, (void*)w_id);
            
        } while (strcmp(w_id, v_id) != 0);
        
        lista_insertTail(ctx->scc_list, scc_atual);
    }
    lista_destroy(vizinhos);
}

static void startTarjan(void *item, void *aux_data) 
{
    vertex *v = (vertex*)item;
    TarjanContext *ctx = (TarjanContext*)aux_data;
    TarjanCell *cell = (TarjanCell*) hashGetData(ctx->tarjan_hash, v->id);
    
    if (cell != NULL && cell->index == -1) {
        tarjanDFS(v->id, ctx);
    }
}



static void freeEdge(void *item, void *aux_data) {
    edge *e = (edge*)item;
    if (e != NULL) {
        if (e->source_id) free((void*)e->source_id);
        if (e->target_id) free((void*)e->target_id);
        if (e->label) free((void*)e->label);
        
        free(e);
    }
}

static void freeVertex(void *item, void *aux_data) {
    vertex *v = (vertex*)item;
    if (v != NULL) {
        if (v->edges != NULL) {
            lista_passthrough(v->edges, freeEdge, NULL);
            

            lista_destroy(v->edges); 
        }

        if (v->id) free((void*)v->id);

        free(v);
    }
}

static void removeEdgesToVertex(void *item, void *aux_data)
{
    vertex *v = (vertex*)item;
    RemoveVertexCtx *ctx = (RemoveVertexCtx*)aux_data;

    int i = 0;
    while (i < lista_getSize(v -> edges)) {
        edge *e = (edge*)lista_getItem(v -> edges, i);

        if (strcmp(e -> target_id, ctx -> id_removido) == 0) {
            freeEdge(e, NULL);
            lista_removeNode(v -> edges, i);
            ctx -> gc -> edge_count--;
        } else {
            i++;
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
    if(gc -> vertice_count == gc -> max_vertices){
        printf("Erro: limite de vertices atingido. Para aumentar limite, use graphSetVertexMaxCount\n");
        return false;
    }

    Vertex v = vertexCreate(0.0, 0.0, id, d);
    if(v == NULL){
        printf("Erro: ponteiro para vertice nulo em graphAddVertex\n");
        return false;
    }
    
    bool added = hashAdd(gc -> vertices, v, id);
    if(added){
        gc -> vertice_count++;
    }

    return added;
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
    if(label == NULL){
        printf("Erro: ponteiro para label nulo em graphConnectVertices\n");
        return false;
    }
    if(graphIsAdjacent(g, source_id, target_id)){
        printf("Erro: vertices ja estao conectadas\n");
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

    lista_insertTail(source -> edges, e);
    gc -> edge_count++;
    return true;
}

bool graphIsAdjacent(Graph g, const char* source_id, const char* target_id)
{
    return graphGetEdge(g, source_id, target_id) != NULL;
}



Vertex graphGetVertex(Graph g, const char* id)
{
    graph *gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphGetVertex\n");
        return NULL;
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

    int size = lista_getSize(source->edges);

    for (int i = 0; i < size; i++) {
        edge *e = (edge*)lista_getItem(source->edges, i);
        
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

double graphGetVertexAxisX(Graph g, const char* id)
{
    vertex *v = graphGetVertex(g, id);
    if(v == NULL){
        printf("Erro: vertice nula em graphGetVertexAxisX");
        return INFINITY;
    }
    return v -> x;
}

double graphGetVertexAxisY(Graph g, const char* id)
{
    vertex *v = graphGetVertex(g, id);
    if(v == NULL){
        printf("Erro: vertice nula em graphGetVertexAxisY");
        return INFINITY;
    }
    return v -> y;
}

Data graphGetVertexData(Graph g, const char* id)
{
    graph* gc = (graph*)g;
    if(gc == NULL){
        printf("Erro: ponteiro de grafo nulo em graphGetVertexData\n");
        return NULL;
    }
    vertex *v = hashGetData(gc -> vertices, id);
    if(v == NULL){
        printf("Erro: vertice nula em graphGetVertexData");
        return NULL;
    }
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

    Lista *copy = lista_create();
    for(int i = 0; i<lista_getSize(source -> edges); i++){
        lista_insertTail(copy, lista_getItem(source -> edges, i));
    }

    return copy;
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
    if(id == NULL){
        printf("Erro: id nulo em graphRemoveVertex\n");
        return false;
    }

    vertex *v = hashGetData(gc -> vertices, id);
    if(v == NULL){
        printf("Erro: vertice nula em graphRemoveVertex\n");
        return false;
    }

    RemoveVertexCtx ctx;
    ctx.id_removido = id;
    ctx.gc = gc;
    hashForEach(gc -> vertices, removeEdgesToVertex, &ctx);

    gc -> edge_count -= lista_getSize(v -> edges);

    hashRemove(gc -> vertices, id);
    freeVertex(v, NULL);

    gc -> vertice_count--;

    return true;
}

bool graphRemoveEdge(Graph g, const char* source_id, const char* target_id)
{
    vertex *v = graphGetVertex(g, source_id);
    if(v == NULL){
        printf("Erro: vertice nula em graphRemoveEdge");
        return false;
    }
    for(int i = 0; i < lista_getSize(v -> edges); i++){
        edge *e = lista_getItem(v ->edges, i);
        if(!strcmp(e -> source_id, source_id) && !strcmp(e -> target_id, target_id)){
            freeEdge(e, NULL);
            lista_removeNode(v ->edges, i);
            ((graph*)g) -> edge_count--;
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



Lista *graphExecuteDijkstra(Graph g, const char* source_id, const char* target_id,
                            double (*weight_fn)(Data edge_data, void *context),
                            void *context, double *out_cost)
{
    graph *gc = (graph*)g;
    if (gc == NULL || source_id == NULL || target_id == NULL || weight_fn == NULL) return NULL;

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
        if (v_atual->edges != NULL) {
            int num_vizinhos = lista_getSize(v_atual->edges);
            
            for (int i = 0; i < num_vizinhos; i++) {
                edge *aresta = (edge*) lista_getItem(v_atual->edges, i);
                const char* vizinho_id = aresta->target_id;
                
                DijkstraCell *vizinho_cell = (DijkstraCell*) hashGetData(ctx.dij_hash, vizinho_id);
                
                if (vizinho_cell == NULL || vizinho_cell->visited) continue;

                
                double peso_aresta = weight_fn(aresta -> data, context);

                if (menor_cell->distance + peso_aresta < vizinho_cell->distance) {
                    vizinho_cell->distance = menor_cell->distance + peso_aresta;
                    vizinho_cell->predecessor_edge = aresta;
                }
            }
        }
    }

    Lista *caminho_final = lista_create();
    const char* curr_id = target_id;
    
    DijkstraCell *target_cell = (DijkstraCell*) hashGetData(ctx.dij_hash, target_id);
    if (target_cell == NULL || target_cell->distance == INFINITY) {
        if(out_cost != NULL){
            *out_cost = -1;
        }
        lista_destroy(caminho_final);
        caminho_final = NULL;
    } else {
        if(out_cost != NULL){
            *out_cost = target_cell->distance;
        }
        while (curr_id != NULL) {
            if (strcmp(curr_id, source_id) == 0) break;
            
            DijkstraCell *cell = (DijkstraCell*) hashGetData(ctx.dij_hash, curr_id);
            lista_insertHead(caminho_final, cell ->predecessor_edge);
            curr_id = cell->predecessor_edge -> source_id;
        }
    }

    lista_destroy(ctx.pq_lista);
    
    hashForEach(ctx.dij_hash, (void (*)(void*, void*))free, NULL);
    hashDestroy(ctx.dij_hash);

    return caminho_final;
}

Lista *graphExecuteTarjan(Graph g, bool (*edge_filter)(void *edge_data, void *context), void* context)
{
    graph *gc = (graph*)g;
    if (gc == NULL || gc->vertice_count == 0) return NULL;

    TarjanContext ctx;
    ctx.tarjan_hash = hashCreate(4); 
    ctx.stack = lista_create();     
    ctx.scc_list = lista_create();  
    ctx.index_counter = 0;
    ctx.g = g;
    ctx.edge_filter = edge_filter;
    ctx.filter_context = context;

    graphForEach(g, initTarjanCell, ctx.tarjan_hash);

    graphForEach(g, startTarjan, &ctx);

    lista_destroy(ctx.stack); 
    
    hashForEach(ctx.tarjan_hash, (void (*)(void*, void*))free, NULL);
    hashDestroy(ctx.tarjan_hash);

    return ctx.scc_list;
}



void graphDestroy(Graph g) 
{
    graph *gc = (graph*)g;
    if (gc == NULL) return;

    if (gc->vertices != NULL) {
        hashForEach(gc->vertices, freeVertex, NULL);
        
        hashDestroy(gc->vertices);
    }

    free(gc);
}