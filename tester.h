#ifndef CLUSTER_TESTER_H
#define CLUSTER_TESTER_H
#include "division.h"

typedef struct _testGraph{
    Graph *G;
    LinkedList *GroupList;
} testGraph;

Graph *generateCommunitiesGraph(LinkedList *GroupList, int n, char noise);

char checkGroupListsEquality(LinkedList *GroupList1, LinkedList *GroupList2, int n);

testGraph *createTestGraph(int edges[][2], int length, int n, LinkedList *GroupList);

void destroyTestGraph(testGraph *TG);

char performTest(testGraph *TG);

char testGraphFromFile(char *path);

void printResultsFromOutputFile(char* output_file_path);

#endif
