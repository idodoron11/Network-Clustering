#include <stdio.h>
#include <stdlib.h>
#include "VerticesGroup.h"
#include "matrix.h"
#include "spmat.h"
#include "ErrorHandler.h"

VerticesGroup *createVerticesGroup() {
    VerticesGroup *group = malloc(sizeof(VerticesGroup));
    assertMemoryAllocation(group);
    group->size = 0;
    group->edgesMinusBHatSubMatrix = NULL;
    group->edgeSubMatrix = NULL;
    group->bSubMatrix = NULL;
    group->bHatSubMatrix = NULL;
    group->isVerticesArrSorted = 0;
    return group;
}

void freeVerticesGroup(VerticesGroup *group) {
    VertexNode *temp;
    VertexNode *node = group->first;
    if (node != NULL) {
        do {
            temp = node->next;
            free(node);
            node = temp;
        } while (node != group->first);
    }
    if (group->bHatSubMatrix != NULL) {
        freeMatrix(group->bHatSubMatrix);
    }
    if (group->edgesMinusBHatSubMatrix != NULL) {
        freeMatrix(group->edgesMinusBHatSubMatrix);
    }
    if (group->edgeSubMatrix != NULL) {
        group->edgeSubMatrix->free(group->edgeSubMatrix);
    }
    free(group);
}

VertexNode *addVertexToGroup(VerticesGroup *group, int index) {
    VertexNode *node = malloc(sizeof(VertexNode));
    assertMemoryAllocation(node);
    node->index = index;
    node->hasMoved = 0;
    if (group->size != 0) {
        node->next = group->first;
        node->prev = group->first->prev;
        group->first->prev->next = node;
        group->first->prev = node;
    } else {
        group->first = node;
        node->prev = node;
        node->next = node;
    }
    group->size++;
    return node;
}

void removeVertexFromGroup(VerticesGroup *group, VertexNode *node) {
    if (group->first == node) {
        if (group->size == 1) {
            group->first = NULL;
        } else {
            group->first = node->next;
        }
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    group->size--;
    free(node);
}

/**
 * Adds a sequence of indices to the group.
 * @param group the group to which nodes are added.
 * @param sequence a sequence of integers, representing nodes in a graph.
 * @param length the length of the input sequence.
 */
void addSequence(VerticesGroup *group, int *sequence, int length) {
    int i;
    for (i = 0; i < length; ++i)
        addVertexToGroup(group, sequence[i]);
}

/**
 * Create sub matrix
 * @param A edges matrix
 * @param M degrees sum
 * @param group
 * @return Fills group->bSubMatrix and group->bHatSubMatrix.
 */
void calculateSubMatrix(Matrix *A, int M, VerticesGroup *group) {
    double *row, expectedEdges;
    int i = 0, j;
    if (group->size != 0) {
        group->edgeSubMatrix = spmat_allocate_list(group->size);
        group->edgesMinusBHatSubMatrix = createMatrix(group->size);
        group->bSubMatrix = createMatrix(group->size);
        group->bHatSubMatrix = createMatrix(group->size);
        row = malloc(sizeof(double) * group->size);
        assertMemoryAllocation(row);
        fillVerticesArr(group);
        for (i = 0; i < group->size; i++) {
            for (j = 0; j < group->size; j++) {
                row[j] = readVal(A, group->verticesArr[i], group->verticesArr[j]);
                /* For each vertex v: A[v][0] + ... + A[v][n-1] = deg(v) */
                expectedEdges = A->rowSums[group->verticesArr[i]] * A->rowSums[group->verticesArr[j]] / M;
                setVal(group->edgesMinusBHatSubMatrix, i, j, -expectedEdges);
                setVal(group->bSubMatrix, i, j, row[j] - expectedEdges);
                /* the case where delta(i,j)=0 */
                if (i != j) {
                    setVal(group->bHatSubMatrix, i, j, row[j] - expectedEdges);
                }
            }
            /* the case where i=j and so delta(i,j)=1. we subtract deg(i) from B[g][i][j]. */
            setVal(group->bHatSubMatrix, i, i, readVal(group->bSubMatrix, i, i) - group->bSubMatrix->rowSums[i]);
            setVal(group->edgesMinusBHatSubMatrix, i, i,
                   readVal(group->edgesMinusBHatSubMatrix, i, i) - group->bSubMatrix->rowSums[i]);
            group->edgeSubMatrix->add_row(group->edgeSubMatrix, row, i);
        }

        free(row);
    }
}

void fillVerticesArr(VerticesGroup *group){
    VertexNode *node = group->first;
    int i = 0;
    char isSorted = 1;
    int prevIndex = node->index;
    group->verticesArr = malloc(sizeof(int) * group->size);
    assertMemoryAllocation(group->verticesArr);
    do {
        if(node->index < prevIndex)
            isSorted = 0;
        group->verticesArr[i] = node->index;
        ++i;
        node = node->next;
    } while (node != group->first);
    group->isVerticesArrSorted = isSorted;
}

/**
 * Calculate modularity delta of group's division given by an eigenvector
 * @param group
 * @param s eigenvector
 * @return
 */
double calculateModularity(VerticesGroup *group, double *s) {
    double *res, numRes;
    res = malloc(group->size * sizeof(double));
    assertMemoryAllocation(res);
    group->edgeSubMatrix->mult(group->edgeSubMatrix, s, res);
    numRes = vectorMult(s, res, group->size);
    matrixVectorMult(group->edgesMinusBHatSubMatrix, s, res);
    numRes -= vectorMult(s, res, group->size);
    numRes *= 0.5;
    free(res);
    return numRes;
}
