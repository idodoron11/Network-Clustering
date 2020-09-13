#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "testUtils.h"
#include "../spmat.h"
#include "../ErrorHandler.h"

/**
 * Print matrix (python style)
 * @param mat
 */
/*void printMatrix(Matrix *mat) {
    int i, j;
    double val;
    char *delimiter1 = "[";
    char *delimiter2 = "";
    printf("[");
    for (i = 0; i < mat->n; i++) {
        printf("%s", delimiter1);
        delimiter1 = ",\n[";
        delimiter2 = "";
        for (j = 0; j < mat->n; j++) {
            val = readMatVal(mat, i, j);
            printf("%s", delimiter2);
            delimiter2 = ",";
            printf("%.4f", val);
        }
        printf("]");
    }
    printf("]\n");
}*/

/**
 * Print matrix Wolfram-style
 * @param mat
 */
/*void printMatrixPy(Matrix *mat) {
    int i, j;
    double val;
    printf("[");
    for (i = 0; i < mat->n; i++) {
        if (i > 0) {
            printf(",");
        }
        printf("[");
        for (j = 0; j < mat->n; j++) {
            val = readMatVal(mat, i, j);
            if (j > 0) {
                printf(",");
            }
            if (j == mat->n / 2) {
                printf("\n");
            }
            printf("%f", val);
        }
        printf("]\n");
    }
    printf("]\n");
}*/

/**
 * Print vector
 * @param vector
 * @param length
 */
void printVect(double *vector, int length) {
    int i;
    printf("( ");
    for (i = 0; i < length; i++) {
        printf("%f ", vector[i]);
    }
    printf(" )\n");
}

/**
 * Generate a random symmetric sparse matrix
 * @param n matrix of capacity nxn
 * @param percent probability of non-zero values
 * @param mat regular matrix representation, should be allocated
 */
/* spmat *generateRandomSymSpmat(int n, double percent, Matrix *mat) {
    int i, j;
    double randNum;
    spmat *spm = spmat_allocate_list(n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i < j) {
                randNum = drand(0, 100);
                if (randNum <= percent) {
                    setVal(mat, i, j, 1);
                } else {
                    setVal(mat, i, j, 0);
                }
                setVal(mat, j, i, readMatVal(mat, i, j));
            } else if (i == j) {
                setVal(mat, i, j, 0);
            }
        }
        spm->add_row(spm, mat->values[i], i);
    }

    return spm;
} */

/**
 * Print sparse matrix
 * @param spm
 */
void printSpmat(spmat *spm) {
    int i, j, col;
    nodeRef *rows = (nodeRef *) spm->private;
    printf("\n[");
    for (i = 0; i < spm->n; i++) {
        nodeRef node = rows[i];
        if (i > 0) {
            printf(",");
        }
        printf("[");
        for (j = 0; j < spm->n; j++) {
            col = node != NULL ? node->colind : spm->n;
            if (j > 0) {
                printf(",");
            }
            if (j < col) {
                printf("%d", 0);
            } else {
                printf("%f", node->value);
                node = node->next;
            }
        }
        printf("]\n");
    }
    printf("]\n");
}

/**
 * Prints the vertex-coloring a given group-list defines over
 * a n-vertices graph.
 * @param groupList a partition of graph vertices into groups.
 * @param n the number of vertices in the graph.
 */
void printGroupList(LinkedList *groupList, int n) {
    int L = groupList->length;
    int *coloring = malloc(sizeof(int) * n);
    int i, j;
    int vertexIndex;
    LinkedListNode *node = groupList->first;
    VerticesGroup *group;
    assertMemoryAllocation(coloring);

    for (i = 0; i < L; ++i) {
        group = node->pointer;
        for (j = 0; j < group->size; ++j) {
            vertexIndex = group->verticesArr[j];
            coloring[vertexIndex] = i;
        }
        node = node->next;
    }

    printf("[");
    if (n > 0)
        printf("%d", coloring[0]);
    for (i = 1; i < n; ++i)
        printf(",%d", coloring[i]);
    printf("]\n");

    free(coloring);
}

void printGroupList2(LinkedList *groupList) {
    int i, j;
    LinkedListNode *node = groupList->first;
    VerticesGroup *group;
    for (i = 0; i < groupList->length; i++) {
        group = node->pointer;
        printf("Group %d: ", i + 1);
        for (j = 0; j < group->size; j++) {
            if (j > 0) {
                printf(", ");
            }
            printf("%d", group->verticesArr[j]);
        }
        printf("\n");
        node = node->next;
    }

}


