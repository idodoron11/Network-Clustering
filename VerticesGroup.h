#ifndef CLUSTER_VERTICESGROUP_H
#define CLUSTER_VERTICESGROUP_H

#include "matrix.h"
#include "spmat.h"
#include "LinkedList.h"

typedef struct verticesGroup {
    int *verticesArr;
    spmat *edgeSubMatrix;
    Matrix *bSubMatrix;
    Matrix *bHatSubMatrix;
    struct _LinkedList *verticesList;
} VerticesGroup;

VerticesGroup *createVerticesGroup();

void freeVerticesGroup(VerticesGroup *group);

void addVertexToGroup(VerticesGroup *group, int index);

void addSequence(VerticesGroup *group, int *sequence, int length);

void calculateSubMatrix(Matrix *A, int M, VerticesGroup *group);

int groupSize(VerticesGroup *group);

#endif
