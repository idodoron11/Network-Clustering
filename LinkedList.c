#include <stdlib.h>
#include "LinkedList.h"
#include "ErrorHandler.h"

/**
 * Creates a new empty linked list.
 * @return a pointer to the new list.
 */
LinkedList *createLinkedList() {
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
void freeLinkedList(LinkedList *list) {
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
void deepFreeGroupList(LinkedList *groupList) {
    LinkedListNode *node = groupList->first;
    VerticesGroup *group;
    int i;
    for (i = 0; i < groupList->length; ++i) {
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
 * @return a pointer to the new LinkedListNode.
 */
void *insertItem(LinkedList *list, void *pointer) {
    LinkedListNode *node = malloc(sizeof(LinkedListNode));
    assertMemoryAllocation(node);
    node->pointer = pointer;
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
void removeItem(LinkedList *list, LinkedListNode *item) {
    LinkedListNode *oldPrev = item->prev;
    LinkedListNode *oldNext = item->next;
    oldPrev->next = oldNext;
    oldNext->prev = oldPrev;

    if (item == list->first) {
        if (list->length == 1) {
            list->first = NULL;
        } else {
            list->first = oldNext;
        }
    }
    free(item);
    list->length--;
}
