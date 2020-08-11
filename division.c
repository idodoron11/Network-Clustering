
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include "division.h"
#include "defs.h"
#include "ErrorHandler.h"
#include "quicksort.h"
#include "LinkedList.h"

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
 * Divides a group into two
 * @param group the group to split
 * @param s the eigenvector to split by
 * @param splitGroupA the first sub group, should be null
 * @param splitGroupB the second sub group, should be null
 * @return
 */
void
divideGroupByEigenvector(VerticesGroup *group, double *s, VerticesGroup **splitGroupA, VerticesGroup **splitGroupB) {
    int i;
    VertexNode *node = group->first;
    if (group->size > 0) {
        for (i = 0; i < group->size; i++) {
            if (!IS_POSITIVE(s[i])) {
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
 * Copy double array into another array
 * @param dst destination array
 * @param src source array
 * @param len number of elements to copy
 */
void copyArr(double *dst, double *src, int len) {
    int i;
    for (i = 0; i < len; i++) {
        dst[i] = src[i];
    }
}

/**
 * Maximize modularity by moving nodes between the sub groups
 * @param group a group of vertices
 * @param s the eigenvevtor, will be assigned the maximum split
 * @param initialModularity the modularity of the group
 */
double maximizeModularity(Graph *G, VerticesGroup *group, double *s, double initialModularity) {
    int i, j, maxNode;
    VertexNode *node, *maxNodeRef;
    double modularity, maxModularity, maxIterationModularity;
    double *maxS = malloc(group->size * sizeof(double));
    assertMemoryAllocation(maxS);
    copyArr(maxS, s, group->size);
    maxModularity = initialModularity;
    for (i = 0; i < group->size; i++) {
        node = group->first;
        maxNodeRef = NULL;
        for (j = 0; j < group->size; j++) {
            if (!node->hasMoved) {
                s[j] = -s[j];
                modularity = calculateModularity(G, group, s);
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
            copyArr(maxS, s, group->size);
        }
    }
    copyArr(s, maxS, group->size);
    free(maxS);
    if (maxModularity > initialModularity) {
        node = group->first;
        for (i = 0; i < group->size; i++) {
            node->hasMoved = 0;
            node = node->next;
        }
    }
    return maxModularity;
}

/**
 * Recursion of the division algorithm.
 * Divides a group into two if possible, and recursively operates on the sub groups.
 * @param G graph object
 * @param group vertices group
 * @param groupsLst a list of indivisible sub groups
 * @param vector an empty allocated array the size of the graph's vertices, used for power iteration
 * @param s an empty allocated array the size of the graph's vertices, used for storing an eigenvector
 */
void divisionAlgorithm2(Graph *G, VerticesGroup *group, double *vector, double *s, VerticesGroup **newGroupA,
                        VerticesGroup **newGroupB) {
    int i;
    double lambda, modularity, modularityAfterMax;

    calculateModularitySubMatrix(G, group);
    randVector(vector, group->size);
    lambda = powerIteration(G, group, vector, s);
    if (!IS_POSITIVE(lambda)) {
        return;
    }
    /* turn s eigenvector into +1 and -1 */
    for (i = 0; i < group->size; i++) {
        s[i] = IS_POSITIVE(s[i]) ? 1 : -1;
    }

    modularityAfterMax = calculateModularity(G, group, s);
    do {
        modularity = modularityAfterMax;
        modularityAfterMax = maximizeModularity(G, group, s, modularity);
    } while (modularityAfterMax > modularity);

    if (!IS_POSITIVE(modularityAfterMax)) {
        return;
    }

    divideGroupByEigenvector(group, s, newGroupA, newGroupB);
}

/**
 * Divide a graph into densely connected groups, forming a Community Structure
 * @param G graph object
 * @return a list of groups
 */
LinkedList *divisionAlgorithm(Graph *G) {
    int i;
    double *vector, *s;
    LinkedList *P, *O;
    VerticesGroup *group, *groupA, *groupB;
    P = createLinkedList();
    O = createLinkedList();

    /* Notice that the formula for the expected number of edges
     * between two given vertices requires division by zero if
     * G->degreeSum == 0. So we have to handle this case
     * separately. Additionally, it is clear that G->degreeSum
     * == 0 iff all vertices are isolated. Thus each one of them
     * belongs to an independent cluster.*/
    if (G->degreeSum == 0) {
        for (i = 0; i < G->n; ++i) {
            group = createVerticesGroup();
            addVertexToGroup(group, i);
            insertItem(P, group);
        }
        return P;
    }

    vector = malloc(G->n * sizeof(double));
    assertMemoryAllocation(vector);
    s = malloc(G->n * sizeof(double));
    assertMemoryAllocation(s);
    group = createVerticesGroup();
    for (i = 0; i < G->n; i++) {
        addVertexToGroup(group, i);
    }
    insertItem(P, group);
    while (P->first != NULL) {
        groupA = NULL;
        groupB = NULL;
        group = P->first->pointer;
        removeItem(P, P->first);
        divisionAlgorithm2(G, group, vector, s, &groupA, &groupB);
        if (groupA == NULL || groupB == NULL) {
            insertItem(O, group);
        } else {
            if (groupA->size == 1) {
                insertItem(O, groupA);
            } else {
                insertItem(P, groupA);
            }
            if (groupB->size == 1) {
                insertItem(O, groupB);
            } else {
                insertItem(P, groupB);
            }
        }
    }

    free(vector);
    free(s);
    return O;
}

void saveOutputToFile(LinkedList *groupLst, char *output_path) {
    FILE *output_file = fopen(output_path, "wb");
    LinkedListNode *currentNode = groupLst->first;
    VerticesGroup *currentGroup;
    int i;
    int *verticesArr;
    assertFileOpen(output_file, output_path);
    assertFileWrite(fwrite(&groupLst->length, sizeof(int), 1, output_file), 1, output_path);
    for (i = 0; i < groupLst->length; ++i) {
        currentGroup = currentNode->pointer;
        assertFileWrite(fwrite(&currentGroup->size, sizeof(int), 1, output_file), 1, output_path);
        if (currentGroup->verticesArr == NULL)
            fillVerticesArr(currentGroup);
        if (currentGroup->isVerticesArrSorted)
            assertFileWrite(fwrite(currentGroup->verticesArr, sizeof(int), currentGroup->size, output_file),
                            currentGroup->size, output_path);
        else {
            verticesArr = malloc(sizeof(int) * currentGroup->size);
            assertMemoryAllocation(verticesArr);
            memcpy(verticesArr, currentGroup->verticesArr, currentGroup->size);
            quickSortArray(verticesArr, currentGroup->size);
            assertFileWrite(fwrite(verticesArr, sizeof(int), currentGroup->size, output_file),
                            currentGroup->size, output_path);
            free(verticesArr);
        }
        currentNode = currentNode->next;
    }
    fclose(output_file);
}
