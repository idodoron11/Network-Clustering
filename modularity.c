#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "modularity.h"
#include <math.h>

double* getModularityMatrix(graph* G){
    int n = G->n;
    int M = 2 * G->m; /* I think it should be multiplied by 2, but I'm not sure */
    double* B = (double*) malloc(n * n * sizeof(double));
    int i, j;
    assert(B != NULL);

    for(i = 0; i < n; ++i)
        for(j = 0; j < n; ++j)
            B[i*n + j] = G->adjMat[i*n + j] - (double)(getDegree(G, i) * getDegree(G, j))/M;

    return B;
}

double* getSubGraphModularityMatrix(double* const B, int n, int* const g, int k){
    /* for clarity, i'm trying to name variables in the same way they
     * were referred to in section "Dividing Into Modularity Groups" */
    int i, j;
    double* B_hat = (double*) malloc(sizeof(double) * k * k);
    double* f = (double*) calloc(k, sizeof(double));
    assert(B_hat != NULL);
    assert(f != NULL);

    /* this loop computes f[i], as defined in the instructions */
    for(i = 0; i < k; ++i)
        for(j = 0; j < k; ++j)
            f[i] += B[g[i]*n + g[j]];

    /* this loop calculates the modularity matrix with respect to the sub-graph, namely B_hat[i, j] */
    for(i = 0; i < k; ++i){
        for(j = 0; j < k; ++j){
            B_hat[i*k + j] = B[g[i]*n + g[j]];
            if(g[i] == g[j])
                B_hat[i*k + j] -= f[i];
        }
    }

    return B_hat;
}

double power_iteration(spmat sm, double *vector, double *vectorResult) {
    register int i, con = 1;
    double eigenvalue = 0;
    register double vectorSize, dif, eps = 0.00001;
    while (con) {
        sm.mult(&sm, vector, vectorResult);
        vectorSize = 0;
        for (i = 0; i < sm.n; i++) {
            vectorSize += vectorResult[i] * vectorResult[i];
        }
        vectorSize = sqrt(vectorSize);
        con = 0;
        for (i = 0; i < sm.n; i++) {
            vectorResult[i] /= vectorSize;
            dif = fabs(vectorResult[i] - vector[i]);
            if (dif >= eps) {
                con = 1;
            }
            vector[i] = vectorResult[i];
        }
    }

    /* the eigenvalue is Transpose(vectorResult)*sm*vectorResult
     * See https://www.cse.huji.ac.il/~csip/tirgul2.pdf
     * I'm using "vector" to store temporary results, since we
     * don't need it anymore. That way we can avoid allocating
     * more memory space. */

    sm.mult(&sm, vectorResult, vector);
    for(i = 0; i < sm.n; ++i){
        eigenvalue += vector[i] * vectorResult[i];
    }
    return eigenvalue;
}
