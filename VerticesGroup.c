#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "VerticesGroup.h"
#include "defs.h"
#include "ErrorHandler.h"

/**
 * Create a group of vertices
 * @param capacity amount of vertices
 * @return group
 */
VerticesGroup *createVerticesGroup(unsigned int capacity) {
    VerticesGroup *group = malloc(sizeof(VerticesGroup));
    assertMemoryAllocation(group);
    group->verticesArr = malloc(capacity * sizeof(int));
    assertMemoryAllocation(group->verticesArr);
    group->capacity = capacity;
    group->edgeSubMatrix = NULL;
    group->modularityRowSums = NULL;
    group->modularityAbsColSum = NULL;
    group->size = 0;
    return group;
}

/**
 * Free memory allocated for vertices group
 * @param group
 */
void freeVerticesGroup(VerticesGroup *group) {
    free(group->verticesArr);
    free(group);
}

/**
 * Free memory allocated for vertices group's modularity sub matrix
 * @param group
 */
void freeVerticesGroupModularitySubMatrix(VerticesGroup *group) {
    if (group->edgeSubMatrix != NULL) {
        /* the modularity was calculated, so all related data should be freed */
        group->edgeSubMatrix->free(group->edgeSubMatrix);
        free(group->modularityRowSums);
        free(group->modularityAbsColSum);
    }
}

/**
 * Add a vertex to a group
 * @param group vertices group
 * @param index the index in the graph
 */
void addVertexToGroup(VerticesGroup *group, int index) {
    assertBooleanStatementIsTrue(group->size < group->capacity);
    group->verticesArr[group->size] = index;
    ++group->size;
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
    nodeRef *rowLists, spmNode;
    int i = 0, j, con;
    if (group->size != 0) {
        rowLists = (nodeRef *) G->adjMat->private;
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
            spmNode = rowLists[group->verticesArr[i]];
            for (j = 0; j < group->size; j++) {
                con = 1;
                while (con) {
                    if (spmNode == NULL || spmNode->colind > group->verticesArr[j]) {
                        row[j] = 0;
                        con = 0;
                    } else if (spmNode->colind == group->verticesArr[j]) {
                        row[j] = 1;
                        con = 0;
                    } else {
                        spmNode = spmNode->next;
                    }
                }
                /* modularityEntry is B_hat[i][j] = A[i][j]-K[i][j], if we use the original notation. */
                modularityEntry = row[j] - getExpectedEdges(G, group->verticesArr[i], group->verticesArr[j]);
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
                group->highestColSumIndex = i;
            }
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
double multiplyModularityByVector(Graph *G, VerticesGroup *group, double *s, double *res, int bothSides, int withNorm,
                                  int withF) {
    int i;
    double numRes = 0;
    /* the common value of all rows of the multiplication of the expectedEdges (K) matrix by s */
    double degreesCommon = 0;
    double modularityNorm1 = withNorm ? getModularityMatrixNorm1(group) : 0;
    double f;

    /* multiply A by s */
    group->edgeSubMatrix->mult(group->edgeSubMatrix, s, res);
    for (i = 0; i < group->size; i++) {
        /* degreesCommon is what we sometimes called y */
        degreesCommon += (double) G->degrees[group->verticesArr[i]] * s[i];
        /* subtracting f_i[g] * s, and adding shift constant to the result. */
        f = withF ? group->modularityRowSums[i] : 0;
        res[i] += (modularityNorm1 - f) * s[i];
    }

    for (i = 0; i < group->size; i++) {
        res[i] -= (double) G->degrees[group->verticesArr[i]] * degreesCommon / G->degreeSum;
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
    numRes = multiplyModularityByVector(G, group, s, res, 1, 0, 1);
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
        vectorNorm = multiplyModularityByVector(G, group, vector, vectorResult, 0, 1, 1);
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
