#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cluster.h"
#include "spmat.h"
#include "graph.h"
#include "VerticesGroup.h"
#include "LinkedList.h"
#include "division.h"
#include "defs.h"

int cluster() {
    int i = 1, j;
    LinkedList *groupsLst;
    LinkedListNode *node;
    VerticesGroup *group;
    VertexNode *vNode;
    Graph *G = constructGraphFromInput(GRAPH_FILE_PATH);
    srand(time(0));
    groupsLst = divisionAlgorithm(G);
    node = groupsLst->first;
    if (node != NULL) {
        do {
            group = (VerticesGroup *) node->pointer;
            vNode = group->first;
            for (j = 0; j < group->size; j++) {
                vNode = vNode->next;
            }
            i++;
            node = node->next;
        } while (node != groupsLst->first);
    }

    saveOutputToFile(groupsLst, "out");

    return 0;
}
