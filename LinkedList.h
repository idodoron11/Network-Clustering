#ifndef CLUSTER_LINKEDLIST_H
#define CLUSTER_LINKEDLIST_H

#include "VerticesGroup.h"

typedef struct _LinkedListNode {
    void *pointer;
    int index;
    struct _LinkedListNode *next;
    struct _LinkedListNode *prev;
} LinkedListNode;
typedef struct _LinkedList {
    LinkedListNode *first;
    int length;
} LinkedList;

LinkedList *createLinkedList();

void freeLinkedList(LinkedList *list);

void deepFreeGroupList(LinkedList *groupList);

void *insertItem(LinkedList *list, void *pointer, int index);

void removeItem(LinkedList *list, LinkedListNode *item);

#endif
