#ifndef CLUSTER_VERTICESGROUP_H
#define CLUSTER_VERTICESGROUP_H

#include "matrix.h"
#include "spmat.h"
#include "graph.h"

typedef struct vertexNode {
    int index;
    /*Has the node's group changed during modularity maximization*/
    int hasMoved;
    struct vertexNode *next;
    struct vertexNode *prev;
} VertexNode;

typedef struct verticesGroup {
    VertexNode *first;
    int size;
    int *verticesArr;
    char isVerticesArrSorted;
    spmat *edgeSubMatrix;
    double *modularityRowSums;
    double *modularityAbsColSum;
    Matrix *bHatSubMatrix;
    int highestColSumIndex;

} VerticesGroup;

VerticesGroup *createVerticesGroup();

void freeVerticesGroup(VerticesGroup *group);

VertexNode *addVertexToGroup(VerticesGroup *group, int index);

void calculateModularitySubMatrix(Graph *G, VerticesGroup *group);

double multiplyModularityByVector(Graph *G, VerticesGroup *group, double *s, double *res, int bothSides, int withNorm);

double calculateModularity(Graph *G, VerticesGroup *group, double *s);

double getModularityMatrixNorm1(VerticesGroup *group);

double powerIteration(Graph *G, VerticesGroup *group, double *vector, double *vectorResult);

void fillVerticesArr(VerticesGroup *group);

void addSequence(VerticesGroup *group, int *sequence, int length);

#endif
