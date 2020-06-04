#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "modularity.h"

int main() {
    graph* G = constructEmptyGraph(10);
    int g[] = {1,2,4,9};
    int i;
    double* B, *B_hat;
    attachVertices(G, 2, 4);
    attachVertices(G, 9, 1);

    B = getModularityMatrix(G);
    B_hat = getSubGraphModularityMatrix(B, 10, g, 4);

    for(i = 0; i < 10*10; ++i){
        printf("%f", B[i]);
        printf((i+1) % 10 == 0 ? "\n" : "\t");
    }
    printf("\n---------------------------\n");
    for(i = 0; i < 4*4; ++i){
        printf("%f", B_hat[i]);
        printf((i+1) % 4 == 0 ? "\n" : "\t");
    }

    destroyGraph(G);
    free(B);
    free(B_hat);

    return 0;
}
