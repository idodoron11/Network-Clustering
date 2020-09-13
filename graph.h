
#ifndef GRAPH_H
#define GRAPH_H

#include "spmat.h"

typedef struct _graph {
    /* number of vertices.
     * V={1,2,...,n} */
    int n;

    /* adjacency matrix of capacity nXn */
    spmat *adjMat;
    /* the degrees of the graph's vertices */
    int *degrees;
    /* sum of vertices' degrees */
    int degreeSum;

} Graph;

/**
 * Creates a new graph object from an input file.
 * @param inputFilePath a path to an input file of a graph
 * @return a reference to a new graph object, defined by the given input file.
 */
Graph *constructGraphFromInput(char *inputFilePath);

/**
 * Frees up any memory resources that have been dynamically allocated by the graph construction.
 * @param G a pointer to a graph
 */
void destroyGraph(Graph *G);

double getExpectedEdges(Graph *G, int i, int j);

#endif /*GRAPH_H*/
