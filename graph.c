#include <stdlib.h>
#include <stdio.h>
#include "graph.h"
#include "ErrorHandler.h"

Graph *constructGraphFromInput(char *inputFilePath) {
    Graph *G = (Graph *) malloc(sizeof(Graph));
    int n, i, k;
    int *list;
    double *row;
    FILE *graph_file = fopen(inputFilePath, "rb");
    assertMemoryAllocation(G);
    assertFileOpen(graph_file, inputFilePath);
    assertFileRead(fread(&n, sizeof(int), 1, graph_file), 1, inputFilePath);
    list = malloc(sizeof(int) * n);
    assertMemoryAllocation(list);
    G->degrees = malloc(n * sizeof(int));
    assertMemoryAllocation(list);
    G->n = n;
    G->degreeSum = 0;
    G->adjMat = spmat_allocate_list(n);

    for (i = 0; i < n; ++i) {
        row = calloc(n, sizeof(double));
        assertFileRead(fread(&k, sizeof(int), 1, graph_file), 1, inputFilePath);
        assertFileRead(fread(list, sizeof(int), k, graph_file), k, inputFilePath);
        G->degreeSum += k;
        G->degrees[i] = k;
        while (k > 0) {
            --k;
            row[list[k]] = 1;
        }
        G->adjMat->add_row(G->adjMat, row, i);
        free(row);
    }

    /* unsupported case because of division by 0 */
    if (G->degreeSum == 0) {
        throw("Degrees sum of 0 is not supported because of division by 0");
    }

    fclose(graph_file);
    free(list);

    return G;
}

/**
 * Get the expected edges between vertices i and j (matrix K)
 * @param i first vertex
 * @param j second vertex
 * @return expected edges
 */
double getExpectedEdges(Graph *G, int i, int j) {
    return (double) G->degrees[i] * G->degrees[j] / G->degreeSum;
}

void destroyGraph(Graph *G) {
    G->adjMat->free(G->adjMat);
    free(G->degrees);
    free(G);
}
