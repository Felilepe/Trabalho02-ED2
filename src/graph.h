#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

typedef void* Graph;
typedef void* Data;
typedef void* Vertex;
typedef void* Edge;

/*Recebe uma tamanho e uma flag e retorna ponteiro para Grafo*/
Graph graphCreate(int n, int flag);

/*Recebe um Grafo e um Data e insere Data no Grafo, retornando booleano*/
bool graphAddVertex(Graph g, Data d, const char* id);

bool graphConnectVertex(Graph g, Data d, const char* source_id, const char* target_id, char *rua);

bool graphIsAdjacent(Graph g, const char* source, const char* target);



Vertex graphGetVertex(Graph g, const char* id);

Edge graphGetEdge(Graph g, const char* source_id, const char* target_id);

int graphGetVertexCount(Graph g);

int graphGetEdgeCount(Graph g);

Data graphGetVertexData(Graph g, const char* id);

Edge graphGetEdgeData(Graph g, const char* source_id, const char* target_id);

/*Lista graphGetNeighbors*/



void graphSetVertexCount(Graph g, int n);



bool graphRemoveVertex(Graph g, const char* id);

bool graphRemoveEdge(Graph g, const char* source_id, const char* target_id);




bool graphDataExists(Graph g, Data n);

void graphForEach(Graph h, void (*aux)(void* item, void* aux_data), void* aux_data);

/*Lista graphExecuteDijkstra*/

/*Lista graphExecuteTarjan*/



bool graphDestroy(Graph g);


#endif