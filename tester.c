#include <stdlib.h>
#include <assert.h>
#include "tester.h"
#include <time.h>
#include <stdio.h>

/**
 * This function receives a partition of {0,1,...,n-1} into disjoint groups G1,G2,...,GK, and
 * generates a new graph G over {0,1,...,n-1}, such that every subgraph G1 is an independent
 * connected component of a clique.
 * You may add some noise to add/remove edges randomly. For instance, if you add 100%
 * noise, every pair of vertices that were connected will get disconnected, and vise versa.
 * It is not recommended to use noise greater than 10%.
 * @param GroupList a partition of {0,1,...,n-1} into disjoint groups.
 * @param n the number of vertices.
 * @param noise a number between 0 to 100 (inclusive).
 * @return a graph consists of the communities given as an input.
 */
graph *generateCommunitiesGraph(LinkedList *GroupList, int n, char noise){
    graph* G;
    double* A = (double*) calloc(n*n, sizeof(double));
    int numberOfClusters = GroupList->length;
    LinkedListNode *node = GroupList->first;
    VerticesGroup *currentGroup;
    VertexNode *vertexU, *vertexV;
    int i, j, k;
    double rnd;
    assert(A != NULL);
    srand(time(NULL));

    for(i = 0; i < numberOfClusters; ++i){
        currentGroup = node->pointer;
        assert(currentGroup != NULL);
        assert(currentGroup->size > 0);
        vertexU = currentGroup->first;
        vertexV = vertexU->next;

        for(j = 0; j < currentGroup->size; ++j){
            for(k = j+1; k < currentGroup->size; ++k){
                /* connects every pair of vertices in a given VerticesGroup */
                A[n*vertexU->index + vertexV->index] = 1;
                A[n*vertexV->index + vertexU->index] = 1;
                vertexV = vertexV->next;
            }
            vertexU = vertexU->next;
            vertexV = vertexU->next;
        }
        node = node->next;
    }

    /* connects or splits vertices in probability 15% */
    if(0 < noise && noise <= 100) {
        for (i = 0; i < n; ++i) {
            for (j = 0; j < i; ++j) {
                rnd = drand(0, 100);
                if (rnd <= noise) {
                    A[i * n + j] = 1 - A[i * n + j];
                    A[j * n + i] = A[i * n + j];
                }
            }
        }
    }

    /* Adjacency matrix is ready. Now we create a graph out of it */
    G = constructGraphFromMatrix(A, n);
    free(A);
    return G;
}

/**
 * This function gets two partitions of G into sub-graphs, and returns
 * 1 iff they are equivalent. Notice that order does not affect the
 * result. Thus, even if the groups are not in the same order, as long
 * as they match, the function returns 1.
 * @param GroupList1 The first partition of G.
 * @param GroupList2 The second partition of G.
 * @param n The number of vertices G consists of.
 * @return 1-if the partitions are equivalent. 0-otherwise.
 */
char checkGroupListsEquality(LinkedList *GroupList1, LinkedList *GroupList2, int n){
    int i = 0, j = 0;
    LinkedListNode *node1 = GroupList1->first, *node2 = GroupList2->first;
    VerticesGroup *G1, *G2;
    VertexNode *vertex1, *vertex2;
    int *coloring1, *coloring2, *colorMapping;
    int numberOfGroups = GroupList1->length;
    if(numberOfGroups != GroupList2->length) {
        printf("The first partition consists of %d groups, while the second consists of %d groups.\n", GroupList1->length, GroupList2->length);
        return 0;
    }
    coloring1 = (int*) malloc(n * sizeof(int));
    coloring2 = (int*) malloc(n * sizeof(int));
    colorMapping = (int*) malloc(numberOfGroups * sizeof(int));
    assert(coloring1 != NULL);
    assert(coloring2 != NULL);
    assert(colorMapping != NULL);
    for(i=0; i < numberOfGroups; ++i)
        colorMapping[i] = -1;

    for(i = 0; i < numberOfGroups; ++i){
        G1 = node1->pointer;
        G2 = node2->pointer;
        vertex1 = G1->first;
        vertex2 = G2->first;
        for(j = 0; j < G1->size; ++j){
            coloring1[vertex1->index] = i;
            vertex1 = vertex1->next;
        }
        for(j = 0; j < G2->size; ++j){
            coloring2[vertex2->index] = i;
            vertex2 = vertex2->next;
        }
        node1 = node1->next;
        node2 = node2->next;
    }
    for(j = 0; j < n; ++j){
        if(colorMapping[coloring1[j]] == -1)
            colorMapping[coloring1[j]] = coloring2[j];
        else if(colorMapping[coloring1[j]] == coloring2[j])
            continue;
        else {
            printf("Coloring1: [");
            if(n>0)
                printf("%d", coloring1[0]);
            for(i = 1; i < n; ++i)
                printf(",%d", coloring1[i]);
            printf("]\n");
            printf("Coloring2: [");
            if(n>0)
                printf("%d", coloring2[0]);
            for(i = 1; i < n; ++i)
                printf(",%d", coloring2[i]);
            printf("]\n");
            free(coloring1);
            free(coloring2);
            free(colorMapping);
            return 0;
        }
    }
    free(coloring1);
    free(coloring2);
    free(colorMapping);
    return 1;
}

int main() {
    LinkedList *GroupList = createLinkedList();
    LinkedList *DivisionResult;
    int n = 20;
    VerticesGroup *c1 = createVerticesGroup();
    VerticesGroup *c2 = createVerticesGroup();
    VerticesGroup *c3 = createVerticesGroup();
    int C1[5] = {0,1,2,3,4};
    int C2[10] = {5,6,7,8,9,10,11,12,13,14};
    int C3[5] = {19,18,17,16,15};
    graph *G;

    addSequence(c1, C1, 5);
    addSequence(c2, C2, 10);
    addSequence(c3, C3, 5);

    insertItem(GroupList, c1, 0);
    insertItem(GroupList, c2, 0);
    insertItem(GroupList, c3, 0);

    G = generateCommunitiesGraph(GroupList, n, 15);
    DivisionResult = divisionAlgorithm(G);
    printf("Valid?\t%d", checkGroupListsEquality(GroupList, DivisionResult, n));


    freeVerticesGroup(c1);
    freeVerticesGroup(c2);
    freeVerticesGroup(c3);
    freeLinkedList(GroupList);

    return 0;
}
