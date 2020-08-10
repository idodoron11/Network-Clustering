#include <stdlib.h>
#include "tester.h"
#include "ErrorHandler.h"
#include "VerticesGroup.h"
#include <time.h>
#include <stdio.h>

void printColorings(int* coloring1, int* coloring2, int n);

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
Graph *generateCommunitiesGraph(LinkedList *GroupList, int n, char noise){
    Graph* G;
    double* A = (double*) calloc(n*n, sizeof(double));
    int numberOfClusters = GroupList->length;
    LinkedListNode *node = GroupList->first;
    VerticesGroup *currentGroup;
    VertexNode *vertexU, *vertexV;
    int i, j, k;
    double rnd;
    assertMemoryAllocation(A);
    srand(time(NULL));

    for(i = 0; i < numberOfClusters; ++i){
        currentGroup = node->pointer;
        assertBooleanStatementIsTrue(currentGroup != NULL);
        assertBooleanStatementIsTrue(currentGroup->size > 0);
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
    int n1 = 0,  n2 = 0;
    int numberOfGroups = GroupList1->length;
    if(numberOfGroups != GroupList2->length) {
        printf("The first partition consists of %d groups, while the second consists of %d groups.\n", GroupList1->length, GroupList2->length);
        printf("Coloring1: ");
        printGroupList(GroupList1, n);
        printf("Coloring2: ");
        printGroupList(GroupList2, n);
        return 0;
    }
    coloring1 = (int*) malloc(n * sizeof(int));
    coloring2 = (int*) malloc(n * sizeof(int));
    colorMapping = (int*) malloc(numberOfGroups * sizeof(int));
    assertMemoryAllocation(coloring1);
    assertMemoryAllocation(coloring2);
    assertMemoryAllocation(colorMapping);
    for(i=0; i < numberOfGroups; ++i)
        colorMapping[i] = -1;

    for(i = 0; i < numberOfGroups; ++i){
        G1 = node1->pointer;
        G2 = node2->pointer;
        vertex1 = G1->first;
        vertex2 = G2->first;
        n1 += G1->size;
        n2 += G2->size;
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
    if(n1 != n2) {
        printf("The two partitions have different total number of vertices.\n"
               "The first consists of %d vertices, while the second consists of %d.\n", n1, n2);
        printColorings(coloring1, coloring2, n);
        free(colorMapping);
        free(coloring2);
        free(coloring1);
        return 0;
    }
    for(j = 0; j < n; ++j){
        if(colorMapping[coloring1[j]] == -1)
            colorMapping[coloring1[j]] = coloring2[j];
        else if(colorMapping[coloring1[j]] == coloring2[j])
            continue;
        else {
            printColorings(coloring1, coloring2, n);
            return 0;
        }
    }
    printColorings(coloring1, coloring2, n);
    free(coloring1);
    free(coloring2);
    free(colorMapping);
    return 1;
}

void printColorings(int* coloring1, int* coloring2, int n){
    int i;
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
}

/**
 * Creates a new graph object based on a list of edges.
 * @param edges an integer 2d-array of size length X 2. Each row represent a single edge.
 * @param length the number of edges
 * @param n the number of vertices in the graph itself.
 * @return the desired graph.
 */
Graph *createGraphFromEdgesGroup(int edges[][2], int length, int n){
    double *adjMat = calloc(n*n, sizeof(double));
    int e, i, j;
    Graph *G;
    assertMemoryAllocation(adjMat);
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

/**
 * Constructor of testGraph objects, that creates the graph by getting a list
 * of edges as an input.
 * @param edges an integer 2d-array of size length X 2. Each row represent a single edge.
 * @param length the number of edges
 * @param n the number of vertices in the graph itself.
 * @param GroupList the list of groups the graph should be partitioned into by the division algorithm.
 * @return a new testGraph object ready to be tested.
 */
testGraph *createTestGraph(int edges[][2], int length, int n, LinkedList *GroupList){
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

/**
 * This function takes a valid testGraph object and tests it.
 * @param TG the testGraph object.
 * @return 0-if the test fails. 1-otherwise.
 */
char performTest(testGraph *TG){
    LinkedList *result = divisionAlgorithm(TG->G);
    return checkGroupListsEquality(result, TG->GroupList, TG->G->n);
}

/**
 * Creates a random graph and tests it.
 * @return 0-if the test fails. 1-otherwise.
 */
char testRandomGraph(){
    LinkedList *GroupList;
    VerticesGroup * c[20];
    testGraph* TG = malloc(sizeof(testGraph));
    int n = 20;
    int C1[5] = {0,1,2,3,4};
    int C2[10] = {5,6,7,8,9,10,11,12,13,14};
    int C3[5] = {19,18,17,16,15};
    Graph *G;
    char result;
    assertMemoryAllocation(TG);

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

/**
 * This function creates a new testGraph object from an input text file.
 * It makes it easier to "feed" the tester with new inputs.
 * Read 'tests/readme.md' file if you want to create or modify test input files.
 * @param path the location of the input text file.
 * @return a new testGraph object.
 */
testGraph *createTestGraphFromFile(char* path){
    FILE* file = fopen(path, "r");
    char c;
    char word[10];
    int n;
    int numberOfClusters;
    int count = 0, value, i = 0, j = 0;
    double *adjMatrix;
    VerticesGroup **group;
    LinkedList *groupList;
    testGraph *TG = malloc(sizeof(testGraph));
    Graph *G = malloc(sizeof(Graph));
    assertFileOpen(file, path);
    assertMemoryAllocation(TG);
    assertMemoryAllocation(G);
    if (file == NULL){
        printf("Could not open file %s",path);
        return NULL;
    }

    while((c = fgetc(file)) != EOF)
    {
        if(count == 0) {
            if ((c == ' ' || c == '\n') && i > 0) {
                word[i] = '\0';
                n = atoi(word);
                i = 0;
                ++count;
                adjMatrix = malloc(n*n*sizeof(double));
                assertMemoryAllocation(adjMatrix);
            } else if (i < 9) {
                word[i] = c;
                ++i;
            }
        }
        else if(count >= 1 && count <= n * n) {
            if ((c == ' ' || c == '\n') && i > 0) {
                word[i] = '\0';
                value = atoi(word);
                i = 0;
                adjMatrix[count-1] = value;
                ++count;
            } else if (i < 9) {
                word[i] = c;
                ++i;
            }
        }
        else if(count == 1+n*n) {
            if ((c == ' ' || c == '\n') && i > 0) {
                word[i] = '\0';
                numberOfClusters = atoi(word);
                i = 0;
                ++count;
                group = malloc(numberOfClusters * sizeof(VerticesGroup));
                assertMemoryAllocation(group);
                group[0] = createVerticesGroup();
            } else if (i < 9) {
                word[i] = c;
                ++i;
            }
        }
        else if(count >= 2+n*n) {
            if ((c == ' ' || c == '\n') && i > 0) {
                word[i] = '\0';
                value = atoi(word);
                i = 0;
                ++count;
                addVertexToGroup(group[j], value);
                if(c == '\n') {
                    ++j;
                    group[j] = createVerticesGroup();
                }
            } else if (i < 9) {
                word[i] = c;
                ++i;
            }
        }
    }
    if(i>0){
        word[i] = '\0';
        value = atoi(word);
        i = 0;
        ++count;
        addVertexToGroup(group[j], value);
        if(c == '\n') {
            ++j;
            group[j] = createVerticesGroup();
        }
    }
    fclose(file);
    groupList = createLinkedList();
    for(i = 0; i < numberOfClusters; ++i)
        insertItem(groupList, group[i], 0);
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
char testGraphFromFile(char *path){
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
void printResultsFromOutputFile(char* output_file_path){
    FILE *output_file = fopen(output_file_path, "rb");
    int numberOfClusters, currentGroup, **groups;
    unsigned int currentGroupSize, i;
    assertFileOpen(output_file, output_file_path);

    assertFileRead(fread(&numberOfClusters, sizeof(int), 1, output_file), 1, output_file_path);
    printf("There are %d clusters.\n", numberOfClusters);
    groups = malloc(numberOfClusters * sizeof(int*));
    assertMemoryAllocation(groups);
    for(currentGroup = 0; currentGroup < numberOfClusters; ++currentGroup){
        assertFileRead(fread(&currentGroupSize, sizeof(int), 1, output_file), 1, output_file_path);
        groups[currentGroup] = malloc(currentGroupSize * sizeof(int));
        assertMemoryAllocation(groups[currentGroup]);
        assertFileRead(fread(groups[currentGroup], sizeof(int), currentGroupSize, output_file), currentGroupSize,
                       output_file_path);
        printf("Group %d is: ", currentGroup);
        for(i = 0; i < currentGroupSize; ++i)
            printf("%d, ", groups[currentGroup][i]);
        printf("\n");
        free(groups[currentGroup]);
    }
    free(groups);
    fclose(output_file);
}

int main() {
    printf("Testing graph 1 from file.\n");
    printf("Result: %d\n", testGraphFromFile("D:\\Users\\idodo\\OneDrive - mail.tau.ac.il\\Studies\\Tel Aviv University\\Semester 4\\Software Project\\Homework\\Project\\Project\\tests\\graph1-adjMat.txt"));
    /*printResultsFromOutputFile("out");*/
    printf("Testing graph 2 from file.\n");
    printf("Result: %d\n", testGraphFromFile("D:\\Users\\idodo\\OneDrive - mail.tau.ac.il\\Studies\\Tel Aviv University\\Semester 4\\Software Project\\Homework\\Project\\Project\\tests\\graph2-adjMat.txt"));
    /*printf("Testing graph 3 from file.\n");
    printf("Result: %d\n", testGraphFromFile("D:\\Users\\idodo\\OneDrive - mail.tau.ac.il\\Studies\\Tel Aviv University\\Semester 4\\Software Project\\Homework\\Project\\Project\\tests\\graph3-adjMat.txt"));*/
    printf("Testing graph 4 from file.\n");
    printf("Result: %d\n", testGraphFromFile("D:\\Users\\idodo\\OneDrive - mail.tau.ac.il\\Studies\\Tel Aviv University\\Semester 4\\Software Project\\Homework\\Project\\Project\\tests\\graph4-adjMat.txt"));
    printf("Testing graph 5 from file.\n");
    printf("Result: %d\n", testGraphFromFile("D:\\Users\\idodo\\OneDrive - mail.tau.ac.il\\Studies\\Tel Aviv University\\Semester 4\\Software Project\\Homework\\Project\\Project\\tests\\graph5-adjMat.txt"));
    printf("Testing graph 6 from file.\n");
    printf("Result: %d\n", testGraphFromFile("D:\\Users\\idodo\\OneDrive - mail.tau.ac.il\\Studies\\Tel Aviv University\\Semester 4\\Software Project\\Homework\\Project\\Project\\tests\\graph6-adjMat.txt"));
    printf("Testing graph 7 from file.\n");
    printf("Result: %d\n", testGraphFromFile("D:\\Users\\idodo\\OneDrive - mail.tau.ac.il\\Studies\\Tel Aviv University\\Semester 4\\Software Project\\Homework\\Project\\Project\\tests\\graph7-adjMat.txt"));
    printf("Testing graph 8 from file.\n");
    printf("Result: %d\n", testGraphFromFile("D:\\Users\\idodo\\OneDrive - mail.tau.ac.il\\Studies\\Tel Aviv University\\Semester 4\\Software Project\\Homework\\Project\\Project\\tests\\graph8-adjMat.txt"));
    printf("Testing graph 9 from file.\n");
    printf("Result: %d\n", testGraphFromFile("D:\\Users\\idodo\\OneDrive - mail.tau.ac.il\\Studies\\Tel Aviv University\\Semester 4\\Software Project\\Homework\\Project\\Project\\tests\\graph9-adjMat.txt"));
    printf("Testing random graph.\n");
    printf("Result: %d\n", testRandomGraph());

    return 0;
}
