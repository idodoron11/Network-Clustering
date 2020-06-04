#include <stdio.h>
#include "graph.h"

int main() {
    graph* G = constructEmptyGraph(10);
    attachVertices(G, 2, 4);
    attachVertices(G, 9, 1);
    printGraph(G);
    destroyGraph(G);

    return 0;
}
