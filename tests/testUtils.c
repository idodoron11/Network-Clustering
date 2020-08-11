#include <stdio.h>
#include <stdlib.h>
#include "testUtils.h"
#include "../spmat.h"
#include "../ErrorHandler.h"

/**
 * Print matrix (python style)
 * @param mat
 */
void printMatrix(Matrix *mat) {
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
            val = readVal(mat, i, j);
            printf("%s", delimiter2);
            delimiter2 = ",";
            printf("%.4f", val);
        }
        printf("]");
    }
    printf("]\n");
}

/**
 * Print matrix Wolfram-style
 * @param mat
 */
void printMatrixPy(Matrix *mat) {
    int i, j;
    double val;
    printf("[");
    for (i = 0; i < mat->n; i++) {
        if (i > 0) {
            printf(",");
        }
        printf("[");
        for (j = 0; j < mat->n; j++) {
            val = readVal(mat, i, j);
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
}

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
 * @param n matrix of size nxn
 * @param percent probability of non-zero values
 * @param mat regular matrix representation, should be allocated
 */
spmat *generateRandomSymSpmat(int n, double percent, Matrix *mat) {
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
                setVal(mat, j, i, readVal(mat, i, j));
            } else if (i == j) {
                setVal(mat, i, j, 0);
            }
        }
        spm->add_row(spm, mat->values[i], i);
    }

    return spm;
}

/**
 * Print sparse matrix
 * @param spm
 */
void printSpmat(spmat *spm) {
    int i, j, col;
    nodeRef *rows = (nodeRef *) spm->private;
    for (i = 0; i < spm->n; i++) {
        nodeRef node = rows[i];
        for (j = 0; j < spm->n; j++) {
            col = node != NULL ? node->colind : spm->n;
            if (j < col) {
                printf("%d ", 0);
            } else {
                printf("%d ", (int) node->value);
                node = node->next;
            }
        }
        printf("\n");
    }
}

/**
 * Prints the vertex-coloring a given group-list defines over
 * a n-vertices graph.
 * @param groupList a partition of graph vertices into groups.
 * @param n the number of vertices in the graph.
 */
void printGroupList(LinkedList *groupList, int n){
    int L = groupList->length;
    int *coloring = malloc(sizeof(int) * n);
    int i, j;
    LinkedListNode *node = groupList->first;
    VerticesGroup *group;
    VertexNode *vertex;
    assertMemoryAllocation(coloring);

    for(i = 0; i < L; ++i){
        group = node->pointer;
        vertex = group->first;
        for(j = 0; j < group->size; ++j){
            coloring[vertex->index] = i;
            vertex = vertex->next;
        }
        node = node->next;
    }

    printf("[");
    if(n>0)
        printf("%d", coloring[0]);
    for(i = 1; i < n; ++i)
        printf(",%d", coloring[i]);
    printf("]\n");

    free(coloring);
}