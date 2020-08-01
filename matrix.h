#ifndef _MATRIX_H
#define _MATRIX_H

typedef struct matrix {
    /* matrix size (n*n) */
    int n;
    double **values;
    double *colSums;
} Matrix;

Matrix *createMatrix(int n);

void freeMatrix(Matrix *mat);

void setVal(Matrix *mat, int r, int c, double val);

int readVal(Matrix *mat, int r, int c);


#endif
