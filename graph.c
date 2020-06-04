#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "graph.h"

graph* constructGraphFromInput(int n, int m, int* adjMat){
    graph* G = (graph*) malloc(sizeof(graph));
    int i, j, sum;
    int* neighborsVector;
    G->n = n;
    G->m  = m;
    G->adjMat = adjMat;
    G->degrees = (int*) malloc(n * sizeof(int));

    /* updates G->degrees to include each the degree of each vertex */
    for(i = 0; i < n; ++i){
        sum = 0;
        neighborsVector = adjMat + (i*n);
        for(j = 0; j < n; ++j)
            sum += neighborsVector[j];
        G->degrees[i] = sum;
    }

    return G;
}

graph* constructEmptyGraph(int n){
    graph* G = (graph*) malloc(sizeof(graph));
    G->n = n;
    G->m = 0;
    G->adjMat = (int*) calloc(n * n, sizeof(int));
    G->degrees = (int*) calloc(n, sizeof(int));
    return G;
}

void destroyGraph(graph* G){
    free(G->adjMat);
    free(G);
}

char setEdge(graph* G, int i, int j, int value){
    int n = G->n;
    assert(value == 0 || value == 1);
    assert(i>=0 && i<n);
    assert(j>=0 && j<n);
    if(G->adjMat[i * n + j] == value)
        return 0;
    else {
        G->adjMat[i * n + j] = value;
        G->adjMat[j * n + i] = value;
        return 1;
    }
}

void attachVertices(graph* G, int i, int j){
    int change = setEdge(G, i, j, 1);
    G->m += change;
    G->degrees[i] += change;
    G->degrees[j] += change;
}

void detachVertices(graph* G, int i, int j){
    int change = setEdge(G, i, j, 0);
    G->m -= change;
    G->degrees[i] -= change;
    G->degrees[j] -= change;
}

int getEdge(graph* G, int i, int j){
    return G->adjMat[i * G->n + j];
}

void printGraph(graph* G){
    int n = G->n;
    int i;
    for(i = 0; i < n*n; ++i){
        printf("%d", G->adjMat[i]);
        printf((i+1) % n == 0 ? "\n" : "\t");
    }
}

int getDegree(graph* G, int i){
    return G->degrees[i];
}
