
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include "division.h"
#include "defs.h"
#include "ErrorHandler.h"
#include "quicksort.h"
#include "tests/testUtils.h"

/**
 * Generate a random vector
 * @param vector an allocated array of capacity n for the vector
 * @param n the capacity of the vector
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
divideGroupByEigenvector(VerticesGroup *group, double *s, VerticesGroup **splitGroupA, VerticesGroup **splitGroupB,
                         unsigned int numberOfPositiveVertices) {
    int i;
    if(numberOfPositiveVertices > 0)
        *splitGroupA = createVerticesGroup(numberOfPositiveVertices);
    if(group->size - numberOfPositiveVertices > 0)
        *splitGroupB = createVerticesGroup(group->size - numberOfPositiveVertices);
    for(i = 0; i < group->size; ++i){
        if (!IS_POSITIVE(s[i]))
            addVertexToGroup(*splitGroupB, group->verticesArr[i]);
        else
            addVertexToGroup(*splitGroupA, group->verticesArr[i]);
    }
}

/**
 * Copy double array into another array
 * @param dst destination array
 * @param src source array
 * @param len number of elements to copy
 */
unsigned int copyArr(double *dst, double *src, int len) {
    int i;
    unsigned int counter = 0;
    for (i = 0; i < len; i++) {
        dst[i] = src[i];
        counter += (dst[i] == 1);
    }
    return counter;
}

/**
 * Maximize modularity by moving nodes between the sub groups
 * @param group a group of vertices
 * @param s the eigenvevtor, will be assigned the maximum split
 * @param initialModularity the modularity of the group
 */
double maximizeModularity(Graph *G, VerticesGroup *group, double *s, double initialModularity,
                          unsigned int *numberOfPositiveVertices) {
    int i, j, maxNode = 0;
    double modularity, maxModularity, maxIterationModularity;
    double *maxS = malloc(group->size * sizeof(double));
    char *hasMoved = calloc(group->size, sizeof(char));
    assertMemoryAllocation(maxS);
    assertMemoryAllocation(hasMoved);
    copyArr(maxS, s, group->size);
    maxModularity = initialModularity;
    for (i = 0; i < group->size; i++) {
        for (j = 0; j < group->size; j++) {
            if (hasMoved[j] == 0) {
                s[j] = -s[j];
                modularity = calculateModularity(G, group, s);
                if (modularity > maxIterationModularity) {
                    maxIterationModularity = modularity;
                    maxNode = j;
                }
                s[j] = -s[j];
            }
        }
        s[maxNode] = -s[maxNode];
        hasMoved[maxNode] = 1;
        if (maxIterationModularity > maxModularity) {
            maxModularity = maxIterationModularity;
            *numberOfPositiveVertices = copyArr(maxS, s, group->size);
        }
    }
    copyArr(s, maxS, group->size);
    free(maxS);
    /* TODO: isn't maxModularity always greater or equal to initialModularity? */
    if (maxModularity > initialModularity) {
        for (i = 0; i < group->size; i++) {
            hasMoved[i] = 0;
        }
    }
    return maxModularity;
}

/**
 * Divide a group into two.
 * @param G graph object
 * @param group vertices group
 * @param vector an empty allocated array the capacity of the graph's vertices, used for power iteration
 * @param s an empty allocated array the capacity of the graph's vertices, used for storing an eigenvector
 * @param newGroupA the first divided group will be assigned to this parameter (or left as NULL)
 * @param newGroupB the second divided group will be assigned to this parameter (or left as NULL)
 */
void divisionAlgorithm2(Graph *G, VerticesGroup *group, double *vector, double *s, VerticesGroup **newGroupA,
                        VerticesGroup **newGroupB) {
    int i;
    double lambda, modularity, modularityAfterMax;
    unsigned int numberOfPositiveVertices = 0;

    calculateModularitySubMatrix(G, group);
    randVector(vector, group->size);
    lambda = powerIteration(G, group, vector, s);
    if (!IS_POSITIVE(lambda)) {
        return;
    }
    /* turn s eigenvector into +1 and -1 */
    for (i = 0; i < group->size; i++) {
        s[i] = IS_POSITIVE(s[i]) ? 1 : -1;
        numberOfPositiveVertices += (s[i] == 1);
    }

    modularityAfterMax = calculateModularity(G, group, s);
    do {
        modularity = modularityAfterMax;
        modularityAfterMax = maximizeModularity(G, group, s, modularity, &numberOfPositiveVertices);
    } while (modularityAfterMax > modularity);

    if (!IS_POSITIVE(modularityAfterMax)) {
        return;
    }

    divideGroupByEigenvector(group, s, newGroupA, newGroupB, numberOfPositiveVertices);
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
     * separately. In this case, there is no need to divide to groups
     * since the max modularity is 0.
     * TODO: Are you sure about that? Intuitively, if all vertices are isolated,
     *  I would expect each of them to be in a separate group. */
    if (G->degreeSum == 0) {
        group = createVerticesGroup(G->n);
        insertItem(O, group);
        for (i = 0; i < G->n; ++i) {
            addVertexToGroup(group, i);
        }
        return O;
    }

    vector = malloc(G->n * sizeof(double));
    assertMemoryAllocation(vector);
    s = malloc(G->n * sizeof(double));
    assertMemoryAllocation(s);
    group = createVerticesGroup(G->n);
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
        if (currentGroup->isVerticesArrSorted) {
            assertFileWrite(fwrite(currentGroup->verticesArr, sizeof(int), currentGroup->size, output_file),
                            currentGroup->size, output_path);
        } else {
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
