#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "graph.h"

graph* constructGraphFromInput(char* inputFilePath){
    graph* G = (graph*) malloc(sizeof(graph));
    int n, i, j, k;
    double* row;
    int* list;
    FILE* graph_file = fopen(inputFilePath, "rb");
    assert(graph_file != NULL);
    assert(fread(&n, sizeof(int), 1, graph_file) == 1);
    G->deg = (int*)calloc(n, sizeof(int));
    assert(G->deg != NULL);
    list = malloc(sizeof(int) * n);
    assert(list != NULL);
    G->n = n;
    G->M = 0;
    G->spAdjMat = spmat_allocate_list(n);
    G->adjMat = calloc(n*n, sizeof(double));
    assert(G->adjMat != NULL);

    for(i=0; i < n; ++i){
        assert(fread(&k, sizeof(int), 1, graph_file) == 1);
        assert(fread(list, sizeof(int), k, graph_file) == (unsigned int) k);
        G->deg[i] = k;
        G->M += k;
        row = G->adjMat + i*n;
        while(k>0){
            --k;
            j = list[k];
            row[j] = 1;
        }
        G->spAdjMat->add_row(G->spAdjMat, row, i);
    }
    fclose(graph_file);
    free(list);

    return G;
}

void destroyGraph(graph* G){
    G->spAdjMat->free(G->spAdjMat);
    free(G->deg);
    free(G->adjMat);
    free(G);
}

int getEdge(graph* G, int i, int j){
    return G->adjMat[i * G->n + j];
}

void printGraph(graph* G){
    int n = G->n;
    int i;
    for(i = 0; i < n*n; ++i){
        printf("%d", (int) G->adjMat[i]);
        printf((i+1) % n == 0 ? "\n" : "\t");
    }
}

int getDegree(graph* G, int i){
    return G->deg[i];
}
