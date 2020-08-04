#include <stdlib.h>
#include <assert.h>
#include "tester.h"
#include <time.h>
#include <stdio.h>

graph *generateCommunitiesGraph(LinkedList *VerticesGroups, int n){
    graph* G = (graph*) malloc(sizeof(graph));
    double* A = (double*) calloc(n*n, sizeof(double));
    int numberOfClusters = VerticesGroups->length;
    LinkedListNode *node = VerticesGroups->first;
    VerticesGroup *currentGroup;
    VertexNode *vertexU, *vertexV;
    int i, j, k;
    double rnd;
    assert(G != NULL);
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
    for(i = 0; i < n; ++i){
        for(j = 0; j < i; ++j){
            rnd = drand(0,100);
            if(rnd <= 7) {
                A[i * n + j] = 1 - A[i * n + j];
                A[j * n + i] = A[i * n + j];
            }
        }
    }

    /* Adjacency matrix is ready. Now we create a graph out of it */
    return constructGraphFromMatrix(A, n);
}

int main() {
    LinkedList *GroupList = createLinkedList();
    VerticesGroup *c1 = createVerticesGroup();
    VerticesGroup *c2 = createVerticesGroup();
    VerticesGroup *c3 = createVerticesGroup();
    int C1[5] = {0,1,2,3,4};
    int C2[10] = {5,6,7,8,9,10,11,12,13,14};
    int C3[5] = {15,16,17,18,19};
    graph *G;

    addSequence(c1, C1, 5);
    addSequence(c2, C2, 10);
    addSequence(c3, C3, 5);

    insertItem(GroupList, c1, 0);
    insertItem(GroupList, c2, 0);
    insertItem(GroupList, c3, 0);

    printf("%d, %d, %d\n", c1->size, c2->size, c3->size);


    G = generateCommunitiesGraph(GroupList, 20);
    freeVerticesGroup(c1);
    freeVerticesGroup(c2);
    freeVerticesGroup(c3);
    freeLinkedList(GroupList);

    printMatrix(G->adjMat);

    return 0;
}
