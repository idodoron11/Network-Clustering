#ifndef _MATRIX_H
#define _MATRIX_H

typedef struct _matrix {
    /* matrix size (n*n) */
    int n;
    double **values;
    double *colSums;
} matrix;

matrix *generateMatrix(int n);

void freeMatrix(matrix *mat);

int readVal(matrix *mat, int r, int c);


#endif
