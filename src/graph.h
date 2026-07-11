#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include "lista.h"

typedef void* Graph;
typedef void* Data;
typedef void* Vertex;
typedef void* Edge;

/*Recebe uma tamanho e retorna ponteiro para Grafo*/
Graph graphCreate(int n);

/*Recebe um Grafo e um Data e insere Data no Grafo, retornando booleano*/
bool graphAddVertex(Graph g, Data d, const char* id);

bool graphConnectVertices(Graph g, Data d, double weight, const char* source_id, const char* target_id, const char *label);

bool graphIsAdjacent(Graph g, const char* source_id, const char* target_id);



Vertex graphGetVertex(Graph g, const char* id);

Edge graphGetEdge(Graph g, const char* source_id, const char* target_id);

int graphGetVertexCount(Graph g);

int graphGetMaxVertexCount(Graph g);

int graphGetEdgeCount(Graph g);

double graphGetVertexAxisX(Graph g, const char* id);

double graphGetVertexAxisY(Graph g, const char* id);

Data graphGetVertexData(Graph g, const char* id);

Data graphGetEdgeData(Graph g, const char* source_id, const char* target_id);

double graphGetEdgeWeight(Graph g, const char* source_id, const char* target_id);

Lista *graphGetVertexNeighbors(Graph g, const char* id);



bool graphSetVertexMaxCount(Graph g, int n);

bool graphSetEdgeWeight(Graph g, const char* source_id, const char* target_id, double peso);

bool graphSetVertexAxisX (Graph g, const char* id, double x);

bool graphSetVertexAxisY(Graph g, const char* id, double y);



bool graphRemoveVertex(Graph g, const char* id);

bool graphRemoveEdge(Graph g, const char* source_id, const char* target_id);




bool graphDataExists(Graph g, Data n);

void graphForEach(Graph g, void (*aux)(void* item, void* aux_data), void* aux_data);

Lista *graphExecuteDijkstra(Graph g, const char* source_id, const char* target_id,
                            double (*weight_fn)(Data edge_data, void *context),
                            void *context, double *out_cost);

Lista *graphExecuteTarjan(Graph g, bool (*edge_filter)(void *edge_data, void *context), void* context);



void graphDestroy(Graph g);


#endif