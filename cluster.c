#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "spmat.h"
#include "graph.h"
#include "VerticesGroup.h"
#include "LinkedList.h"
#include "division.h"
#include "defs.h"

int main() {
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
            printf("Group %d:\n", i);
            group = (VerticesGroup *) node->pointer;
            vNode = group->first;
            for (j = 0; j < group->size; j++) {
                printf("%d ", vNode->index);
                vNode = vNode->next;
            }
            printf("\n\n");
            i++;
            node = node->next;
        } while (node != groupsLst->first);
    }

    saveOutputToFile(groupsLst, "out");

    return 0;
}
