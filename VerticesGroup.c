#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "VerticesGroup.h"
#include "matrix.h"
#include "spmat.h"
#include "defs.h"
#include "ErrorHandler.h"

VerticesGroup *createVerticesGroup() {
    VerticesGroup *group = malloc(sizeof(VerticesGroup));
    assertMemoryAllocation(group);
    group->size = 0;
    group->edgeSubMatrix = NULL;
    group->modularityRowSums = NULL;
    group->modularityAbsColSum = NULL;
    group->verticesArr = NULL;
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

    if (group->edgeSubMatrix != NULL) {
        /* the modularity was calculated, so all related data should be freed */
        group->edgeSubMatrix->free(group->edgeSubMatrix);
        free(group->modularityRowSums);
        free(group->modularityAbsColSum);
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
 * Get the 1-norm of the modularity matrix
 * @param group vertices group
 * @return 1-norm
 */
double getModularityMatrixNorm1(VerticesGroup *group) {
    return group->modularityAbsColSum[group->highestColSumIndex];
}

/**
 * Calculate the modularity sub matrix in the VerticesGroup object
 * @param G graph object
 * @param group vertices group containing the modularity sub matrix
 */
void calculateModularitySubMatrix(Graph *G, VerticesGroup *group) {
    double *row, modularityEntry;
    int i = 0, j;
    if (group->size != 0) {
        group->edgeSubMatrix = spmat_allocate_list(group->size);
        assertMemoryAllocation(group->edgeSubMatrix);
        group->modularityRowSums = calloc(group->size, sizeof(double));
        assertMemoryAllocation(group->modularityRowSums);
        group->modularityAbsColSum = calloc(group->size, sizeof(double));
        assertMemoryAllocation(group->modularityAbsColSum);
        group->highestColSumIndex = 0;
        row = malloc(sizeof(double) * group->size);
        assertMemoryAllocation(row);
        fillVerticesArr(group);
        for (i = 0; i < group->size; i++) {
            for (j = 0; j < group->size; j++) {
                row[j] = readVal(G->adjMat, group->verticesArr[i], group->verticesArr[j]);
                /* modularityEntry is B_hat[i][j] = A[i][j]-K[i][j], if we use the original notation. */
                modularityEntry = row[j] - readVal(G->expectedEdges, group->verticesArr[i], group->verticesArr[j]);
                group->modularityRowSums[i] += modularityEntry;
                if (i != j) {
                    /* the case of non diagonal values.
                     * the modularity matrix is symmetric, so we can add row sums instead of columns */
                    group->modularityAbsColSum[i] += fabs(modularityEntry);
                } else {
                    /* handle diagonal values by subtracting the row sum (f).
                     * Notice that modularityAbsColSum = B_hat[i][0] + ... + B_hat[i][n-1].
                     * It does not equal to B[i][0] + ... + B[i][n-1] as the name suggests. */
                    group->modularityAbsColSum[i] += fabs(modularityEntry - group->modularityRowSums[i]);
                }
            }
            if (group->modularityAbsColSum[i] >= getModularityMatrixNorm1(group)) {
                /* replace highest column absolute sum
                 * TODO:    I think it is a good idea to require group->modularityAbsColSum[i] to be
                 *          strictly greater than getModularityMatrixNorm1(group), to avoid unnecessary
                 *          changes in group->highestColSumIndex. */
                group->highestColSumIndex = i;
            }
            /* TODO: improve instead of adding row at a time */
            group->edgeSubMatrix->add_row(group->edgeSubMatrix, row, i);
        }

        free(row);
    }
}

/**
 * This function allocates space for group->verticesArr
 * and fills it up with the group's vertices. It also
 * tracks whether or not the array is considered sorted.
 * (otherwise, we'll have to sort it later).
 * @param group a non-empty VerticesGroup.
 */
void fillVerticesArr(VerticesGroup *group) {
    VertexNode *node = group->first;
    int i = 0;
    char isSorted = 1;
    int prevIndex = node->index;
    group->verticesArr = malloc(sizeof(int) * group->size);
    assertMemoryAllocation(group->verticesArr);
    do {
        if (node->index < prevIndex)
            isSorted = 0;
        group->verticesArr[i] = node->index;
        ++i;
        node = node->next;
    } while (node != group->first);
    group->isVerticesArrSorted = isSorted;
}

/**
 * Multiply the modularity matrix of a sub group of vertices by an eigenvector: s_t*B*s
 * @param group the vertices group of the modularity matrix
 * @param s eigenvector to multiply by
 * @param res the vector multiplication result, should be allocated
 * @param bothSides boolean, if set to 0 will only multiply by s on the right,
 * storing the result in res and returning the the norm of res
 * @return multiplication result (or the vector's norm if bothSides=0)
 */
double multiplyModularityByVector(Graph *G, VerticesGroup *group, double *s, double *res, int bothSides) {
    int i;
    double numRes = 0;
    /* the common value of all rows of the multiplication of the expectedEdges (K) matrix by s */
    double degreesCommon = 0, modularityNorm1 = getModularityMatrixNorm1(group);
    if (G->degreeSum == 0) {
        return 0;
    }
    group->edgeSubMatrix->mult(group->edgeSubMatrix, s, res);
    for (i = 0; i < group->size; i++) {
        degreesCommon += G->degrees[i] * s[i];
        res[i] += (modularityNorm1 - group->modularityRowSums[i]) * s[i];
    }
    for (i = 0; i < group->size; i++) {
        res[i] -= G->degrees[i] * degreesCommon / G->degreeSum;
        if (bothSides) {
            numRes += s[i] * res[i];
        } else {
            numRes += res[i] * res[i];
        }
    }
    if (!bothSides) {
        /* if the result is a vector, we return its norm */
        numRes = sqrt(numRes);
    }
    return numRes;
}

/**
 * Calculate modularity delta of group's division given by an eigenvector
 * @param group
 * @param s eigenvector
 * @return modularity
 */
double calculateModularity(Graph *G, VerticesGroup *group, double *s) {
    double *res, numRes;
    res = malloc(group->size * sizeof(double));
    assertMemoryAllocation(res);
    numRes = multiplyModularityByVector(G, group, s, res, 1);
    numRes *= 0.5;
    free(res);
    return numRes;
}

/**
 * Perform the power iteration algorithm
 * @param group a vertices group, containing the modularity sub matrix
 * @param vector initial vector for the algorithm
 * @param vectorResult the eigenvector found by the algorithm, should be allocated
 * @return the eigenvalue of the eigenvector 'vectorResult'.
 */
double powerIteration(Graph *G, VerticesGroup *group, double *vector, double *vectorResult) {
    int i, con = 1;
    double vectorNorm, dif, lambda, x, y;
    while (con) {
        x = y = 0;
        vectorNorm = multiplyModularityByVector(G, group, vector, vectorResult, 0);
        con = 0;
        for (i = 0; i < group->size; i++) {
            x += vector[i] * vectorResult[i];
            y += vector[i] * vector[i];
            vectorResult[i] /= vectorNorm;
            dif = fabs(vectorResult[i] - vector[i]);
            if (IS_POSITIVE(dif)) {
                con = 1;
            }
            vector[i] = vectorResult[i];
        }
    }

    /* compute the corresponding eigenvalue (with respect to the un-shifted B_hat) */
    lambda = x / y;
    lambda -= getModularityMatrixNorm1(group);
    return lambda;
}
