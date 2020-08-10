#include <stdlib.h>
#include "LinkedList.h"
#include "ErrorHandler.h"

/**
 * Creates a new empty linked list.
 * @return a pointer to the new list.
 */
LinkedList *createLinkedList(){
    LinkedList *list = malloc(sizeof(LinkedList));
    assertMemoryAllocation(list);
    list->length = 0;
    return list;
}

/**
 * Destroys a given list and frees up its allocated memory.
 * Does NOT free the pointers nodes are pointing to.
 * @param list a linked list.
 */
void freeLinkedList(LinkedList *list){
    LinkedListNode *temp;
    LinkedListNode *node = list->first;
    if (node != NULL) {
        do {
            temp = node->next;
            free(node);
            node = temp;
        } while (node != list->first);
    }
    free(list);
}

/**
 * Destroys a given GROUP list and frees up its allocated memory.
 * It also destroys every group in the list.
 * @param list a linked list, containing pointers to VerticesGroups.
 */
void deepFreeGroupList(LinkedList *groupList){
    LinkedListNode *node = groupList->first;
    VerticesGroup *group;
    int i;
    for(i = 0; i < groupList->length; ++i){
        group = node->pointer;
        freeVerticesGroup(group);
        node = node->next;
    }
    freeLinkedList(groupList);
}

/**
 * Inserts a new item to a given list.
 * In case this list functions as a list of VerticesGroups, 'value' should be a pointer to a 'VerticesGroup'.
 * @param list the list where the item should be inserted.
 * @param pointer a pointer to some variable of any type. (casting). Can be NULL.
 * @param index an integer.
 * @return a pointer to the new LinkedListNode.
 */
void *insertItem(LinkedList *list, void *pointer, int index){
    LinkedListNode *node = malloc(sizeof(LinkedListNode));
    assertMemoryAllocation(node);
    node->pointer = pointer;
    node->index = index;
    if (list->length != 0) {
        node->next = list->first;
        node->prev = list->first->prev;
        list->first->prev->next = node;
        list->first->prev = node;
    } else {
        list->first = node;
        node->prev = node;
        node->next = node;
    }
    list->length++;
    return node;
}

/**
 * Removes a specific item from a list.
 * @param list a linked list
 * @param item a LinkedListNode pointer to some item in 'list'.
 */
void removeItem(LinkedList *list, LinkedListNode *item){
    LinkedListNode *oldPrev = item->prev;
    LinkedListNode  *oldNext = item->next;
    if(list->length > 1){
        oldPrev->next = oldNext;
        oldNext->prev = oldPrev;
        free(item);
        list->length--;
    }
    else if(list->length == 1){
        free(item);
        list->first = NULL;
        list->length = 0;
    }
}

/**
 * Prints the vertex-coloring a given group-list defines over
 * a n-vertices graph.
 * @param groupList a partition of graph vertices into groups.
 * @param n the number of vertices in the graph.
 */
void printGroupList(LinkedList *groupList, int n){
    int L = groupList->length;
    int *coloring = malloc(sizeof(int) * n);
    assertMemoryAllocation(coloring);
    int i, j;
    LinkedListNode *node = groupList->first;
    VerticesGroup *group;
    VertexNode *vertex;

    for(i = 0; i < L; ++i){
        group = node->pointer;
        vertex = group->first;
        for(j = 0; j < group->size; ++j){
            coloring[vertex->index] = i;
            vertex = vertex->next;
        }
        node = node->next;
    }

    printf("[");
    if(n>0)
        printf("%d", coloring[0]);
    for(i = 1; i < n; ++i)
        printf(",%d", coloring[i]);
    printf("]\n");

    free(coloring);
}
