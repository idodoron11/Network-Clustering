#include <stdlib.h>
#include "tester.h"
#include "../ErrorHandler.h"
#include "testUtils.h"
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

/*#define GRAPHS_DIR "tests\\graphs"*/
#define GRAPHS_DIR "..\\tests\\graphs"

void printColorings(int *coloring1, int *coloring2, int n);

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
Graph *generateCommunitiesGraph(LinkedList *GroupList, int n, char noise) {
    Graph *G;
    double *A = (double *) calloc(n * n, sizeof(double));
    int numberOfClusters = GroupList->length;
    LinkedListNode *node = GroupList->first;
    VerticesGroup *currentGroup;
    int vertexUIndex, vertexVIndex;
    int i, j, k;
    double rnd;
    assertMemoryAllocation(A);
    srand(time(NULL));

    for (i = 0; i < numberOfClusters; ++i) {
        currentGroup = node->pointer;
        assertBooleanStatementIsTrue(currentGroup != NULL);
        assertBooleanStatementIsTrue(currentGroup->size > 0);

        for (j = 0; j < currentGroup->size; ++j) {
            for (k = j + 1; k < currentGroup->size; ++k) {
                /* connects every pair of vertices in a given VerticesGroup */
                vertexVIndex = currentGroup->verticesArr[j];
                vertexUIndex = currentGroup->verticesArr[k];
                A[n * vertexUIndex + vertexVIndex] = 1;
                A[n * vertexVIndex + vertexUIndex] = 1;
            }
        }
        node = node->next;
    }

    /* connects or splits vertices in probability 15% */
    if (0 < noise && noise <= 100) {
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
char checkGroupListsEquality(LinkedList *GroupList1, testGraph *TG) {
    int i = 0, j = 0, n = TG->G->n;
    double modularity;
    LinkedList *GroupList2 = TG->GroupList;
    LinkedListNode *node1 = GroupList1->first, *node2 = GroupList2->first;
    VerticesGroup *G1, *G2;
    int vertex1Index, vertex2Index;
    int *coloring1, *coloring2, *colorMapping;
    int n1 = 0, n2 = 0;
    int numberOfGroups = GroupList1->length;
    modularity = calculateDivisionModularity(TG->G, GroupList1);
    printf("Found modularity: %f\n", modularity);
    modularity = calculateDivisionModularity(TG->G, GroupList2);
    printf("Expected modularity: %f\n", modularity);
    if (numberOfGroups != GroupList2->length) {
        printf("The first partition consists of %d groups, while the second consists of %d groups.\n",
               GroupList1->length, GroupList2->length);
        printf("Coloring1: ");
        printGroupList(GroupList1, n);
        printf("Coloring2: ");
        printGroupList(GroupList2, n);

        return 0;
    }
    coloring1 = (int *) malloc(n * sizeof(int));
    coloring2 = (int *) malloc(n * sizeof(int));
    colorMapping = (int *) malloc(numberOfGroups * sizeof(int));
    assertMemoryAllocation(coloring1);
    assertMemoryAllocation(coloring2);
    assertMemoryAllocation(colorMapping);
    for (i = 0; i < numberOfGroups; ++i)
        colorMapping[i] = -1;

    for (i = 0; i < numberOfGroups; ++i) {
        G1 = node1->pointer;
        G2 = node2->pointer;
        n1 += G1->size;
        n2 += G2->size;
        for (j = 0; j < G1->size; ++j) {
            vertex1Index = G1->verticesArr[j];
            coloring1[vertex1Index] = i;
        }
        for (j = 0; j < G2->size; ++j) {
            vertex2Index = G2->verticesArr[j];
            coloring2[vertex2Index] = i;
        }
        node1 = node1->next;
        node2 = node2->next;
    }
    if (n1 != n2) {
        printf("The two partitions have different total number of vertices.\n"
               "The first consists of %d vertices, while the second consists of %d.\n", n1, n2);
        printColorings(coloring1, coloring2, n);
        free(colorMapping);
        free(coloring2);
        free(coloring1);
        return 0;
    }
    for (j = 0; j < n; ++j) {
        if (colorMapping[coloring1[j]] == -1)
            colorMapping[coloring1[j]] = coloring2[j];
        else if (colorMapping[coloring1[j]] == coloring2[j])
            continue;
        else {
            printColorings(coloring1, coloring2, n);
            free(coloring1);
            free(coloring2);
            free(colorMapping);
            return 0;
        }
    }
    printColorings(coloring1, coloring2, n);
    free(coloring1);
    free(coloring2);
    free(colorMapping);
    return 1;
}

void printColorings(int *coloring1, int *coloring2, int n) {
    int i;
    printf("Coloring1: [");
    if (n > 0)
        printf("%d", coloring1[0]);
    for (i = 1; i < n; ++i)
        printf(",%d", coloring1[i]);
    printf("]\n");
    printf("Coloring2: [");
    if (n > 0)
        printf("%d", coloring2[0]);
    for (i = 1; i < n; ++i)
        printf(",%d", coloring2[i]);
    printf("]\n");
}

/**
 * Creates a new graph object based on a list of edges.
 * @param edges an integer 2d-array of capacity length X 2. Each row represent a single edge.
 * @param length the number of edges
 * @param n the number of vertices in the graph itself.
 * @return the desired graph.
 */
Graph *createGraphFromEdgesGroup(int edges[][2], int length, int n) {
    double *adjMat = calloc(n * n, sizeof(double));
    int e, i, j;
    Graph *G;
    assertMemoryAllocation(adjMat);
    for (e = 0; e < length; ++e) {
        i = edges[e][0];
        j = edges[e][1];
        adjMat[i * n + j] = 1;
        adjMat[j * n + i] = 1;
    }
    G = constructGraphFromMatrix(adjMat, n);
    free(adjMat);
    return G;
}

/**
 * Constructor of testGraph objects, that creates the graph by getting a list
 * of edges as an input.
 * @param edges an integer 2d-array of capacity length X 2. Each row represent a single edge.
 * @param length the number of edges
 * @param n the number of vertices in the graph itself.
 * @param GroupList the list of groups the graph should be partitioned into by the division algorithm.
 * @return a new testGraph object ready to be tested.
 */
testGraph *createTestGraph(int edges[][2], int length, int n, LinkedList *GroupList) {
    testGraph *TG = malloc(sizeof(testGraph));
    assertMemoryAllocation(TG);
    TG->G = createGraphFromEdgesGroup(edges, length, n);
    TG->GroupList = GroupList;
    return TG;
}

/**
 * frees up memory allocated by a testGraph object.
 * @param TG a testGraph object to destroy.
 */
void destroyTestGraph(testGraph *TG) {
    int i = 0;
    LinkedListNode *groupNode = TG->GroupList->first;
    VerticesGroup *group;
    for (i = 0; i < TG->GroupList->length; ++i) {
        group = groupNode->pointer;
        freeVerticesGroup(group);
        groupNode = groupNode->next;
    }
    freeLinkedList(TG->GroupList);
    destroyGraph(TG->G);
    free(TG);
}

/**
 * This function takes a valid testGraph object and tests it.
 * @param TG the testGraph object.
 * @return 0-if the test fails. 1-otherwise.
 */
char performTest(testGraph *TG) {
    LinkedList *result = divisionAlgorithm(TG->G);
    return checkGroupListsEquality(result, TG);
}

/**
 * Creates a random graph and tests it.
 * @return 0-if the test fails. 1-otherwise.
 */
char testRandomGraph() {
    LinkedList *GroupList;
    VerticesGroup *c[20];
    testGraph *TG = malloc(sizeof(testGraph));
    int n = 20;
    int C1[5] = {0, 1, 2, 3, 4};
    int C2[10] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    int C3[5] = {19, 18, 17, 16, 15};
    Graph *G;
    char result;
    assertMemoryAllocation(TG);

    /* random community graph */
    GroupList = createLinkedList();
    c[0] = createVerticesGroup(5);
    c[1] = createVerticesGroup(10);
    c[2] = createVerticesGroup(5);

    addSequence(c[0], C1, 5);
    addSequence(c[1], C2, 10);
    addSequence(c[2], C3, 5);

    insertItem(GroupList, c[0]);
    insertItem(GroupList, c[1]);
    insertItem(GroupList, c[2]);

    G = generateCommunitiesGraph(GroupList, n, 15);
    TG->GroupList = GroupList;
    TG->G = G;
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

/**
 * This function creates a new testGraph object from an input text file.
 * It makes it easier to "feed" the tester with new inputs.
 * Read 'tests/readme.md' file if you want to create or modify test input files.
 * @param path the location of the input text file.
 * @return a new testGraph object.
 */
testGraph *createTestGraphFromFile(char *path) {
    FILE *file = fopen(path, "r");
    int n, value, numberOfClusters, i, j;
    double *adjMatrix;
    char delimiter;
    VerticesGroup *group;
    LinkedList *groupList = createLinkedList();
    testGraph *TG = malloc(sizeof(testGraph));
    Graph *G = malloc(sizeof(Graph));
    assertFileOpen(file, path);
    assertMemoryAllocation(TG);
    assertMemoryAllocation(G);
    assertFileOpen(file, path);

    /* first value determines n */
    assertFileRead(fscanf(file, "%d%c", &n, &delimiter), 2, path);
    assertBooleanStatementIsTrue(delimiter == '\n');
    adjMatrix = malloc(sizeof(double) * n * n);
    assertMemoryAllocation(adjMatrix);

    /* read matrix entry by entry */
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            assertFileRead(fscanf(file, "%d%c", &value, &delimiter), 2, path);
            adjMatrix[i * n + j] = value;
        }
    }
    assertBooleanStatementIsTrue(delimiter == '\n');

    /* read number of clusters */
    assertFileRead(fscanf(file, "%d%c", &numberOfClusters, &delimiter), 2, path);
    assertBooleanStatementIsTrue(delimiter == '\n');

    /* read groups one by one */
    for (i = 0; i < numberOfClusters - 1; ++i) {
        group = createVerticesGroup(n);
        do {
            assertFileRead(fscanf(file, "%d%c", &value, &delimiter), 2, path);
            addVertexToGroup(group, value);
        } while (delimiter == ' ');
        assertBooleanStatementIsTrue(delimiter == '\n');
        insertItem(groupList, group);
    }
    /* the last row might not end with ' ' or '\n' so we need to handle this differently */
    if (i == numberOfClusters - 1) {
        group = createVerticesGroup(n);
        while (fscanf(file, "%d", &value) == 1)
            addVertexToGroup(group, value);
        insertItem(groupList, group);
    }

    fclose(file);
    assertBooleanStatementIsTrue(groupList->length == numberOfClusters);
    G = constructGraphFromMatrix(adjMatrix, n);
    TG->GroupList = groupList;
    TG->G = G;
    free(adjMatrix);
    return TG;
}

/**
 * This function takes a test input file, converts it into a graph,
 * runs division algorithm on it and checks whether or not the output
 * matches the expected result.
 * Read 'tests/readme.md' file if you want to create or modify test input files.
 * @param path the location of the input text file.
 * @return 0-if the test fails. 1-if the test succeeds.
 */
char testGraphFromFile(char *path) {
    testGraph *TG = createTestGraphFromFile(path);
    char result;
    assertMemoryAllocation(TG);
    result = performTest(TG);
    destroyTestGraph(TG);
    return result;
}

/**
 * This function is useful if you want to print an output file,
 * to manually make sure it is valid or for any other purpose.
 * @param output_file_path the location of the output file.
 */
void printResultsFromOutputFile(char *output_file_path) {
    FILE *output_file = fopen(output_file_path, "rb");
    int numberOfClusters, currentGroup, **groups;
    unsigned int currentGroupSize, i;
    assertFileOpen(output_file, output_file_path);

    assertFileRead(fread(&numberOfClusters, sizeof(int), 1, output_file), 1, output_file_path);
    printf("There are %d clusters.\n", numberOfClusters);
    groups = malloc(numberOfClusters * sizeof(int *));
    assertMemoryAllocation(groups);
    for (currentGroup = 0; currentGroup < numberOfClusters; ++currentGroup) {
        assertFileRead(fread(&currentGroupSize, sizeof(int), 1, output_file), 1, output_file_path);
        groups[currentGroup] = malloc(currentGroupSize * sizeof(int));
        assertMemoryAllocation(groups[currentGroup]);
        assertFileRead(fread(groups[currentGroup], sizeof(int), currentGroupSize, output_file), currentGroupSize,
                       output_file_path);
        printf("Group %d is: ", currentGroup);
        for (i = 0; i < currentGroupSize; ++i)
            printf("%d, ", groups[currentGroup][i]);
        printf("\n");
        free(groups[currentGroup]);
    }
    free(groups);
    fclose(output_file);
}

/*void testMatrixMult() {
    Graph *G;
    VerticesGroup *group;
    int n = 200, i;
    Matrix *mat = createMatrix(n);
    double *s, *res, numRes1, numRes2;
    s = malloc(n * sizeof(double));
    res = malloc(n * sizeof(double));
    generateRandomSymSpmat(n, 20, mat);
    G = constructGraphFromAdjMat(mat);
    group = createVerticesGroup();
    for (i = 0; i < n; i++) {
        addVertexToGroup(group, i);
        s[i] = (drand(0, 100) > 50) ? 1 : -1;
    }
    calculateModularitySubMatrix(G, group);
    numRes1 = multiplyModularityByVector(G, group, s, res, 1, 0);
    numRes2 = multiplyModularityByVectorNormal(group, s, res, 1);
    destroyGraph(G);
    freeVerticesGroup(group);
    assert(fabs(numRes1 - numRes2) < 0.01);
}*/

/*void testModularityChange() {
    Graph *G;
    VerticesGroup *group, *groupA, *groupB;
    int n = 200, i;
    Matrix *mat = createMatrix(n);
    double *s, aModularity, bModularity, modularity;
    s = malloc(n * sizeof(double));
    generateRandomSymSpmat(n, 20, mat);
    G = constructGraphFromAdjMat(mat);
    group = createVerticesGroup(n);
    groupA = createVerticesGroup(n);
    groupB = createVerticesGroup(n);
    for (i = 0; i < n; i++) {
        addVertexToGroup(group, i);
        if (drand(0, 100) < 20) {
            addVertexToGroup(groupA, i);
            s[i] = -1;
        } else {
            addVertexToGroup(groupB, i);
            s[i] = 1;
        }
    }
    calculateModularitySubMatrix(G, group);
    modularity = calculateModularity(G, group, s);
    aModularity = calculateModularityOfGroup(G, groupA);
    bModularity = calculateModularityOfGroup(G, groupB);
    assert(fabs(modularity - (aModularity + bModularity)) < 0.001);
}*/

int main() {
    srand(time(0));
    /*for (i = 0; i < 10; i++) {
        testMatrixMult();
    }
    tstModularityChange(); */

    printf("Testing graph 1 from file.\n");
    printf("Result: %d\n", testGraphFromFile(GRAPHS_DIR"\\graph1-adjMat.txt"));
    /*printResultsFromOutputFile("out");*/
    printf("Testing graph 2 from file.\n");
    printf("Result: %d\n", testGraphFromFile(GRAPHS_DIR"\\graph2-adjMat.txt"));
    /*printf("Testing graph 3 from file.\n");
    printf("Result: %d\n", testGraphFromFile(GRAPHS_DIR"\\graph3-adjMat.txt"));*/
    printf("Testing graph 4 from file.\n");
    printf("Result: %d\n", testGraphFromFile(GRAPHS_DIR"\\graph4-adjMat.txt"));
    printf("Testing graph 5 from file.\n");
    printf("Result: %d\n", testGraphFromFile(GRAPHS_DIR"\\graph5-adjMat.txt"));
    printf("Testing graph 6 from file.\n");
    printf("Result: %d\n", testGraphFromFile(GRAPHS_DIR"\\graph6-adjMat.txt"));
    printf("Testing graph 7 from file.\n");
    printf("Result: %d\n", testGraphFromFile(GRAPHS_DIR"\\graph7-adjMat.txt"));
    printf("Testing graph 8 from file.\n");
    printf("Result: %d\n", testGraphFromFile(GRAPHS_DIR"\\graph8-adjMat.txt"));
    printf("Testing graph 9 from file.\n");
    printf("Result: %d\n", testGraphFromFile(GRAPHS_DIR"\\graph9-adjMat.txt"));
    printf("Testing graph 10 from file.\n");
    printf("Result: %d\n", testGraphFromFile(GRAPHS_DIR"\\graph10-adjMat.txt"));
    printf("Testing random graph.\n");
    printf("Result: %d\n", testRandomGraph());
    return 0;
}
