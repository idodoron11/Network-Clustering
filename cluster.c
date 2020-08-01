#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"
#include "spmat.h"
#include "VerticesGroup.h"

void randVector(double *vector, int n);

void printVector(double *vector);

int main() {
    spmat *A;
    Matrix *AMatrix;
    int i, M, n = 10, gSize = 4;
    double *vector, *s;
    int gVertices[] = {3, 4, 7, 9};
    VerticesGroup *group;
    srand(time(0));
    vector = malloc(gSize * sizeof(double));
    s = malloc(gSize * sizeof(double));
    AMatrix = createMatrix(n);
    A = generateRandomSymSpmat(n, 20, AMatrix);
    M = spmatValuesSum(A);
    group = createVerticesGroup();
    for (i = 0; i < gSize; i++) {
        addVertexToGroup(group, gVertices[i]);
    }
    calculateSubMatrix(AMatrix, M, group);
    randVector(vector, gSize);
    powerIteration(group->bHatSubMatrix, vector, s);
    printf("Edges matrix:\n");
    printSpmat(A);
    printf("\nEdges sub matrix:\n");
    printSpmat(group->edgeSubMatrix);
    printf("\nB sub matrix:\n");
    printMatrix(group->bSubMatrix);
    printf("\nB-hat sub matrix:\n");
    printMatrix(group->bHatSubMatrix);
    printf("\ns vector:\n");
    printVector(s);
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
 */
void printVector(double *vector) {
    int i;
    printf("( ");
    for (i = 0; i < (int) (sizeof(*vector) / sizeof(double)); i++) {
        printf("%.1f ", vector[i]);
    }
    printf(" )");
}
