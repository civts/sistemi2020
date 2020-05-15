#include <stdlib.h> // malloc & free
#ifndef INT_LIST_H
#define INT_LIST_H
// This files defines a list of integers

typedef struct intNode_t {
  struct intNode_t *nextNode;
  int value;
} intNode;

typedef struct {
  intNode *firstNode;
  intNode *lastNode;
} intList;

intNode *constructorIntNode(int v) {
  intNode *node = (intNode *)malloc(sizeof(intNode));
  node->value = v;
  node->nextNode = NULL;
  return node;
}

void addToIntList(intList *l, int value) {
  if (l->firstNode == NULL) {
    l->firstNode = l->lastNode = constructorIntNode(value);
  } else {
    intNode *lastNode = l->lastNode;
    intNode *newNode = constructorIntNode(value);
    lastNode->nextNode = newNode;
    l->lastNode = newNode;
  }
}

intList *constructorIntList() {
  intList *l = (intList *)malloc(sizeof(intList));
  l->firstNode = NULL;
  l->lastNode = NULL;
}

void destructorIntList(intList *l) {
  intNode *node = l->firstNode;
  while (node != NULL) {
    intNode *tmp = node->nextNode;
    free(node);
    node = tmp;
  }
  free(l);
}

#endif