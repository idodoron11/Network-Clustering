#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "VerticesGroup.h"
#include "defs.h"
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
    mat->values[r][c] = val;
}

/**
 * Read value in matrix
 * @param mat
 * @param r row
 * @param c column
 * @return value
 */
double readVal(Matrix *mat, int r, int c) {
    return mat->values[r][c];
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
 * Print matrix Wolfram-style
 * @param mat
 */
void printMatrixPy(Matrix *mat) {
    int i, j;
    double val;
    printf("[");
    for (i = 0; i < mat->n; i++) {
        if (i > 0) {
            printf(",");
        }
        printf("[");
        for (j = 0; j < mat->n; j++) {
            val = readVal(mat, i, j);
            if (j > 0) {
                printf(",");
            }
            if (j == mat->n / 2) {
                printf("\n");
            }
            printf("%f", val);
        }
        printf("]\n");
    }
    printf("]\n");
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
