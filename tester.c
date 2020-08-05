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

graph *createGraphFromEdgesGroup(int edges[][2], int length, int n){
    double *adjMat = calloc(n*n, sizeof(double));
    int e, i, j;
    graph *G;
    assert(adjMat != NULL);
    for(e = 0; e < length; ++e){
        i = edges[e][0];
        j = edges[e][1];
        adjMat[i*n + j] = 1;
        adjMat[j*n + i] = 1;
    }
    G = constructGraphFromMatrix(adjMat, n);
    free(adjMat);
    return G;
}

testGraph *createTestGraph(int edges[][2], int length, int n, LinkedList *GroupList){
    testGraph *TG = malloc(sizeof(testGraph));
    assert(TG != NULL);
    TG->G = createGraphFromEdgesGroup(edges, length, n);
    TG->GroupList = GroupList;
    return TG;
}

void destroyTestGraph(testGraph *TG){
    int i = 0;
    LinkedListNode *groupNode = TG->GroupList->first;
    VerticesGroup  *group;
    for(i = 0; i < TG->GroupList->length; ++i){
        group = groupNode->pointer;
        freeVerticesGroup(group);
        groupNode = groupNode->next;
    }
    freeLinkedList(TG->GroupList);
    destroyGraph(TG->G);
    free(TG);
}

char performTest(testGraph *TG){
    LinkedList *result = divisionAlgorithm(TG->G);
    return checkGroupListsEquality(result, TG->GroupList, TG->G->n);
}

