#ifndef CLUSTER_DIVISION_H
#define CLUSTER_DIVISION_H
#include "matrix.h"
#include "spmat.h"
#include "graph.h"
#include "VerticesGroup.h"
#include "LinkedList.h"

LinkedList *divisionAlgorithm(graph *G);

void randVector(double *vector, int n);

void printVector(double *vector, int length);

void divideGroupByS(VerticesGroup *group, double *s, VerticesGroup **splitGroupA, VerticesGroup **splitGroupB);

void maximizeModularity(VerticesGroup *group, double *s);

void divisionAlgRec(graph *G, VerticesGroup *group, LinkedList *groupsLst, double *vector, double *s);

void saveOutputToFile(LinkedList *groupLst, char *output_path);

#endif
