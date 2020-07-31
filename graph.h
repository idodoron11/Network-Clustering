
#ifndef GRAPH_H
#define GRAPH_H

#include "spmat.h"

typedef struct _graph {
    /* Number of vertices.
     * V={1,2,...,n} */
    int	n;

    /* Adjacency matrix of size nXn
     * E={(i,j) s.t. spAdjMat[i*n + j]=1} */
    double* adjMat;
    spmat* spAdjMat;

    /* number of edges */
    int m;

    /* degrees vector */
    int* deg;
} graph;

/**
 * Creates a new graph object.
 * @param n number of vertices
 * @param m number of edges
 * @param input a path to an input file of a graph
 * @return a reference to a new graph object, defined by the given adjacency matrix.
 */
graph* constructGraphFromInput(char* inputFilePath);

/**
 * Frees up any memory resources that have been dynamically allocated by the graph construction.
 * @param G a pointer to a graph
 */
void destroyGraph(graph* G);

/**
 * Returns the value kept in the (i,j) entry of G's adjacency matrix.
 * @param G a pointer to a graph
 * @param i the vertex from which the edge exists. Should satisfy 0 <= i < G->n.
 * @param j the vertex to which the edge enters. Should satisfy 0 <= j < G->n.
 * @return 1 if the edge (i,j) exists, and 0 otherwise.
 */
int getEdge(graph* G, int i, int j);

/**
 * Prints a representation of G.
 * @param G a not too large graph.
 */
void printGraph(graph* G);

/**
 * @param G a graph.
 * @param i a vertex in G. should satisfy 0 <= i < G->n.
 * @return The degree of vertex i, namely the number of undirected edges it's connected to.
 */
int getDegree(graph* G, int i);

#endif /*GRAPH_H*/
