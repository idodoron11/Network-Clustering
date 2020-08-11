#include <stdlib.h>
#include <stdio.h>
#include "graph.h"
#include "ErrorHandler.h"

Graph *constructGraphFromInput(char *inputFilePath) {
    Graph *G = (Graph *) malloc(sizeof(Graph));
    int n, i, j, k;
    int* list;
    FILE* graph_file = fopen(inputFilePath, "rb");
    assertMemoryAllocation(G);
    assertFileOpen(graph_file, inputFilePath);
    assertFileRead(fread(&n, sizeof(int), 1, graph_file), 1, inputFilePath);
    list = malloc(sizeof(int) * n);
    assertMemoryAllocation(list);
    G->degrees = malloc(n * sizeof(int));
    assertMemoryAllocation(list);
    G->n = n;
    G->degreeSum = 0;
    G->adjMat = createMatrix(n);
    G->expectedEdges = createMatrix(n);

    for(i=0; i < n; ++i){
        assertFileRead(fread(&k, sizeof(int), 1, graph_file), 1, inputFilePath);
        assertFileRead(fread(list, sizeof(int), k, graph_file), k, inputFilePath);
        G->degreeSum += k;
        G->degrees[i] = k;
        while (k > 0) {
            --k;
            setVal(G->adjMat, i, list[k], 1);
        }
    }

    if(G->degreeSum != 0) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                setVal(G->expectedEdges, i, j, (double) G->degrees[i] * G->degrees[j] / G->degreeSum);
            }
        }
    }
    fclose(graph_file);
    free(list);

    return G;
}

Graph* constructGraphFromMatrix(double* adjMatrix, int n){
    Graph* G = (Graph*) malloc(sizeof(Graph));
    int i, j;
    assertMemoryAllocation(G);
    G->degrees = malloc(n * sizeof(int));
    assertMemoryAllocation(G->degrees);
    G->n = n;
    G->degreeSum = 0;
    G->adjMat = createMatrix(n);
    G->expectedEdges = createMatrix(n);

    for(i=0; i < n; ++i){
        G->degrees[i] = 0;
        for(j=0; j < n; ++j) {
            setVal(G->adjMat, i, j, adjMatrix[i*n+j]);
            G->degrees[i] += adjMatrix[i*n+j];
        }
        G->degreeSum += G->degrees[i];
    }

    if(G->degreeSum != 0) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                setVal(G->expectedEdges, i, j, (double) G->degrees[i] * G->degrees[j] / G->degreeSum);
            }
        }
    }

    return G;
}

void destroyGraph(Graph *G) {
    freeMatrix(G->adjMat);
    free(G);
}

int getEdge(Graph *G, int i, int j) {
    return readVal(G->adjMat, i, j);
}

void printGraph(Graph *G) {
    printMatrix(G->adjMat);
}

int getDegree(Graph* G, int i){
    return G->degrees[i];
}
