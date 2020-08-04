#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mem.h>
#include "matrix.h"
#include "spmat.h"
#include "graph.h"
#include "VerticesGroup.h"

void randVector(double *vector, int n);

void printVector(double *vector, int length);

void divideGroupByS(VerticesGroup *group, double *s, VerticesGroup **splitGroupA, VerticesGroup **splitGroupB);

void maximizeModularity(VerticesGroup *group, double *s);

int main() {
    spmat *A;
    Matrix *AMatrix;
    VertexNode *node;
    int M, gSize = 4, i;
    double *vector, *s, lambda;
    int gVertices[] = {1, 3, 4, 9};
    VerticesGroup *group, *newGroupA = NULL, *newGroupB = NULL;
    graph *G = constructGraphFromInput("C:\\Users\\royar\\Workspace\\C projects\\cproject-cluster\\graph.in");
    srand(time(0));
    vector = malloc(gSize * sizeof(double));
    s = malloc(gSize * sizeof(double));
    /*AMatrix = createMatrix(n);
    A = generateRandomSymSpmat(n, 20, AMatrix); */
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
    divideGroupByS(group, s, &newGroupA, &newGroupB);
    node = newGroupA->first;
    printf("\nNodes in first group:\n");
    for (i = 0; i < newGroupA->size; i++) {
        printf("%d ", node->index);
        node = node->next;
    }
    node = newGroupB->first;
    printf("\nNodes in second group:\n");
    for (i = 0; i < newGroupB->size; i++) {
        printf("%d ", node->index);
        node = node->next;
    }
    maximizeModularity(group, s);
    freeVerticesGroup(newGroupA);
    freeVerticesGroup(newGroupB);
    newGroupA = NULL;
    newGroupB = NULL;
    divideGroupByS(group, s, &newGroupA, &newGroupB);
    node = newGroupA->first;
    printf("\nAfter maximization:");
    printf("\nNodes in first group:\n");
    for (i = 0; i < newGroupA->size; i++) {
        printf("%d ", node->index);
        node = node->next;
    }
    node = newGroupB->first;
    printf("\nNodes in second group:\n");
    for (i = 0; i < newGroupB->size; i++) {
        printf("%d ", node->index);
        node = node->next;
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
 * Divides a group into two
 * @param group the group to split
 * @param s the eigenvector to split by
 * @param splitGroupA the first sub group, should be null
 * @param splitGroupB the second sub group, should be null
 * @return
 */
void divideGroupByS(VerticesGroup *group, double *s, VerticesGroup **splitGroupA, VerticesGroup **splitGroupB) {
    int i;
    VertexNode *node = group->first;
    *splitGroupA = createVerticesGroup();
    if (group->size > 0) {
        for (i = 0; i < group->size; i++) {
            if (s[i] < 0) {
                if (*splitGroupB == NULL) {
                    *splitGroupB = createVerticesGroup();
                }
                addVertexToGroup(*splitGroupB, node->index);
            } else {
                addVertexToGroup(*splitGroupA, node->index);
            }
            node = node->next;
        }
    }
}

/**
 * Maximize modularity by moving nodes between the sub groups
 * @param group a group of vertices
 * @param s the eigenvevtor, will be assigned the maximum split
 */
void maximizeModularity(VerticesGroup *group, double *s) {
    int i, j, maxNode;
    VertexNode *node, *maxNodeRef;
    double modularity, maxModularity, maxIterationModularity;
    double *maxS = malloc(group->size * sizeof(double));
    memcpy(maxS, s, group->size);
    maxModularity = calculateModularity(group, s);
    for (i = 0; i < group->size; i++) {
        node = group->first;
        maxNodeRef = NULL;
        for (j = 0; j < group->size; j++) {
            if (!node->hasMoved) {
                /*TODO: is it enough to change groups? is 0 possible?*/
                s[j] = -s[j];
                modularity = calculateModularity(group, s);
                if (modularity > maxIterationModularity || maxNodeRef == NULL) {
                    maxIterationModularity = modularity;
                    maxNode = i;
                    maxNodeRef = node;
                }
                s[j] = -s[j];
            }
            node = node->next;
        }
        s[maxNode] = -s[maxNode];
        maxNodeRef->hasMoved = 1;
        if (maxIterationModularity > maxModularity) {
            maxModularity = maxIterationModularity;
            memcpy(maxS, s, group->size);
        }
    }
    memcpy(s, maxS, group->size);
}
