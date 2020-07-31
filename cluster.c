#include <stdio.h>
#include "graph.h"

int main() {
    graph* G = constructGraphFromInput("graph.in");
    printGraph(G);

    return 0;
}
