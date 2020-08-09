
#ifndef GRAPH_H
#define GRAPH_H

#include "spmat.h"

typedef struct _graph {
    /* number of vertices.
     * V={1,2,...,n} */
    int n;

    /* adjacency matrix of size nXn
     * E={(i,j) s.t. spAdjMat[i*n + j]=1} */
    Matrix *adjMat;
    /* the degrees of the graph's vertices */
    int *degrees;
    /* sum of vertices' degrees */
    int degreeSum;
    /* "K", a symmetric matrix, each entry i,j equals degrees[i]*degrees[j]/degreeSum */
    Matrix *expectedEdges;

} Graph;

/**
 * Creates a new graph object from an input file.
 * @param inputFilePath a path to an input file of a graph
 * @return a reference to a new graph object, defined by the given input file.
 */
Graph *constructGraphFromInput(char *inputFilePath);

/**
 * Creates a nee graph object from an input matrix.
 * @param adjMatrix the adjacency matrix given as an input.
 * @param n the number of vertices.
 * @return a reference to a new graph object, defined by the given adjacency matrix.
 */
graph* constructGraphFromMatrix(double* adjMatrix, int n);

/**
 * Frees up any memory resources that have been dynamically allocated by the graph construction.
 * @param G a pointer to a graph
 */
void destroyGraph(Graph *G);

/**
 * Returns the value kept in the (i,j) entry of G's adjacency matrix.
 * @param G a pointer to a graph
 * @param i the vertex from which the edge exists. Should satisfy 0 <= i < G->n.
 * @param j the vertex to which the edge enters. Should satisfy 0 <= j < G->n.
 * @return 1 if the edge (i,j) exists, and 0 otherwise.
 */
int getEdge(Graph *G, int i, int j);

/**
 * Prints a representation of G.
 * @param G a not too large graph.
 */
void printGraph(Graph *G);

#endif /*GRAPH_H*/
