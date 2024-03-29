
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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
 * Find increasing index elements in row of sparse matrix
 * @param spmNode node of sparse matrix
 * @param col column of sparse matrix
 * @return value
 */
double loopFindSpmValue(nodeRef spmNode, int col) {
    double spmValue;
    int con = 1;
    /* this while loop essentially finds the (i,j) value
     * in the sparse adjacency matrix of the sub-graph */
    while (con) {
        if (spmNode == NULL || spmNode->colind > col) {
            spmValue = 0;
            con = 0;
        } else if (spmNode->colind == col) {
            spmValue = 1;
            con = 0;
        } else {
            spmNode = spmNode->next;
        }
    }
    return spmValue;
}

/**
 * Maximize modularity by moving nodes between the sub groups
 * @param group a group of vertices
 * @param s the eigenvevtor, will be assigned the maximum split
 * @param numberOfPositiveVertices will be assigned the number of vertices in sub group A after the division
 */
double maximizeModularity(Graph *G, VerticesGroup *group, double *s, unsigned int *numberOfPositiveVertices) {
    nodeRef *adjRows, spmNode;
    double bestImprovement = 0, improve, modularity, maxScore, spmValue, bValue;
    int iteration, i, maxNode, prevMaxNode, bestIteration, isMaxSet, isSetBestImprovement;
    char *hasMoved;
    int *indices;
    double *score;
    double *x;

    hasMoved = calloc(group->size, sizeof(char));
    assertMemoryAllocation(hasMoved);
    indices = malloc(group->size * sizeof(int));
    assertMemoryAllocation(indices);
    score = malloc(group->size * sizeof(double));
    assertMemoryAllocation(score);
    x = malloc(group->size * sizeof(double));
    assertMemoryAllocation(x);
    adjRows = group->edgeSubMatrix->private;

    do {
        multiplyModularityByVector(G, group, s, x, 0, 0, 0);

        bestImprovement = 0;
        improve = 0;
        bestIteration = -1;
        isSetBestImprovement = 0;
        for (iteration = 0; iteration < group->size; iteration++) {
            isMaxSet = 0;
            for (i = 0; i < group->size; i++) {
                if (!hasMoved[i]) {
                    if (iteration == 0) {
                        score[i] = -2 *
                                   (s[i] * x[i] +
                                    pow(G->degrees[group->verticesArr[i]], 2) /
                                    G->degreeSum);
                    } else {
                        spmNode = adjRows[i];
                        spmValue = loopFindSpmValue(spmNode, prevMaxNode);
                        bValue = spmValue - getExpectedEdges(G, group->verticesArr[i], group->verticesArr[prevMaxNode]);
                        score[i] -= 4 * s[i] * s[prevMaxNode] * bValue;
                    }

                    if (!isMaxSet || score[i] > maxScore) {
                        maxScore = score[i];
                        maxNode = i;
                        isMaxSet = 1;
                    }
                }
            }
            s[maxNode] = -s[maxNode];
            prevMaxNode = maxNode;
            hasMoved[maxNode] = 1;
            indices[iteration] = maxNode;

            improve += maxScore;
            if (!isSetBestImprovement || improve > bestImprovement) {
                bestIteration = iteration;
                bestImprovement = improve;
                isSetBestImprovement = 1;
            }
        }

        *numberOfPositiveVertices = 0;
        for (iteration = 0; iteration < group->size; iteration++) {
            i = indices[iteration];
            hasMoved[iteration] = 0;
            if (iteration > bestIteration) {
                s[i] = -s[i];
            }
            *numberOfPositiveVertices += (s[i] == 1);
        }
    } while (bestIteration != group->size - 1 && IS_POSITIVE(bestImprovement));

    free(indices);
    free(hasMoved);
    free(score);
    free(x);

    modularity = calculateModularity(G, group, s);
    return modularity;
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
    double lambda, modularity;
    unsigned int numberOfPositiveVertices = 0;

    calculateModularitySubMatrix(G, group);
    randVector(vector, group->size);
    lambda = powerIteration(G, group, vector, s);
    if (IS_POSITIVE(lambda)) {
        /* turn s eigenvector into +1 and -1 */
        for (i = 0; i < group->size; i++) {
            s[i] = IS_POSITIVE(s[i]) ? 1 : -1;
        }

        modularity = maximizeModularity(G, group, s, &numberOfPositiveVertices);

        if (IS_POSITIVE(modularity)) {
            divideGroupByEigenvector(group, s, newGroupA, newGroupB, numberOfPositiveVertices);
        }
    }
    freeVerticesGroupModularitySubMatrix(group);
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
