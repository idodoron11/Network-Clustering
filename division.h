#ifndef CLUSTER_DIVISION_H
#define CLUSTER_DIVISION_H

#include "spmat.h"
#include "graph.h"
#include "VerticesGroup.h"
#include "LinkedList.h"

LinkedList *divisionAlgorithm(Graph *G);

void randVector(double *vector, int n);

void divideGroupByEigenvector(VerticesGroup *group, double *s, VerticesGroup **splitGroupA, VerticesGroup **splitGroupB,
                              unsigned int numberOfPositiveVertices);

double maximizeModularity(Graph *G, VerticesGroup *group, double *s, unsigned int *numberOfPositiveVertices);

void divisionAlgorithm2(Graph *G, VerticesGroup *group, double *vector, double *s, VerticesGroup **newGroupA,
                        VerticesGroup **newGroupB);

void saveOutputToFile(LinkedList *groupLst, char *output_path);

#endif
