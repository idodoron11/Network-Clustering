#include "graph.h"

#ifndef CLUSTER_MODULARITY_H
#define CLUSTER_MODULARITY_H

/**
 * This function dynamically allocates memory, so be sure to free it when you're done.
 * @param G an undirected graph with n vertices (where n is considered as a parameter).
 * @return a new double vector of size n*n, representing the modularity matrix.
 */
double* getModularityMatrix(graph* G);

/**
 * This function dynamically allocates memory, so be sure to free it when you're done.
 * @param fullModularityMatrix the modularity matrix of G, produced by "getModularityMatrix".
 * @param n the dimension of the squared matrix fullModularityMatrix.
 * @param g a subset of {0,1,...,n-1}, represented by an array of size k. It is assumed to be sorted.
 * @param k the length of the array g
 * @return The modularity matrix (B hat) with respect to the sub-graph defined by g.
 */
double* getSubGraphModularityMatrix(double* const fullModularityMatrix, int n, int* const g, int k);

#endif
