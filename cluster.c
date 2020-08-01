#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"
#include "spmat.h"
#include "VerticesGroup.h"

int main() {
    spmat *A;
    Matrix *AMatrix;
    int i, M, n = 10;
    int vertices[] = {3, 4, 7, 9};
    VerticesGroup *group;
    srand(time(0));
    AMatrix = createMatrix(n);
    A = generateRandomSymSpmat(n, 20, AMatrix);
    M = spmatValuesSum(A);
    group = createVerticesGroup();
    for (i = 0; i < (int) (sizeof(vertices) / sizeof(vertices[0])); i++) {
        addVertexToGroup(group, vertices[i]);
    }
    calculateSubMatrix(AMatrix, M, group);
    printf("Edges matrix:\n");
    printSpmat(A);
    printf("\nEdges sub matrix:\n");
    printSpmat(group->edgeSubMatrix);
    printf("\nB sub matrix:\n");
    printMatrix(group->bSubMatrix);
    printf("\nB-hat sub matrix:\n");
    printMatrix(group->bHatSubMatrix);
    return 0;
}
