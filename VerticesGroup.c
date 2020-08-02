#include <stdio.h>
#include <stdlib.h>
#include "VerticesGroup.h"
#include "matrix.h"
#include "spmat.h"

VerticesGroup *createVerticesGroup() {
    VerticesGroup *group = malloc(sizeof(VerticesGroup));
    group->size = 0;
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
    freeMatrix(group->bHatSubMatrix);
    group->edgeSubMatrix->free(group->edgeSubMatrix);
}

VertexNode *addVertexToGroup(VerticesGroup *group, int index) {
    VertexNode *node = malloc(sizeof(VertexNode));
    node->index = index;
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

/**
 * Create sub matrix
 * @param A edges matrix
 * @param M degrees sum
 * @param group
 * @return Fills group->bSubMatrix and group->bHatSubMatrix.
 */
void calculateSubMatrix(Matrix *A, int M, VerticesGroup *group) {
    VertexNode *node;
    double *row, expectedEdges;
    int i = 0, j;
    if (group->size != 0) {
        group->edgeSubMatrix = spmat_allocate_list(group->size);
        group->bSubMatrix = createMatrix(group->size);
        group->bHatSubMatrix = createMatrix(group->size);
        group->verticesArr = malloc(sizeof(int) * group->size);
        row = malloc(sizeof(double) * group->size);
        node = group->first;
        do {
            group->verticesArr[i] = node->index;
            i++;
            node = node->next;
        } while (node != group->first);
        for (i = 0; i < group->size; i++) {
            for (j = 0; j < group->size; j++) {
                row[j] = readVal(A, group->verticesArr[i], group->verticesArr[j]);
                /* For each vertex v: A[v][0] + ... + A[v][n-1] = deg(v) */
                expectedEdges = A->rowSums[group->verticesArr[i]] * A->rowSums[group->verticesArr[j]] / M;
                setVal(group->bSubMatrix, i, j, row[j] - expectedEdges);
                /* the case where delta(i,j)=0 */
                if (i != j) {
                    setVal(group->bHatSubMatrix, i, j, row[j] - expectedEdges);
                }
            }
            /* the case where i=j and so delta(i,j)=1. we subtract deg(i) from B[g][i][j]. */
            setVal(group->bHatSubMatrix, i, i, readVal(group->bSubMatrix, i, i) - group->bSubMatrix->rowSums[i]);
            group->edgeSubMatrix->add_row(group->edgeSubMatrix, row, i);
        }

        free(row);
    }
}

