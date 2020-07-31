#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

/**
 * Generate a square matrix object
 * @param n size of matrix is nxn
 * @return matrix
 */
matrix *generateMatrix(int n) {
    int i;
    matrix *mat = malloc(sizeof(matrix));
    mat->values = malloc(sizeof(int) * n);
    for (i = 0; i < n; i++) {
        mat->values[i] = malloc(sizeof(int) * n);
    }
    return mat;
}

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

