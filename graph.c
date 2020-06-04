#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "graph.h"

graph* constructGraphFromInput(int n, int* adjMat){
    graph* G = (graph*) malloc(sizeof(graph));
    G->n = n;
    G->adjMat = adjMat;
    return G;
}

graph* constructEmptyGraph(int n){
    graph* G = (graph*) malloc(sizeof(graph));
    G->n = n;
    G->adjMat = (int*) calloc(n * n, sizeof(int));
    return G;
}

void destroyGraph(graph* G){
    free(G->adjMat);
    free(G);
}

void setEdge(graph* G, int i, int j, int value){
    int n = G->n;
    assert(value == 0 || value == 1);
    assert(i>=0 && i<n);
    assert(j>=0 && j<n);
    G->adjMat[i * n + j] = 1;
    G->adjMat[j * n + i] = 1;
}

void attachVertices(graph* G, int i, int j){
    setEdge(G, i, j, 1);
}

void detachVertices(graph* G, int i, int j){
    setEdge(G, i, j, 0);
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
