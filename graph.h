
#ifndef GRAPH_H
#define GRAPH_H

typedef struct _graph {
    /* Number of vertices.
     * V={1,2,...,n} */
    int		n;

    /* Adjacency matrix of size nXn
     * E={(i,j) s.t. adjMat[i*n + j]=1} */
    int* adjMat;
} graph;

/**
 * Creates a new graph object.
 * @param n number of vertices
 * @param adjMat a n by n matrix consisting of zeros and ones only, represented by a n^2 long vector.
 *          Should be a symmetric matrix.
 * @return a reference to a new graph object, defined by the given adjacency matrix.
 */
graph* constructGraphFromInput(int n, int* adjMat);

/**
 * Creates a new graph with n vertices and no edges.
 * @param n the number of vertices in the graph
 * @return a reference to a new graph with no edges.
 */
graph* constructEmptyGraph(int n);

/**
 * Frees up any memory resources that have been dynamically allocated by the graph construction.
 * @param G a pointer to a graph
 */
void destroyGraph(graph* G);

/**
 * Creates a new edge (i,j) in the graph G.
 * @param G a pointer to a graph.
 * @param i the vertex from which the edge exists. Should satisfy 0 <= i < G->n.
 * @param j the vertex to which the edge enters. Should satisfy 0 <= j < G->n.
 */
void attachVertices(graph* G, int i, int j);

/**
 * Removes the edge (i,j) in the graph G.
 * @param G a pointer to a graph.
 * @param i the vertex from which the edge exists. Should satisfy 0 <= i < G->n.
 * @param j the vertex to which the edge enters. Should satisfy 0 <= j < G->n.
 */
void attachVertices(graph* G, int i, int j);

/**
 * Sets the value in the (i,j) and (j,i) entries of G's adjacency matrix.
 * @param G a pointer to a graph.
 * @param i the vertex from which the edge exists. Should satisfy 0 <= i < G->n.
 * @param j the vertex to which the edge enters. Should satisfy 0 <= j < G->n.
 * @param value should satisfy 0==value or 1==value
 */
void setEdge(graph* G, int i, int j, int value);

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

#endif /*GRAPH_H*/
