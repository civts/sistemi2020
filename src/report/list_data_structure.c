#include "../utils.c";
#include "./file_with_stats_data_structure.c";
#include "./file_with_stats_node.h";
#include <string.h> //strlen, strcpy
#include <stdio.h>  //print etc
#include <stdlib.h> // malloc, free
/*  File where we define the structure of the list with all the fileWithStats
 * nodes
 *
 * We have:
 *  - a constructor
 *  - a destructor
 *  - a method for adding additional stats (that may come from other chunks of
 *    the same file)
*/

// List of the fileWithStats nodes.
// It handles de-allocating nodes when they are removed.
//
// Methods:
//- constructorListEmpty        -> Creates empty list
//- constructorListOne          -> Creates list with one node (passed in params)
//- destructorList              -> Destroys the list (also de-allocates nodes)
//- isEmpty                     -> If list is empty or not
//- append                      -> appends new node to the end of the list
//- getNodeByIndex              -> returns reference to node with given index
//- getNodeWithPath             -> returns reference to node with given path
//- removeElementByPath         -> Remove (and de-allocates) node with give
// filePath
//- removeFirst                 -> Remove (and de-allocates) first node
//- removeLast                  -> Remove (and de-allocates) last node
//- updateFileData              -> Adds new stats to the relative fileWithStats
//                                 (if not present appends a new node)
//- removeLast                  -> Remove (and de-allocates) last node
//- printList                   -> prints the list
typedef struct {
  // pointer to the first node
  fwsNode *firstNode;
  // how many nodes are currently in the list
  int count;
} list;

// Returns reference to new empty list
list *constructorListEmpty() {
  list *l = malloc(sizeof(list));
  l->count = 0;
  l->firstNode = NULL;
}

//  Returns reference to new list w/ just one item - TESTED✔️
list *constructorListOne(fileWithStats *fs) {
  fwsNode *firstNode = constructorFwsNode(fs);
  list *l = constructorListEmpty();
  l->count = 1;
  l->firstNode = firstNode;
  return l;
}

void destructorList(list *l) {
  fwsNode *current = l->firstNode;
  // delete every node until none is left
  while (current != NULL) {
    fwsNode *nextNode = current->nextNode;
    deleteFwsNode(current);
    current = nextNode;
  }
  free(l);
}

// Returns wether the list is empty or not
bool isEmpty(list *l) { return l->count == 0; }

// Appends new node to end of list - TESTED✔️
//
// ⚠️ do NOT deallocate fs, the list will take care of it when needed
void append(list *list, fileWithStats *fs) {
  fwsNode *node = constructorFwsNode(fs);
  if (isEmpty(list)) {
    list->firstNode = node;
  } else {
    fwsNode *cursor = list->firstNode;
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    cursor->nextNode = node;
    node->previousNode = cursor;
  }
  list->count++;
}

// Returns node in the given position (starting from 0)
// Returns NULL if not found
fwsNode *getNodeByIndex(list *lista, int index) {
  fwsNode *result = NULL;
  if (index >= 0 && index < lista->count) {
    result = lista->firstNode;
    int i;
    for (i = 0; i < index; i++) {
      result = result->nextNode;
    }
  }
  return result;
}

// Returns reference to node w/ given path or NULL
fwsNode *getNodeWithPath(list *list, char *path) {
  fwsNode *current = list->firstNode;
  while (current != NULL) {
    if (comparePaths(current->val->path, path)) {
      return current;
    }
    current = current->nextNode;
  }
}

// Removes the element with the specified path (first occourrence only)
// TODO test!
void removeElementByPath(list *list, char *path) {
  fwsNode *targetNode = getNodeWithPath(list, path);
  if (targetNode != NULL) {
    fwsNode *prev = targetNode->previousNode;
    fwsNode *next = targetNode->nextNode;
    prev->nextNode = next;
    next->previousNode = prev;
    deleteFwsNode(targetNode);
  }
}

// removes first element from the fwsNodeList
void removeFirst(list *lista) {
  if (!isEmpty(lista)) {
    fwsNode *newFirstNode = NULL;
    // TODO: I think this check is redundant given that here list is not empty
    if (lista->firstNode != NULL)
      newFirstNode = lista->firstNode->nextNode;
    deleteFwsNode(lista->firstNode);
    lista->firstNode = newFirstNode;
    lista->count--;
  }
}

// Remove the last element from the list
void removeLast(list *list) {
  if (!isEmpty(list)) {
    fwsNode *cursor = list->firstNode;
    // If list has just one item
    if (list->count == 1) {
      // Detach
      list->firstNode = NULL;
      // Delete
      deleteFwsNode(cursor);
    } else {
      // Get cursor to the last list node
      while (cursor->nextNode != NULL) {
        cursor = cursor->nextNode;
      }
      // Detach from the list
      fwsNode *penultimate = cursor->previousNode;
      penultimate->nextNode = NULL;
      // Delete
      deleteFwsNode(cursor);
    }
    list->count--;
  }
}

// Adds the stats from newData to the right file in this list.
// if file is not present it is appended to the end of the list
void updateFileData(list *list, char *filePath, fileWithStats *newData) {
  fwsNode *targetNode = getNodeWithPath(list, filePath);
  if (targetNode != NULL) {
    int charsToAdd = newData->totalCharacters;
    int *occourrrences = newData->occorrenze;
    addStatsToFWS(targetNode->val, charsToAdd, occourrrences);
  } else {
    append(list, newData);
  }
}

// Prints the list on stdOut TESTED✔️
void printList(list *list) {
  fwsNode *cursor = list->firstNode;
  while (cursor != NULL) {
    printFileWithStats(cursor->val);
    cursor = cursor->nextNode;
  }
}

// main di prova per testare
int main(int c, char *argv[]) {
  int retCode = 0;
  char *prova = "GATTO\0";
  int oc[256] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0}; // non li metto tutti, sei matto
  fwsNode *fs = constructorListItem(prova, 20, oc, true, NULL, NULL);
  printListItem(fs);
  prova = "deleted\0";
  oc[0] = -1;
  printListItem(fs);
  char *testo1 = "EL1";
  fwsNode *f1 = constructorListItem(testo1, 20, oc, true, NULL, NULL);
  printListItem(f1);
  char *testo2 = "EL2";
  fwsNode *f2 = constructorListItem(testo2, 20, oc, true, NULL, NULL);
  printListItem(f2);
  list *lista = malloc(sizeof(list));

  // constructorListOne(lista,f1);
  constructorListEmpty(lista);
  append(lista, f1);
  append(lista, f2);

  printList(lista);
  printf("%d\n", lista->count);
  // listItem *f3 = getFirst(lista);
  // printListItem(f3);
  // removeLast(lista);
  // removeLast(lista);
  // removeLast(lista);
  removeFirst(lista);
  removeFirst(lista);
  removeFirst(lista);
  printList(lista);
  printf("elementi della lista %d\n", lista->count);
  return retCode;
}
