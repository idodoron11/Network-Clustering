#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include <math.h>

void printVect(double *vector, int length);

/**
 * Generate a square matrix object
 * @param n size of matrix is nxn
 * @return matrix
 */
Matrix *createMatrix(int n) {
    int i;
    Matrix *mat = malloc(sizeof(Matrix));
    mat->n = n;
    mat->values = malloc(sizeof(double *) * n);
    for (i = 0; i < n; i++) {
        mat->values[i] = calloc(n, sizeof(double));
    }
    mat->rowSums = calloc(n, sizeof(double));
    /* If the matrix is non-symmetric, colSum[i] doesn't necessarily have to be rowSum[i].
     * Yet, if we know the matrix is symmetric in advance, it might be a little waste of memory. */
    mat->colAbsSums = calloc(n, sizeof(double));
    mat->highestColSumIndex = 0;
    mat->isShifted = 0;
    return mat;
}

/**
 * Free all the memory allocated by a matrix
 * @param mat matrix object
 */
void freeMatrix(Matrix *mat) {
    int i;
    for (i = 0; i < mat->n; i++) {
        free(mat->values[i]);
    }
    free(mat->values);
    free(mat->rowSums);
    free(mat->colAbsSums);
    free(mat);
}

/**
 * Set value in matrix
 * @param mat
 * @param r row
 * @param c column
 * @return value
 */
void setVal(Matrix *mat, int r, int c, double val) {
    double oldVal = mat->values[r][c];
    int j;
    mat->values[r][c] = val;
    mat->rowSums[r] += val - oldVal;
    mat->colAbsSums[c] += fabs(val) - fabs(oldVal);
    if (c == mat->highestColSumIndex && fabs(val) < fabs(oldVal)) {
        for (j = 0; j < mat->n; ++j) {
            if (mat->colAbsSums[j] > mat->colAbsSums[mat->highestColSumIndex])
                mat->highestColSumIndex = j;
        }
    } else if (c != mat->highestColSumIndex && mat->colAbsSums[c] > mat->colAbsSums[mat->highestColSumIndex])
        mat->highestColSumIndex = c;
}

/**
 * Calculate matrix norm-1.
 * @param mat
 * @return norm-1 of mat, which is the maximum of colSum[i], for i=1,2,...,n.
 */
double matrixNorm1(Matrix *mat) {
    return mat->colAbsSums[mat->highestColSumIndex];
}

/**
 * Shifts or unshifts a given matrix.
 * @param mat
 * @param status should be 0 (unshift) or 1 (shift).
 */
void setMatrixShift(Matrix *mat, char status) {
    mat->isShifted = status;
}

/**
 * Returns boolean value to indicate whether or not the matrix is shifted.
 * @param mat
 * @return True iff mat is shifted, namely mat->isShifted != 0;
 */
char isMatrixShifted(Matrix *mat) {
    return mat->isShifted != 0;
}

/**
 * Read value in matrix
 * @param mat
 * @param r row
 * @param c column
 * @return value
 */
double readVal(Matrix *mat, int r, int c) {
    if (r == c && mat->isShifted != 0)
        return mat->values[r][c] + mat->colAbsSums[mat->highestColSumIndex];
    else
        return mat->values[r][c];
}

/**
 *
 * @param mat nxn matrix
 * @param vector the size n vector to multiply by
 * @param vectorResult vector of size n, should already be allocated
 */
void matrixVectorMult(Matrix *mat, double *vector, double *vectorResult) {
    int r, c;
    double sum;
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
 * @return the eigenvalue of the eigenvector 'vectorResult'.
 */
double powerIteration(Matrix *mat, double *vector, double *vectorResult) {
    int i, con = 1, originalShiftStatus = isMatrixShifted(mat);
    double vectorSize, dif, eps = 0.0001, lambda, x, y;
    setMatrixShift(mat, 1);
    while (con == 1) {
        x = y = 0;
        matrixVectorMult(mat, vector, vectorResult);
        vectorSize = 0;
        for (i = 0; i < mat->n; i++) {
            vectorSize += vectorResult[i] * vectorResult[i];
        }
        vectorSize = sqrt(vectorSize);
        con = 0;
        /* printf("\n\n\ni\tvectorResult[i]\tvectorResult[i]/vectorSize\tvector[i]\tdiff\tsum\n"); */
        for (i = 0; i < mat->n; i++) {
            /* printf("%d\t%f\t%f\t%f\t%f\t%f\n", i,vectorResult[i], vectorResult[i]/vectorSize,vector[i], fabs(vectorResult[i]/vectorSize - vector[i]), vectorResult[i]/vectorSize + vector[i]); */
            vectorResult[i] /= vectorSize;
            dif = fabs(vectorResult[i] - vector[i]);
            if (dif >= eps) {
                con = 1;
            }
            x += vector[i] * vectorResult[i];
            y += vector[i] * vector[i];
            vector[i] = vectorResult[i];

        }
    }
    setMatrixShift(mat, originalShiftStatus);

    /* compute the corresponding eigenvalue (with respect to the un-shifted B_hat) */
    lambda = x / y;
    lambda -= matrixNorm1(mat);
    return lambda;
}

/**
 * Print matrix
 * @param mat
 */
void printMatrix(Matrix *mat) {
    int i, j;
    double val;
    for (i = 0; i < mat->n; i++) {
        for (j = 0; j < mat->n; j++) {
            val = readVal(mat, i, j);
            if (val >= 0) {
                printf(" ");
            }
            printf("%.1f ", val);
        }
        printf("\n");
    }
}

/**
 * Print vector
 * @param vector
 * @param length
 */
void printVect(double *vector, int length) {
    int i;
    printf("( ");
    for (i = 0; i < length; i++) {
        printf("%f ", vector[i]);
    }
    printf(" )\n");
}
