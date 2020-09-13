#ifndef CLUSTER_TESTUTILS_H
#define CLUSTER_TESTUTILS_H

#include "../matrix.h"
#include "../LinkedList.h"

void printMatrix(Matrix *mat);

void printMatrixPy(Matrix *mat);

void printVect(double *vector, int length);

void printGroupList(LinkedList *groupList, int n);

void printGroupList2(LinkedList *groupList);

double calculateDivisionModularity(Graph *G, LinkedList *groupLst);

double calculateModularityOfGroup(Graph *G, VerticesGroup *group);

void compareExpected(char *inputPath, LinkedList *lst, int *expected);

/**
 * Creates a nee graph object from an input matrix.
 * @param adjMatrix the adjacency matrix given as an input.
 * @param n the number of vertices.
 * @return a reference to a new graph object, defined by the given adjacency matrix.
 */
Graph* constructGraphFromMatrix(double* adjMatrix, int n);

Graph *constructGraphFromAdjMat(Matrix *mat);

spmat *generateRandomSymSpmat(int n, double percent, Matrix *mat);

void addSequence(VerticesGroup *group, int *sequence, int length);

double drand(double low, double high);

double readSpmVal(spmat *spm, int r, int c);

#endif
