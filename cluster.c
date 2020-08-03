#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"
#include "spmat.h"
#include "VerticesGroup.h"

void randVector(double *vector, int n);

void printVector(double *vector, int length);

VerticesGroup *divideGroupByS(VerticesGroup *group, double *s);

int main() {
    spmat *A;
    Matrix *AMatrix;
    VertexNode *node;
    int i, M, n = 50, gSize = 4;
    double *vector, *s;
    int gVertices[] = {5, 17, 32, 6};
    VerticesGroup *group, *newGroup;
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
    powerIteration(group->bHatSubMatrix, vector, s);
    printVector(s, gSize);
    newGroup = divideGroupByS(group, s);
    node = group->first;
    printf("\nNodes in first group:\n");
    if (group->size > 0) {
        do {
            printf("%d ", node->index);
            node = node->next;
        } while (node != group->first);
    }
    node = newGroup->first;
    printf("\nNodes in second group:\n");
    if (newGroup->size > 0) {
        do {
            printf("%d ", node->index);
            node = node->next;
        } while (node != newGroup->first);
    }

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

/**
 * Divides a group into two, leaving some vertices in the original group
 * and transferring the rest to a new VerticesGroup
 * @param group
 * @param s
 * @return
 */
VerticesGroup *divideGroupByS(VerticesGroup *group, double *s) {
    int createdNewGroup = 0, i, groupSize;
    VerticesGroup *newGroup;
    VertexNode *tmp, *node = group->first;
    newGroup = createVerticesGroup();
    if (group->size > 0) {
        groupSize = group->size;
        for (i = 0; i < groupSize; i++) {
            tmp = node->next;
            if (s[i] < 0) {
                createdNewGroup = 1;
                removeVertexFromGroup(group, node);
                addVertexToGroup(newGroup, node->index);
            }
            node = tmp;
        }
    }
    if (!createdNewGroup) {
        free(newGroup);
        newGroup = NULL;
    }
    return newGroup;
}