char testGraph1(){
    int Edges1[41][2] = {{0,2},{0,4},{1,2},{1,3},{1,4},{2,4},{2,5},{2,6},{3,4},{3,5},{3,6},{3,11},{4,5},{4,6},{4,7},{5,15},{7,8},{7,10},{7,11},{7,12},{8,10},{8,11},{8,13},{8,16},{9,10},{9,12},{9,13},{10,11},{10,12},{10,14},{12,14},{13,14},{13,18},{15,16},{15,17},{15,19},{16,17},{16,18},{16,19},{17,18},{17,19}};
    int C11[7] = {0,1,2,3,4,5,6};
    int C12[5] = {15,16,17,18,19};
    int C13[8] = {7,8,9,10,11,12,13};
    VerticesGroup *c[3];
    testGraph *TG;
    LinkedList *GroupList = createLinkedList();
    char result;
    c[0] = createVerticesGroup();
    c[1] = createVerticesGroup();
    c[2] = createVerticesGroup();
    addSequence(c[0], C11, 7);
    addSequence(c[1], C12, 5);
    addSequence(c[2], C13, 8);

    insertItem(GroupList, c[0], 0);
    insertItem(GroupList, c[1], 0);
    insertItem(GroupList, c[2], 0);
    TG = createTestGraph(Edges1, 41, 20, GroupList);
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

char testGraph2(){
    int Edges2[61][2] = {{0,1},{0,2},{0,3},{0,4},{1,2},{1,3},{1,4},{2,3},{2,4},{3,4},{5,6},{5,7},{5,8},{5,9},{5,10},{6,7},{6,8},{6,9},{6,10},{7,8},{7,9},{7,10},{8,9},{8,10},{9,10},{11,12},{11,13},{11,14},{11,15},{11,16},{11,17},{11,18},{11,19},{12,13},{12,14},{12,15},{12,16},{12,17},{12,18},{12,19},{13,14},{13,15},{13,16},{13,17},{13,18},{13,19},{14,15},{14,16},{14,17},{14,18},{14,19},{15,16},{15,17},{15,18},{15,19},{16,17},{16,18},{16,19},{17,18},{17,19},{18,19}};
    int C21[5] = {0,1,2,3,4};
    int C22[6] = {5,6,7,8,9,10};
    int C23[9] = {11,12,13,14,15,16,17,18,19};
    VerticesGroup *c[3];
    testGraph *TG;
    LinkedList *GroupList = createLinkedList();
    char result;
    c[0] = createVerticesGroup();
    c[1] = createVerticesGroup();
    c[2] = createVerticesGroup();
    addSequence(c[0], C21, 5);
    addSequence(c[1], C22, 6);
    addSequence(c[2], C23, 9);
    insertItem(GroupList, c[0], 0);
    insertItem(GroupList, c[1], 0);
    insertItem(GroupList, c[2], 0);
    TG = createTestGraph(Edges2, 61, 20, GroupList);
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

char testGraph3(){
    int Edges3[74][2] = {{0,1},{0,2},{0,3},{0,4},{0,10},{1,2},{1,3},{1,4},{2,3},{2,4},{3,4},{4,9},{5,6},{5,7},{5,8},{5,9},{5,10},{6,7},{6,8},{6,9},{6,10},{6,16},{7,8},{7,9},{7,10},{7,18},{8,9},{8,10},{9,10},{11,12},{11,13},{11,14},{11,15},{11,16},{11,17},{11,18},{11,19},{11,20},{12,13},{12,14},{12,15},{12,16},{12,17},{12,18},{12,19},{12,20},{13,14},{13,15},{13,16},{13,17},{13,18},{13,19},{13,20},{14,15},{14,16},{14,17},{14,18},{14,19},{14,20},{15,16},{15,17},{15,18},{15,19},{15,20},{16,17},{16,18},{16,19},{16,20},{17,18},{17,19},{17,20},{18,19},{18,20},{19,20}};
    int C31[5] = {0,1,2,3,4};
    int C32[6] = {5,6,7,8,9,10};
    int C33[9] = {11,12,13,14,15,16,17,18,19};
    VerticesGroup *c[3];
    testGraph *TG;
    LinkedList *GroupList = createLinkedList();
    char result;
    c[0] = createVerticesGroup();
    c[1] = createVerticesGroup();
    c[2] = createVerticesGroup();
    addSequence(c[0], C31, 5);
    addSequence(c[1], C32, 6);
    addSequence(c[2], C33, 9);
    insertItem(GroupList, c[0], 0);
    insertItem(GroupList, c[1], 0);
    insertItem(GroupList, c[2], 0);
    /* there's some bug here. the adjacency matrix is created successfully,
     * but when i convert it to a graph object there's some problem.
     * specifically, it happens during memory allocation for a Matrix object. */
    TG = createTestGraph(Edges3, 74, 20, GroupList);
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

char testGraph4(){
    int Edges4[119][2] = {{0,1},{0,2},{0,3},{0,4},{0,10},{1,2},{1,3},{1,4},{1,16},{2,3},{2,4},{2,18},{3,4},{4,9},{5,6},{5,7},{5,8},{5,9},{5,10},{5,11},{5,12},{5,13},{5,14},{5,15},{5,16},{5,17},{5,18},{5,19},{6,7},{6,8},{6,9},{6,10},{6,11},{6,12},{6,13},{6,14},{6,15},{6,16},{6,17},{6,18},{6,19},{7,8},{7,9},{7,10},{7,11},{7,12},{7,13},{7,14},{7,15},{7,16},{7,17},{7,18},{7,19},{8,9},{8,10},{8,11},{8,12},{8,13},{8,14},{8,15},{8,16},{8,17},{8,18},{8,19},{9,10},{9,11},{9,12},{9,13},{9,14},{9,15},{9,16},{9,17},{9,18},{9,19},{10,11},{10,12},{10,13},{10,14},{10,15},{10,16},{10,17},{10,18},{10,19},{11,12},{11,13},{11,14},{11,15},{11,16},{11,17},{11,18},{11,19},{12,13},{12,14},{12,15},{12,16},{12,17},{12,18},{12,19},{13,14},{13,15},{13,16},{13,17},{13,18},{13,19},{14,15},{14,16},{14,17},{14,18},{14,19},{15,16},{15,17},{15,18},{15,19},{16,17},{16,18},{16,19},{17,18},{17,19},{18,19}};
    int C41[5] = {0,1,2,3,4};
    int C42[15] = {5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    VerticesGroup *c[2];
    testGraph *TG;
    LinkedList *GroupList = createLinkedList();
    char result;
    c[0] = createVerticesGroup();
    c[1] = createVerticesGroup();
    addSequence(c[0], C41, 5);
    addSequence(c[1], C42, 15);
    insertItem(GroupList, c[0], 0);
    insertItem(GroupList, c[1], 0);
    TG = createTestGraph(Edges4, 119, 20, GroupList);
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

char testGraph5(){
    int Edges5[100][2] = {{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,10},{1,2},{1,4},{1,5},{1,6},{1,16},{2,3},{2,4},{2,5},{2,6},{2,18},{3,4},{3,5},{3,6},{4,5},{4,6},{4,9},{5,6},{7,8},{7,9},{7,10},{7,11},{7,12},{7,14},{7,15},{7,16},{7,17},{7,18},{7,19},{8,9},{8,10},{8,11},{8,12},{8,13},{8,14},{8,15},{8,16},{8,17},{8,18},{9,10},{9,11},{9,12},{9,13},{9,14},{9,15},{9,16},{9,17},{9,18},{9,19},{10,11},{10,12},{10,13},{10,14},{10,15},{10,16},{10,17},{10,18},{10,19},{11,12},{11,13},{11,14},{11,15},{11,16},{11,17},{11,18},{11,19},{12,13},{12,14},{12,15},{12,16},{12,17},{12,18},{12,19},{13,14},{13,15},{13,16},{13,17},{13,18},{13,19},{14,15},{14,16},{14,17},{14,18},{14,19},{15,16},{15,17},{15,18},{15,19},{16,17},{16,18},{16,19},{17,18},{17,19},{18,19}};
    int C51[7] = {0,1,2,3,4,5,6};
    int C52[13] = {7,8,9,10,11,12,13,14,15,16,17,18,19};
    VerticesGroup *c[2];
    testGraph *TG;
    LinkedList *GroupList = createLinkedList();
    char result;
    c[0] = createVerticesGroup();
    c[1] = createVerticesGroup();
    addSequence(c[0], C51, 7);
    addSequence(c[1], C52, 13);
    insertItem(GroupList, c[0], 0);
    insertItem(GroupList, c[1], 0);
    TG = createTestGraph(Edges5, 100, 20, GroupList);
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

char testGraph6(){
    double edgeMat6[20*20] = {0};
    VerticesGroup *c[20];
    testGraph *TG;
    int i;
    LinkedList *GroupList = createLinkedList();
    char result;
    for(i = 0; i < 20; ++i){
        c[i] = createVerticesGroup();
        addVertexToGroup(c[i], i);
        insertItem(GroupList, c[i], 0);
    }
    TG = malloc(sizeof(testGraph));
    TG->G = constructGraphFromMatrix(edgeMat6, 20);
    TG->GroupList = GroupList;
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

char testGraph7(){
    int Edges7[190][2] = {{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7},{0,8},{0,9},{0,10},{0,11},{0,12},{0,13},{0,14},{0,15},{0,16},{0,17},{0,18},{0,19},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7},{1,8},{1,9},{1,10},{1,11},{1,12},{1,13},{1,14},{1,15},{1,16},{1,17},{1,18},{1,19},{2,3},{2,4},{2,5},{2,6},{2,7},{2,8},{2,9},{2,10},{2,11},{2,12},{2,13},{2,14},{2,15},{2,16},{2,17},{2,18},{2,19},{3,4},{3,5},{3,6},{3,7},{3,8},{3,9},{3,10},{3,11},{3,12},{3,13},{3,14},{3,15},{3,16},{3,17},{3,18},{3,19},{4,5},{4,6},{4,7},{4,8},{4,9},{4,10},{4,11},{4,12},{4,13},{4,14},{4,15},{4,16},{4,17},{4,18},{4,19},{5,6},{5,7},{5,8},{5,9},{5,10},{5,11},{5,12},{5,13},{5,14},{5,15},{5,16},{5,17},{5,18},{5,19},{6,7},{6,8},{6,9},{6,10},{6,11},{6,12},{6,13},{6,14},{6,15},{6,16},{6,17},{6,18},{6,19},{7,8},{7,9},{7,10},{7,11},{7,12},{7,13},{7,14},{7,15},{7,16},{7,17},{7,18},{7,19},{8,9},{8,10},{8,11},{8,12},{8,13},{8,14},{8,15},{8,16},{8,17},{8,18},{8,19},{9,10},{9,11},{9,12},{9,13},{9,14},{9,15},{9,16},{9,17},{9,18},{9,19},{10,11},{10,12},{10,13},{10,14},{10,15},{10,16},{10,17},{10,18},{10,19},{11,12},{11,13},{11,14},{11,15},{11,16},{11,17},{11,18},{11,19},{12,13},{12,14},{12,15},{12,16},{12,17},{12,18},{12,19},{13,14},{13,15},{13,16},{13,17},{13,18},{13,19},{14,15},{14,16},{14,17},{14,18},{14,19},{15,16},{15,17},{15,18},{15,19},{16,17},{16,18},{16,19},{17,18},{17,19},{18,19}};
    int C71[20] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    VerticesGroup *c[1];
    testGraph *TG;
    LinkedList *GroupList = createLinkedList();
    char result;
    c[0] = createVerticesGroup();
    addSequence(c[0], C71, 20);
    insertItem(GroupList, c[0], 0);
    TG = createTestGraph(Edges7, 190, 20, GroupList);
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

char testGraph8(){
    int Edges8[21][2] = {{0,1},{0,10},{1,2},{2,3},{2,19},{3,4},{4,5},{4,9},{5,6},{6,7},{7,8},{8,9},{10,11},{11,12},{12,13},{13,14},{14,15},{15,16},{16,17},{17,18},{18,19}};
    int C81[13] = {0,1,2,10,11,12,13,14,15,16,17,18,19};
    int C82[7] = {3,4,5,6,7,8,9};
    VerticesGroup *c[2];
    testGraph *TG;
    LinkedList *GroupList = createLinkedList();
    char result;
    c[0] = createVerticesGroup();
    c[1] = createVerticesGroup();
    addSequence(c[0], C81, 13);
    addSequence(c[1], C82, 7);

    insertItem(GroupList, c[0], 0);
    insertItem(GroupList, c[1], 0);
    TG = createTestGraph(Edges8, 21, 20, GroupList);
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

char testGraph9(){
    int Edges9[75][2] = {{0,1},{0,2},{0,3},{0,4},{0,8},{1,2},{1,3},{1,9},{2,3},{2,4},{2,7},{2,12},{3,4},{3,14},{4,16},{5,6},{5,7},{5,8},{5,10},{5,11},{5,12},{5,13},{6,7},{6,8},{6,9},{6,10},{6,11},{6,12},{6,13},{6,14},{7,8},{7,9},{7,10},{7,11},{7,12},{7,13},{7,14},{7,16},{7,18},{8,9},{8,10},{8,11},{8,12},{8,13},{8,14},{9,10},{9,11},{9,12},{9,13},{9,14},{9,17},{10,11},{10,12},{10,13},{10,14},{10,17},{10,19},{11,12},{11,13},{11,14},{12,13},{12,14},{12,16},{13,14},{13,15},{15,16},{15,17},{15,18},{15,19},{16,17},{16,18},{16,19},{17,18},{17,19},{18,19}};
    int C91[5] = {15,16,17,18,19};
    int C92[5] = {0,1,2,3,4};
    int C93[10] = {5,6,7,8,9,10,11,12,13};
    char result;
    VerticesGroup *c[3];
    testGraph *TG;
    LinkedList *GroupList = createLinkedList();
    c[0] = createVerticesGroup();
    c[1] = createVerticesGroup();
    c[2] = createVerticesGroup();
    addSequence(c[0], C91, 5);
    addSequence(c[1], C92, 5);
    addSequence(c[1], C93, 10);

    insertItem(GroupList, c[0], 0);
    insertItem(GroupList, c[1], 0);
    insertItem(GroupList, c[2], 0);
    TG = createTestGraph(Edges9, 75, 20, GroupList);
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

char testRandomGraph(){
    LinkedList *GroupList;
    VerticesGroup * c[20];
    testGraph* TG = malloc(sizeof(testGraph));
    int n = 20;
    int C1[5] = {0,1,2,3,4};
    int C2[10] = {5,6,7,8,9,10,11,12,13,14};
    int C3[5] = {19,18,17,16,15};
    graph *G;
    char result;

    /* random community graph */
    GroupList = createLinkedList();
    c[0] = createVerticesGroup();
    c[1] = createVerticesGroup();
    c[2] = createVerticesGroup();

    addSequence(c[0], C1, 5);
    addSequence(c[1], C2, 10);
    addSequence(c[2], C3, 5);

    insertItem(GroupList, c[0], 0);
    insertItem(GroupList, c[1], 0);
    insertItem(GroupList, c[2], 0);

    G = generateCommunitiesGraph(GroupList, n, 15);
    TG->GroupList = GroupList;
    TG->G = G;
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

int main() {
    printf("Testing graph 1. Result: %d\n", testGraph1());
    printf("Testing graph 2. Result: %d\n", testGraph2());
    /*printf("Testing graph 3. Result: %d\n", testGraph3());*/
    printf("Testing graph 4. Result: %d\n", testGraph4());
    printf("Testing graph 5. Result: %d\n", testGraph5());
    printf("Testing graph 6. Result: %d\n", testGraph6());
    printf("Testing graph 7. Result: %d\n", testGraph7());
    printf("Testing graph 8. Result: %d\n", testGraph8());
    printf("Testing graph 9. Result: %d\n", testGraph9());
    printf("Testing random graph. Result: %d\n", testRandomGraph());
    return 0;
}
