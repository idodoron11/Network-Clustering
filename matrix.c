#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

/**
 * Generate a square matrix object
 * @param n size of matrix is nxn
 * @return matrix
 */
matrix *generateMatrix(int n) {
    int i;
    matrix *mat = malloc(sizeof(matrix));
    mat->values = malloc(sizeof(double *) * n);
    for (i = 0; i < n; i++) {
        mat->values[i] = malloc(sizeof(double) * n);
    }
    return mat;
}

/**
 * Free all the memory allocated by a matrix
 * @param mat matrix object
 */
void freeMatrix(matrix *mat) {
    int i;
    for (i = 0; i < mat->n; i++) {
        free(mat->values[i]);
    }
    free(mat->values);
    free(mat->colSums);
    free(mat);
}

/**
 * Read value in matrix
 * @param mat
 * @param r row
 * @param c column
 * @return value
 */
int readVal(matrix *mat, int r, int c) {
    return mat->values[r][c];
}

/**
 *
 * @param mat nxn matrix
 * @param vector the size n vector to multiply by
 * @param vectorResult vector of size n, should already be allocated
 */
void matrixVectorMult(matrix *mat, double *vector, double *vectorResult) {
    int r, c, sum;
    for (r = 0; r < mat->n; r++) {
        sum = 0;
        for (c = 0; c < mat->n; c++) {
            sum += readVal(mat, r, c) * vector[c];
        }
        vectorResult[r] = sum;
    }
}

/**
 * Perform the power iteration algorithm
 * @param matrix a matrix object
 * @param vector initial vector for the algorithm
 * @param vectorResult the eigenvector found by the algorithm, should be allocated
 */
void powerIteration(matrix *mat, double *vector, double *vectorResult) {
    register int i, con = 1;
    register double vectorSize, dif, eps = 0.00001;
    while (con) {
        matrixVectorMult(mat, vector, vectorResult);
        vectorSize = 0;
        for (i = 0; i < mat->n; i++) {
            vectorSize += vectorResult[i] * vectorResult[i];
        }
        vectorSize = sqrt(vectorSize);
        con = 0;
        for (i = 0; i < mat->n; i++) {
            vectorResult[i] /= vectorSize;
            dif = fabs(vectorResult[i] - vector[i]);
            if (dif >= eps) {
                con = 1;
            }
            vector[i] = vectorResult[i];
        }
    }
}

