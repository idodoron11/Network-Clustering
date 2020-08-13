#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "VerticesGroup.h"
#include "matrix.h"
#include "spmat.h"
#include "defs.h"
#include "ErrorHandler.h"

VerticesGroup *createVerticesGroup(unsigned int capacity) {
    VerticesGroup *group = malloc(sizeof(VerticesGroup));
    assertMemoryAllocation(group);
    group->verticesArr = malloc(capacity * sizeof(int));
    assertMemoryAllocation(group->verticesArr);
    group->capacity = capacity;
    group->edgeSubMatrix = NULL;
    group->modularityRowSums = NULL;
    group->modularityAbsColSum = NULL;
    group->isVerticesArrSorted = 1;
    group->size = 0;
    return group;
}

void freeVerticesGroup(VerticesGroup *group) {
    free(group->verticesArr);
    if (group->edgeSubMatrix != NULL) {
        /* the modularity was calculated, so all related data should be freed */
        group->edgeSubMatrix->free(group->edgeSubMatrix);
        free(group->modularityRowSums);
        free(group->modularityAbsColSum);
    }
    free(group);
}

void addVertexToGroup(VerticesGroup *group, int index) {
    assertBooleanStatementIsTrue(group->size < group->capacity);
    group->verticesArr[group->size] = index;
    if(group->size > 2 && group->verticesArr[group->size] < group->verticesArr[group->size - 1])
        group->isVerticesArrSorted = 0;
    ++group->size;
}

/**
 * Adds a sequence of indices to the group.
 * @param group the group to which nodes are added.
 * @param sequence a sequence of group->capacity distinct integers, representing vertices.
 */
void addSequence(VerticesGroup *group, int *sequence, int length) {
    int i;
    assertBooleanStatementIsTrue(length <= group->capacity);
    for(i = 0; i < length; ++i) {
        group->verticesArr[i] = sequence[i];
        ++group->size;
    }
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
 * Multiply the modularity matrix of a sub group of vertices by an eigenvector: s_t*B*s
 * @param group the vertices group of the modularity matrix
 * @param s eigenvector to multiply by
 * @param res the vector multiplication result, should be allocated
 * @param bothSides boolean, if set to 0 will only multiply by s on the right,
 * storing the result in res and returning the the norm of res
 * @return multiplication result (or the vector's norm if bothSides=0)
 */
double multiplyModularityByVector(Graph *G, VerticesGroup *group, double *s, double *res, int bothSides, int withNorm) {
    int i;
    double numRes = 0;
    /* the common value of all rows of the multiplication of the expectedEdges (K) matrix by s */
    double degreesCommon = 0;
    double modularityNorm1 = withNorm ? getModularityMatrixNorm1(group) : 0;
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
    numRes = multiplyModularityByVector(G, group, s, res, 1, 0);
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
        vectorNorm = multiplyModularityByVector(G, group, vector, vectorResult, 0, 1);
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
