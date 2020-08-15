#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "spmat.h"
#include "graph.h"
#include "VerticesGroup.h"
#include "LinkedList.h"
#include "division.h"
#include "ErrorHandler.h"

#define FILE_PATH_MAX_LENGTH 200

int main(int argc, char **argv) {
    int i = 1, j;
    LinkedList *groupsLst;
    LinkedListNode *node;
    VerticesGroup *group;
    VertexNode *vNode;
    Graph *G;

    srand(time(0));
    if (argc != 3) {
        throw("Two command line arguments expected");
    }

    G = constructGraphFromInput(argv[1]);
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

    saveOutputToFile(groupsLst, argv[2]);

    return 0;
}
