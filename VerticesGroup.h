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
    spmat *edges;
    Matrix *bHatSubMatrix;
} VerticesGroup;

VerticesGroup *createVerticesGroup();

void freeVerticesGroup(VerticesGroup *group);

VertexNode *addVertexToGroup(VerticesGroup *group, int index);

#endif //CLUSTER_VERTICESGROUP_H
