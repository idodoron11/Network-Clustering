#include <stdlib.h>
#include <time.h>
#include "spmat.h"
#include "graph.h"
#include "LinkedList.h"
#include "division.h"
#include "ErrorHandler.h"

int main(int argc, char **argv) {
    LinkedList *groupsLst;
    Graph *G;

    srand(time(0));
    if (argc != 3) {
        throw("Two command line arguments expected");
    }

    G = constructGraphFromInput(argv[1]);
    groupsLst = divisionAlgorithm(G);

    saveOutputToFile(groupsLst, argv[2]);
    deepFreeGroupList(groupsLst);
    destroyGraph(G);

    return 0;
}
