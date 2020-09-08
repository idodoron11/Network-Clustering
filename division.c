
#include <stdlib.h>
#include <stdio.h>
#include "division.h"
#include "defs.h"
#include "ErrorHandler.h"

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
 * @param numberOfPositiveVertices number of vertices belonging to sub group A
 * @return
 */
void
divideGroupByEigenvector(VerticesGroup *group, double *s, VerticesGroup **splitGroupA, VerticesGroup **splitGroupB,
                         unsigned int numberOfPositiveVertices) {
    int i;
    if (numberOfPositiveVertices > 0)
        *splitGroupA = createVerticesGroup(numberOfPositiveVertices);
    if (group->size - numberOfPositiveVertices > 0)
        *splitGroupB = createVerticesGroup(group->size - numberOfPositiveVertices);
    for (i = 0; i < group->size; ++i) {
        if (!IS_POSITIVE(s[i]))
            addVertexToGroup(*splitGroupB, group->verticesArr[i]);
        else
            addVertexToGroup(*splitGroupA, group->verticesArr[i]);
    }
}

/**
 * Maximize modularity by moving nodes between the sub groups
 * @param group a group of vertices
 * @param s the eigenvevtor, will be assigned the maximum split
 * @param initialModularity the modularity of the group
 * @param numberOfPositiveVertices will be assigned the number of vertices in sub group A after the division
 */
double maximizeModularity(Graph *G, VerticesGroup *group, double *s, double initialModularity,
                          unsigned int *numberOfPositiveVertices) {
    int i, j, maxNode = 0;
    double deltaModularity, maxImprovement, score;
    int *indices = malloc(group->size * sizeof(int));
    double *improve = malloc(group->size * sizeof(double));
    char *hasMoved = calloc(group->size, sizeof(char));
    int bestIteration;
    char isMaxNodeValid = 0;
    assertMemoryAllocation(indices);
    assertMemoryAllocation(improve);
    assertMemoryAllocation(hasMoved);
    maxImprovement = 0;

    for (i = 0; i < group->size; i++) {
        isMaxNodeValid = 0;
        for (j = 0; j < group->size; j++) {
            if (hasMoved[j] == 0) {
                s[j] = -s[j];
                deltaModularity = calculateModularity(G, group, s) - initialModularity;
                if (isMaxNodeValid == 0 || deltaModularity > score) {
                    score = deltaModularity;
                    maxNode = j;
                    isMaxNodeValid = 1;
                }
                s[j] = -s[j];
            }
        }
        s[maxNode] = -s[maxNode];
        *numberOfPositiveVertices += s[maxNode] == 1.0 ? 1 : (-1);
        indices[i] = maxNode;
        hasMoved[maxNode] = 1;

        if(i == 0){
            improve[i] = score;
        } else {
            improve[i] = improve[i-1] + score;
        }
        if (i == 0 || improve[i] > maxImprovement) {
            maxImprovement = improve[i];
            bestIteration = i;
        }
    }

    /* this loop moves back vertices that were moved after the best iteration */
    for (i = group->size - 1; i > bestIteration; --i) {
        j = indices[i];
        s[j] = -s[j];
        *numberOfPositiveVertices += s[j] == 1.0 ? 1 : (-1);
    }

    free(indices);
    free(hasMoved);
    free(improve);
    return maxImprovement;
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
    double lambda, modularity, maxDeltaModularity;
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

    modularity = calculateModularity(G, group, s);
    maxDeltaModularity = 0;
    do {
        modularity += maxDeltaModularity;
        maxDeltaModularity = maximizeModularity(G, group, s, modularity, &numberOfPositiveVertices);
    } while (IS_POSITIVE(maxDeltaModularity));

    if (!IS_POSITIVE(modularity)) {
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
            freeVerticesGroup(group);
        }
    }

    free(vector);
    free(s);
    deepFreeGroupList(P);
    return O;
}

/**
 * Save the list of sub groups to an output file
 * @param groupLst list of vertices groups
 * @param output_path path of output file
 */
void saveOutputToFile(LinkedList *groupLst, char *output_path) {
    FILE *output_file = fopen(output_path, "wb");
    LinkedListNode *currentNode = groupLst->first;
    VerticesGroup *currentGroup;
    int i;
    assertFileOpen(output_file, output_path);
    assertFileWrite(fwrite(&groupLst->length, sizeof(int), 1, output_file), 1, output_path);
    for (i = 0; i < groupLst->length; ++i) {
        currentGroup = currentNode->pointer;
        assertFileWrite(fwrite(&currentGroup->size, sizeof(int), 1, output_file), 1, output_path);
        assertFileWrite(fwrite(currentGroup->verticesArr, sizeof(int), currentGroup->size, output_file),
                        currentGroup->size, output_path);

        currentNode = currentNode->next;
    }
    fclose(output_file);
}
