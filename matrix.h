#ifndef _MATRIX_H
#define _MATRIX_H

typedef struct _matrix {
    /* Matrix size (n*n) */
    int n;
    int **values;
    int *colSums;
} matrix;

matrix *generateMatrix(int n);

int readVal(matrix *mat, int r, int c);


#endif
