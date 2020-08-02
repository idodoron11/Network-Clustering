#ifndef _MATRIX_H
#define _MATRIX_H

typedef struct matrix {
    /* matrix size (n*n) */
    int n;
    double **values;
    double *rowSums;
    double *colSums;
    int highestColSumIndex;
    char isShifted;
} Matrix;

Matrix *createMatrix(int n);

void freeMatrix(Matrix *mat);

void setVal(Matrix *mat, int r, int c, double val);

double readVal(Matrix *mat, int r, int c);

void matrixVectorMult(Matrix *mat, double *vector, double *vectorResult);

void powerIteration(Matrix *mat, double *vector, double *vectorResult);

double matrixNorm1(Matrix *mat);

void setMatrixShift(Matrix *mat, char status);

char isMatrixShifted(Matrix *mat);

void printMatrix(Matrix *mat);

#endif
