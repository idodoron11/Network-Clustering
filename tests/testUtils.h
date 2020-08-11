#ifndef CLUSTER_TESTUTILS_H
#define CLUSTER_TESTUTILS_H

#include "../matrix.h"
#include "../LinkedList.h"

void printMatrix(Matrix *mat);

void printMatrixPy(Matrix *mat);

void printVect(double *vector, int length);

void printGroupList(LinkedList *groupList, int n);

#endif
