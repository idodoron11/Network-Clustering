#ifndef CLUSTER_DIVISION_H
#define CLUSTER_DIVISION_H
#include "matrix.h"
#include "spmat.h"
#include "graph.h"
#include "VerticesGroup.h"
#include "LinkedList.h"

LinkedList *divisionAlgorithm(Graph *G);

void randVector(double *vector, int n);

void printVector(double *vector, int length);

void divideGroupByEigenvector(VerticesGroup *group, double *s, VerticesGroup **splitGroupA, VerticesGroup **splitGroupB);

void maximizeModularity(Graph *G, VerticesGroup *group, double *s);

void divisionAlgRec(Graph *G, VerticesGroup *group, LinkedList *groupsLst, double *vector, double *s);

#endif
