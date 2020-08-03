#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"
#include "spmat.h"
#include "graph.h"
#include "VerticesGroup.h"

void randVector(double *vector, int n);

void printVector(double *vector, int length);

int main() {
    spmat *A;
    Matrix *AMatrix;
    int M, n, gSize = 20;
    double *vector, *s, lambda;
    graph *G = constructGraphFromInput("graph.in");
    int gVertices[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    VerticesGroup *group;
    n = G->n;
    srand(time(0));
    vector = malloc(gSize * sizeof(double));
    s = malloc(gSize * sizeof(double));
    AMatrix = createMatrix(n);
    /* A = generateRandomSymSpmat(n, 20, AMatrix); */
    A = G->spAdjMat;
    AMatrix = G->adjMat;
    M = G->M;
    group = createVerticesGroup();
    addSequence(group, gVertices, gSize);
    calculateSubMatrix(AMatrix, M, group);
    randVector(vector, gSize);
    printf("Edges matrix:\n");
    printSpmat(A);
    printf("\nEdges sub matrix:\n");
    printSpmat(group->edgeSubMatrix);
    printf("\nB sub matrix:\n");
    printMatrix(group->bSubMatrix);
    printf("\nB-hat sub matrix:\n");
    printMatrix(group->bHatSubMatrix);
    printf("\nShifted B-hat sub matrix:\n");
    setMatrixShift(group->bHatSubMatrix, 1);
    printMatrix(group->bHatSubMatrix);
    setMatrixShift(group->bHatSubMatrix, 0);
    printf("\ns vector:\n");
    lambda = powerIteration(group->bHatSubMatrix, vector, s);
    printVector(s, gSize);
    printf("\nlambda: %f\n", lambda);
    return 0;
}

/**
 * Generate a random vector
 * @param vector an allocated array of size n for the vector
 * @param n the size of the vector
 */
void randVector(double *vector, int n) {
    int i;
    for (i = 0; i < n; i++) {
        *(vector + i) = rand();
    }
}

/**
 * Print vector
 * @param vector
 * @param length
 */
void printVector(double *vector, int length) {
    int i;
    printf("( ");
    for (i = 0; i < length; i++) {
        printf("%.1f ", vector[i]);
    }
    printf(" )\n");
}
