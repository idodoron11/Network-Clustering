#ifndef CLUSTER_TESTER_H
#define CLUSTER_TESTER_H
#include "division.h"

graph *generateCommunitiesGraph(LinkedList *GroupList, int n, char noise);

char checkGroupListsEquality(LinkedList *GroupList1, LinkedList *GroupList2, int n);

#endif
