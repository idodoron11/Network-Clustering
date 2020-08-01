#include <stdio.h>
#include <stdlib.h>
#include "VerticesGroup.h"
#include "matrix.h"
#include "spmat.h"

VerticesGroup *createVerticesGroup() {
    VerticesGroup *group = malloc(sizeof(VerticesGroup));
    return group;
}

void freeVerticesGroup(VerticesGroup *group) {
    VertexNode *temp;
    VertexNode *node = group->first;
    if (node != NULL) {
        do {
            temp = node->next;
            free(node);
            node = temp;
        } while (node != group->first);
    }
    freeMatrix(group->bHatSubMatrix);
    group->edgeSubMatrix->free(group->edgeSubMatrix);
}

VertexNode *addVertexToGroup(VerticesGroup *group, int index) {
    VertexNode *node = malloc(sizeof(VertexNode));
    node->index = index;
    if (group->first != NULL) {
        node->next = group->first;
        node->prev = group->first->prev;
        group->first->prev->next = node;
        group->first->prev = node;
    } else {
        group->first = node;
        node->prev = node;
        node->next = node;
    }
    return node;
}

