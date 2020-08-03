#ifndef CLUSTER_VERTICESGROUP_H
#define CLUSTER_VERTICESGROUP_H

#include "matrix.h"
#include "spmat.h"

typedef struct vertexNode {
    int index;
    struct vertexNode *next;
    struct vertexNode *prev;
} VertexNode;

typedef struct verticesGroup {
    VertexNode *first;
    int size;
    int *verticesArr;
    spmat *edgeSubMatrix;
    Matrix *bSubMatrix;
    Matrix *bHatSubMatrix;
} VerticesGroup;

VerticesGroup *createVerticesGroup();

void freeVerticesGroup(VerticesGroup *group);

VertexNode *addVertexToGroup(VerticesGroup *group, int index);

void addSequence(VerticesGroup *group, int *sequence, int length);

void calculateSubMatrix(Matrix *A, int M, VerticesGroup *group);

#endif
