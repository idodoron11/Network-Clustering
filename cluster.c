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
    char *input_file_path, *output_file_path;

    if (argc == 3) {
        input_file_path = argv[1];
        output_file_path = argv[2];
    } else {
        input_file_path = malloc(sizeof(char) * FILE_PATH_MAX_LENGTH);
        assertMemoryAllocation(input_file_path);
        printf("Please specify a valid input file path.\n");
        if (scanf("%s", input_file_path) != 1) {
            printf("Error: the path exceeds 200 characters, and is too long to load.");
            exit(6);
        }
    }

    G = constructGraphFromInput(input_file_path);
    if (argc != 3)
        free(input_file_path);
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

    if (argc != 3) {
        output_file_path = malloc(sizeof(char) * FILE_PATH_MAX_LENGTH);
        assertMemoryAllocation(input_file_path);
        printf("Please specify a valid output file path.\n");
        if (scanf("%s", output_file_path) != 1) {
            printf("Error: the path exceeds 200 characters, and is too long to load.");
            exit(6);
        }
    }
    saveOutputToFile(groupsLst, output_file_path);

    if (argc != 3)
        free(output_file_path);

    return 0;
}