/**
 * Calculate modularity of a graph's division
 * @param groupLst
 * @return modularity
 */
double calculateDivisionModularity(Graph *G, LinkedList *groupLst) {
    VerticesGroup *group;
    double modularity = 0;
    LinkedListNode *node = groupLst->first;
    if (node != NULL) {
        do {
            group = node->pointer;
            calculateModularitySubMatrix(G, group);
            modularity += calculateModularityOfGroup(G, group);
            node = node->next;
        } while (node != groupLst->first);
    }
    return modularity;
}

double calculateModularityOfGroup(Graph *G, VerticesGroup *group) {
    int i, j;
    double modularity = 0;
    calculateModularitySubMatrix(G, group);
    for (i = 0; i < group->size; i++) {
        for (j = 0; j < group->size; j++) {
            modularity += readSpmVal(G->adjMat, group->verticesArr[i], group->verticesArr[j]) -
                          getExpectedEdges(G, group->verticesArr[i], group->verticesArr[j]);
        }
    }
    return modularity;
}

LinkedList *createGroupsFromIndices(Graph *G, int *groups) {
    LinkedList *lst = createLinkedList();
    VerticesGroup *group;
    int i, gIndex = 0, found;
    do {
        found = 0;
        group = createVerticesGroup(G->n);
        for (i = 0; i < G->n; i++) {
            if (groups[i] == gIndex) {
                addVertexToGroup(group, i);
                found = 1;
            }
        }
        if (found) {
            insertItem(lst, group);
        }
        gIndex++;
    } while (found);
    return lst;
}

void compareExpected(char *inputPath, LinkedList *lst, int *expected) {
    Graph *G;
    LinkedList *expectedLst;
    G = constructGraphFromInput(inputPath);
    expectedLst = createGroupsFromIndices(G, expected);
    printf("%s:\n", inputPath);
    printf("Found modularity: %f", calculateDivisionModularity(G, lst));
    printf("\nExpected modularity: %f\n\n", calculateDivisionModularity(G, expectedLst));
}

Graph *constructGraphFromMatrix(double *adjMatrix, int n) {
    Graph *G = (Graph *) malloc(sizeof(Graph));
    int i, j;
    assertMemoryAllocation(G);
    G->degrees = malloc(n * sizeof(int));
    assertMemoryAllocation(G->degrees);
    G->n = n;
    G->degreeSum = 0;
    G->adjMat = spmat_allocate_list(n);

    for (i = 0; i < n; ++i) {
        G->degrees[i] = 0;
        for (j = 0; j < n; ++j)
            G->degrees[i] += adjMatrix[i * n + j];
        G->adjMat->add_row(G->adjMat, adjMatrix + i * n, i);
        G->degreeSum += G->degrees[i];
    }

    return G;
}

/* Graph *constructGraphFromAdjMat(Matrix *mat) {
    Graph *G = (Graph *) malloc(sizeof(Graph));
    int i, j;
    G->degrees = malloc(mat->n * sizeof(int));
    G->n = mat->n;
    G->degreeSum = 0;
    G->adjMat = mat;

    for (i = 0; i < mat->n; ++i) {
        G->degrees[i] = 0;
        for (j = 0; j < mat->n; ++j) {
            G->degrees[i] += readSpmVal(G->adjMat, i, j);
        }
        G->degreeSum += G->degrees[i];
    }

    return G;
} */

/**
 * Adds a sequence of indices to the group.
 * @param group the group to which nodes are added.
 * @param sequence a sequence of group->capacity distinct integers, representing vertices.
 */
void addSequence(VerticesGroup *group, int *sequence, int length) {
    int i;
    assertBooleanStatementIsTrue(length <= group->capacity);
    for (i = 0; i < length; ++i) {
        group->verticesArr[i] = sequence[i];
        ++group->size;
    }
}

/**
 * Generate a random double
 * @param low inclusive
 * @param high inclusive
 * @return random number
 */
double drand(double low, double high) {
    return ((double) rand() * (high - low)) / (double) RAND_MAX + low;
}

double readSpmVal(spmat *spm, int r, int c) {
    nodeRef *rows = spm->private;
    nodeRef node = rows[r];
    while (node != NULL && node->colind <= c) {
        if (node->colind == c) {
            return 1;
        }
        node = node->next;
    }
    return 0;
}
