#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

typedef void* Graph;
typedef void* Node;

Graph graphCreate(int n, int flag);

bool graphAddNode(Graph g, Node n);

bool graphRemoveNode(Graph g, Node n);

Node graphGetNode(Graph g, int n);

int graphGetSize(Graph g);

bool graphNodeExists(Graph g, Node n);

void graphForEach(Graph h, void (*aux)(void* item, void* aux_data), void* aux_data);

#endif