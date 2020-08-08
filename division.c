
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include "division.h"

/**
 * Generate a random vector
 * @param vector an allocated array of size n for the vector
 * @param n the size of the vector
 */
void randVector(double *vector, int n) {
    int i;
    for (i = 0; i < n; i++) {
        *(vector + i) = rand();
    }
}

/**
 * Print vector
 * @param vector
 * @param length
 */
void printVector(double *vector, int length) {
    int i;
    printf("( ");
    for (i = 0; i < length; i++) {
        printf("%.1f ", vector[i]);
    }
    printf(" )\n");
}

/**
 * Divides a group into two
 * @param group the group to split
 * @param s the eigenvector to split by
 * @param splitGroupA the first sub group, should be null
 * @param splitGroupB the second sub group, should be null
 * @return
 */
void divideGroupByS(VerticesGroup *group, double *s, VerticesGroup **splitGroupA, VerticesGroup **splitGroupB) {
    int i;
    VertexNode *node = group->first;
    if (group->size > 0) {
        for (i = 0; i < group->size; i++) {
            if (s[i] < 0) {
                if (*splitGroupB == NULL) {
                    *splitGroupB = createVerticesGroup();
                }
                addVertexToGroup(*splitGroupB, node->index);
            } else {
                if (*splitGroupA == NULL) {
                    *splitGroupA = createVerticesGroup();
                }
                addVertexToGroup(*splitGroupA, node->index);
            }
            node = node->next;
        }
    }
}

/**
 * Maximize modularity by moving nodes between the sub groups
 * @param group a group of vertices
 * @param s the eigenvevtor, will be assigned the maximum split
 */
void maximizeModularity(VerticesGroup *group, double *s) {
    int i, j, maxNode;
    VertexNode *node, *maxNodeRef;
    double modularity, maxModularity, maxIterationModularity;
    double *maxS = malloc(group->size * sizeof(double));
    memcpy(maxS, s, group->size);
    maxModularity = calculateModularity(group, s);
    for (i = 0; i < group->size; i++) {
        node = group->first;
        maxNodeRef = NULL;
        for (j = 0; j < group->size; j++) {
            if (s[j] == 0)
                s[j] = 0.0001;
            if (!node->hasMoved) {
                s[j] = -s[j];
                modularity = calculateModularity(group, s);
                if (modularity > maxIterationModularity || maxNodeRef == NULL) {
                    maxIterationModularity = modularity;
                    maxNode = j;
                    maxNodeRef = node;
                }
                s[j] = -s[j];
            }
            node = node->next;
        }
        s[maxNode] = -s[maxNode];
        maxNodeRef->hasMoved = 1;
        if (maxIterationModularity > maxModularity) {
            maxModularity = maxIterationModularity;
            memcpy(maxS, s, group->size);
        }
    }
    memcpy(s, maxS, group->size);
}

void divisionAlgRec(graph *G, VerticesGroup *group, LinkedList *groupsLst, double *vector, double *s) {
    VerticesGroup *newGroupA = NULL, *newGroupB = NULL;
    double lambda;
    if (group->size == 1) {
        insertItem(groupsLst, group, 0);
        return;
    }
    calculateSubMatrix(G->adjMat, G->M, group);
    randVector(vector, group->size);
    lambda = powerIteration(group->bHatSubMatrix, vector, s);
    if (lambda <= 0) {
        insertItem(groupsLst, group, 0);
        return;
    }
    /* here, i think we should split into cases depending on the eigenvalue returned by powerIteration.
     * i'm currently leaving it as it is since it is not entirely clear. */
    maximizeModularity(group, s);
    divideGroupByS(group, s, &newGroupA, &newGroupB);
    if (newGroupA == NULL || newGroupB == NULL) {
        insertItem(groupsLst, group, 0);
    } else {
        divisionAlgRec(G, newGroupA, groupsLst, vector, s);
        divisionAlgRec(G, newGroupB, groupsLst, vector, s);
    }
}

LinkedList *divisionAlgorithm(graph *G) {
    int i;
    double *vector, *s;
    LinkedList *groupsLst;
    VerticesGroup *group;
    vector = malloc(G->n * sizeof(double));
    s = malloc(G->n * sizeof(double));
    group = createVerticesGroup();
    for (i = 0; i < G->n; i++) {
        addVertexToGroup(group, i);
    }
    groupsLst = createLinkedList();
    divisionAlgRec(G, group, groupsLst, vector, s);
    return groupsLst;
}
