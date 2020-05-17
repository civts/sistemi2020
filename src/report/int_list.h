#include <stdlib.h> // malloc & free
#ifndef INT_LIST_H
#define INT_LIST_H
// This files defines a list of integers

typedef struct intNode_t {
  struct intNode_t *nextNode;
  struct intNode_t *previousNode;
  // struct intNode_t *previuosNode; ? 
  int value;
} intNode;

typedef struct {
  intNode *firstNode;
  int count;
} intList;


// list node with a int
intNode *constructorIntNode(int v);
//destructor for intNode  
void destructorIntNode(intNode * n);
//constructor for a empty int list
intList *constructorIntListEmpty();
//constructor for a int list with one element
intList *constructorIntListOne(int v);
// Returns wether the analyzeranalyzerList is empty or not
bool intListIsEmpty(intList *l) ;
// Returns reference to the int in the list w/ the given pid or NULL if none founded
intNode *intListGetElementByValue(intList *l, int v); 
// Removes the element with the specified value (first occourrence only).
void intListRemoveElementByValue(intList *l, int v);

// list node with a int
intNode *constructorIntNode(int v) {
  intNode *node = (intNode *)malloc(sizeof(intNode));
  node->value = v;
  node->nextNode = NULL;
  node->previousNode = NULL;
  return node;
}

//destructor for intNode  
void destructorIntNode(intNode * n){
  free(n);
}

//constructor for a empty int list
intList *constructorIntListEmpty() {
  intList *l = (intList *)malloc(sizeof(intList));
  l->firstNode = NULL;
  l->count=0;
}

//constructor for a int list with one element
intList *constructorIntListOne(int v) {
  intList *l = constructorIntListEmpty();
  intListAppend(l,v);
}

// Returns wether the analyzeranalyzerList is empty or not
bool intListIsEmpty(intList *l) { return l->count == 0; }

//appends element v to the end of the list v
void intListAppend(intList * l, int value){
  intNode * v = constructorIntNode(value);
  if (intListIsEmpty(l)) {
    l->firstNode = v;
  } else {
    analyzer *cursor = l->firstNode;
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    cursor->nextNode = v;
    v->previousNode = cursor;
  }
  l->count++;
}

// Returns reference to the int in the list w/ the given pid or NULL if none founded
intNode *intListGetIntNodeByValue(intList *l, int v) {
  intNode *current = l->firstNode;
  while (current != NULL) {
    if ((current->value) == v) {
      return current;
    }
    current = current->nextNode;
  }
  return NULL;
}

void intListRemoveElementByValue(intList *l, int v){
  intNode *targetNode = intListGetIntNodeByValue(l, v);
  if (targetNode != NULL) {
    intNode *prev = targetNode->previousNode;
    intNode *next = targetNode->nextNode;
    if (prev != NULL)
      prev->nextNode = next;
    if (next != NULL)
      next->previousNode = prev;
    destructorIntNode(targetNode);
    l->count--;
  }


}
void destructorIntList(intList *l) {
  intNode *node = l->firstNode;
  while (node != NULL) {
    intNode *tmp = node->nextNode;
    destructorIntNode(node);
    node = tmp;
  }
  free(l);
}

#endif