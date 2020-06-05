#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "graph.h"
#include "modularity.h"

int main() {
    graph* G = constructEmptyGraph(10);
    int g[] = {1,2,4,9,5};
    int i, nnz = 0;
    double* B, *B_hat;
    spmat *sm;
    double* eigenvector = (double*) malloc(4*sizeof(double));
    double eigenvalue;
    double vector[] = {0.22,0.01,-0.87,0.33,0.11};
    attachVertices(G, 2, 4);
    attachVertices(G, 9, 1);
    assert(eigenvector != NULL);

    B = getModularityMatrix(G);
    B_hat = getSubGraphModularityMatrix(B, 10, g, 5);

    for(i = 0; i < 10*10; ++i){
        printf("%f", B[i]);
        printf((i+1) % 10 == 0 ? "\n" : "\t");
    }
    printf("\n---------------------------\n");
    for(i = 0; i < 5*5; ++i){
        printf("%f", B_hat[i]);
        printf((i+1) % 5 == 0 ? "\n" : "\t");
    }

    for(i = 0; i < 5*5; ++i)
        nnz += (B_hat[i] != 0);
    sm = spmat_allocate_array(5, nnz);
    for (i = 0; i < 5; i++) {
        (*sm).add_row(sm, &B_hat[i*5], i);
    }

    eigenvalue = power_iteration(*sm, vector, eigenvector);
    printf("eigenvalue=%f\n", eigenvalue);
    printf("eigenvector=(%f,%f,%f,%f)", eigenvector[0], eigenvector[1], eigenvector[2], eigenvector[3]);

    destroyGraph(G);
    free(B);
    free(B_hat);
    sm->free(sm);
    free(eigenvector);

    return 0;
}
