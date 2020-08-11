#ifndef CLUSTER_DIVISION_H
#define CLUSTER_DIVISION_H
#include "matrix.h"
#include "spmat.h"
#include "graph.h"
#include "VerticesGroup.h"
#include "LinkedList.h"

LinkedList *divisionAlgorithm(Graph *G);

void randVector(double *vector, int n);

void divideGroupByEigenvector(VerticesGroup *group, double *s, VerticesGroup **splitGroupA, VerticesGroup **splitGroupB);

double maximizeModularity(Graph *G, VerticesGroup *group, double *s, double initialModularity);

void divisionAlgorithm2(Graph *G, VerticesGroup *group, double *vector, double *s, VerticesGroup **newGroupA,
                             VerticesGroup **newGroupB);

void saveOutputToFile(LinkedList *groupLst, char *output_path);

#endif
