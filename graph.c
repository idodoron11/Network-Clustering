#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "graph.h"

Graph *constructGraphFromInput(char *inputFilePath) {
    Graph *G = (Graph *) malloc(sizeof(Graph));
    int n, i, j, k;
    int *list;
    FILE *graph_file = fopen(inputFilePath, "rb");
    assert(graph_file != NULL);
    assert(fread(&n, sizeof(int), 1, graph_file) == 1);
    list = malloc(sizeof(int) * n);
    G->degrees = malloc(n * sizeof(int));
    assert(list != NULL);
    G->n = n;
    G->degreeSum = 0;
    G->spAdjMat = spmat_allocate_list(n);
    G->adjMat = createMatrix(n);
    G->expectedEdges = createMatrix(n);

    for (i = 0; i < n; ++i) {
        assert(fread(&k, sizeof(int), 1, graph_file) == 1);
        assert(fread(list, sizeof(int), k, graph_file) == (unsigned int) k);
        G->degreeSum += k;
        G->degrees[i] = k;
        while (k > 0) {
            --k;
            setVal(G->adjMat, i, list[k], 1);
        }
        G->spAdjMat->add_row(G->spAdjMat, G->adjMat->values[i], i);
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            setVal(G->expectedEdges, i, j, (double) G->degrees[i] * G->degrees[j] / G->degreeSum);
        }
    }
    fclose(graph_file);
    free(list);

    return G;
}

void destroyGraph(Graph *G) {
    G->spAdjMat->free(G->spAdjMat);
    freeMatrix(G->adjMat);
    free(G);
}

int getEdge(Graph *G, int i, int j) {
    return readVal(G->adjMat, i, j);
}

void printGraph(Graph *G) {
    printMatrix(G->adjMat);
}
