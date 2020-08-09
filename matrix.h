#ifndef _MATRIX_H
#define _MATRIX_H

typedef struct matrix {
    /* matrix size (n*n) */
    int n;
    double **values;
} Matrix;

Matrix *createMatrix(int n);

void freeMatrix(Matrix *mat);

void setVal(Matrix *mat, int r, int c, double val);

double readVal(Matrix *mat, int r, int c);

void printMatrix(Matrix *mat);

void printMatrixPy(Matrix *mat);

#endif
