#include <stdio.h>
#include <stdlib.h>
#include "VerticesGroup.h"
#include "matrix.h"
#include "spmat.h"

VerticesGroup *createVerticesGroup() {
    VerticesGroup *group = malloc(sizeof(VerticesGroup));
    group->verticesList = createLinkedList();
    return group;
}

void freeVerticesGroup(VerticesGroup *group) {
    freeLinkedList(group->verticesList);
    freeMatrix(group->bHatSubMatrix);
    group->edgeSubMatrix->free(group->edgeSubMatrix);
}

void addVertexToGroup(VerticesGroup *group, int index) {
    insertItem(group->verticesList, NULL, index);
}

/**
 * Adds a sequence of indices to the group.
 * @param group the group to which nodes are added.
 * @param sequence a sequence of integers, representing nodes in a graph.
 * @param length the length of the input sequence.
 */
void addSequence(VerticesGroup *group, int *sequence, int length){
    int i;
    for(i = 0; i < length; ++i)
        addVertexToGroup(group, sequence[i]);
}

int groupSize(VerticesGroup *group){
    return group->verticesList->length;
}

/**
 * Create sub matrix
 * @param A edges matrix
 * @param M degrees sum
 * @param group
 * @return Fills group->bSubMatrix and group->bHatSubMatrix.
 */
void calculateSubMatrix(Matrix *A, int M, VerticesGroup *group) {
    LinkedListNode *node, *first;
    double *row, expectedEdges;
    int i = 0, j, size = groupSize(group);
    if (size != 0) {
        group->edgeSubMatrix = spmat_allocate_list(size);
        group->bSubMatrix = createMatrix(size);
        group->bHatSubMatrix = createMatrix(size);
        group->verticesArr = malloc(sizeof(int) * size);
        row = malloc(sizeof(double) * size);
        node = first = group->verticesList->first;
        do {
            group->verticesArr[i] = node->index;
            i++;
            node = node->next;
        } while (node != first);
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
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

