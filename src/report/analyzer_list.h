#include "../utils.c"
#include "./analyzer_data_structure.h"
#include "./analyzer_node.h"

#ifndef ANALYZER_LIST_DATA_STRUCTURE_H
#define ANALZYER_LIST_DATA_STRUCTURE_H
#include <stdio.h>  //print etc
/*  File where we define the structure of the analyzerList with all the analyzer
 *  nodes
*/

// analyzerList of the analyzer nodes.
// It handles de-allocating nodes when they are removed.
//
// Methods:
//- constructoranalyzerListEmpty        -> Creates empty analyzerList
//- constructoranalyzerListOne          -> Creates analyzerList with one node (passed in params)
//- destructoranalyzerList              -> Destroys the analyzerList (also de-allocates nodes)
//- analyzerListisEmpty                     -> If analyzerList is empty or not
//- analyzerListAppend                      -> appends new node to the end of the analyzerList
//- getNodeByIndex              -> returns reference to node with given index
//- getNodeWithPath             -> returns reference to node with given path
//- removeElementByPath         -> Remove (and de-allocates) node with give
// filePath
//- removeFirst                 -> Remove (and de-allocates) first node
//- removeLast                  -> Remove (and de-allocates) last node
//- updateFileData              -> Adds new stats to the relative fileWithStats
//                                 (if not present appends a new node)
//- removeLast                  -> Remove (and de-allocates) last node
//- printanalyzerList                   -> prints the analyzerList
typedef struct {
  // pointer to the first node
  analyzerNode *firstNode;
  // how many nodes are currently in the analyzerList
  int count;
} analyzerList;

// Returns reference to new empty analyzerList
analyzerList *constructorAnalyzerListEmpty() {
  analyzerList *l = malloc(sizeof(analyzerList));
  l->count = 0;
  l->firstNode = NULL;
}

//  Returns reference to new analyzerList w/ just one item - TESTED✔️
analyzerList *constructoranalyzerListOne(analyzer *a) {
  analyzerNode *firstNode = constructorAnalyzerNode(a);
  analyzerList *l = constructorAnalyzerListEmpty();
  l->count = 1;
  l->firstNode = firstNode;
  return l;
}

// Deletes l and everything within, de-allocating what's needed
void destructoraAnalyzerList(analyzerList *l) {
  analyzerNode *current = l->firstNode;
  // delete every node until none is left
  while (current != NULL) {
    analyzerNode *nextNode = current->nextNode;
    deleteAnalyzerNode(current);
    current = nextNode;
  }
  free(l);
}

// Returns wether the analyzeranalyzerList is empty or not
bool analyzerListisEmpty(analyzerList *l) { return l->count == 0; }

// Appends new node to end of analyzeranalyzerList - TESTED✔️
//
// ⚠️ do NOT deallocate fs, the analyzeranalyzerList will take care of it when needed
void analyzerListAppend(analyzerList *analyzerList, analyzer *fs) {
  analyzerNode *node = constructorAnalyzerNode(fs);
  if (analyzerListisEmpty(analyzerList)) {
    analyzerList->firstNode = node;
  } else {
    analyzerNode *cursor = analyzerList->firstNode;
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    cursor->nextNode = node;
    node->previousNode = cursor;
  }
  analyzerList->count++;
}

// Returns node in the given position (starting from 0)
// Returns NULL if not found
analyzerNode *analyzerListGetNodeByIndex(analyzerList *analyzerLista, int index) {
  analyzerNode *result = NULL;
  if (index >= 0 && index < analyzerLista->count) {
    result = analyzerLista->firstNode;
    int i;
    for (i = 0; i < index; i++) {
      result = result->nextNode;
    }
  }
  return result;
}

// Returns reference to node w/ given path or NULL
analyzerNode *analyzerListGetNodeByID(analyzerList *analyzerList, int pid) {
  analyzerNode *current = analyzerList->firstNode;
  while (current != NULL) {
    if (current->a->pid == pid) {
      return current;
    }
    current = current->nextNode;
  }
}

// Removes the element with the specified path (first occourrence only).
// Also handles its de-allocation.
// TODO test!
void analyzerListRemoveElementByID(analyzerList *analyzerList, int pid) {
  if (DEBUGGING)
    printf("Getting element with pid \"%d\" for deletion\n", pid);
  analyzerNode *targetNode = analyzerListGetNodeByID(analyzerList, pid);
  if (targetNode != NULL) {
    if (DEBUGGING)
      printf("Found element with pid \"%d\", its @%p\n", pid, targetNode);
    analyzerNode *prev = targetNode->previousNode;
    analyzerNode *next = targetNode->nextNode;
    prev->nextNode = next;
    next->previousNode = prev;
    deleteAnalyzerNode(targetNode);
  } else {
    if (DEBUGGING)
      printf("Element with pid \"%d\" is not in this analyzerList so it was not "
             "deleted\n",
             pid);
  }
}

// Removes first element from the analyzerNodeanalyzerList.
// (and handles its de-allocation)
void analyzerListRemoveFirst(analyzerList *analyzerLista) {
  if (!analyzerListisEmpty(analyzerLista)) {
    analyzerNode *newFirstNode = NULL;
    // TODO: I think this check is redundant given that here analyzerList is not empty
    if (analyzerLista->firstNode != NULL)
      newFirstNode = analyzerLista->firstNode->nextNode;
    deleteAnalyzerNode(analyzerLista->firstNode);
    analyzerLista->firstNode = newFirstNode;
    analyzerLista->count--;
  }
}

// Removes the last element from the analyzerList.
// (and handles its de-allocation)
void analyzerListRemoveLast(analyzerList *analyzerList) {
  if (!analyzerListisEmpty(analyzerList)) {
    analyzerNode *cursor = analyzerList->firstNode;
    // If analyzerList has just one item
    if (analyzerList->count == 1) {
      // Detach
      analyzerList->firstNode = NULL;
      // Delete
      deleteAnalyzerNode(cursor);
    } else {
      // Get cursor to the last analyzerList node
      while (cursor->nextNode != NULL) {
        cursor = cursor->nextNode;
      }
      // Detach from the analyzerList
      analyzerNode *penultimate = cursor->previousNode;
      penultimate->nextNode = NULL;
      // Delete
      deleteAnalyzerNode(cursor);
    }
    analyzerList->count--;
  }
}

/*
// Prints the analyzerList on stdOut TESTED✔️
void printanalyzerList(analyzerList *analyzerList) {
  analyzerNode *cursor = analyzerList->firstNode;
  while (cursor != NULL) {
    printFileWithStats(cursor->val);
    cursor = cursor->nextNode;
  }
}
*/
/*
// main di prova per testare
int main(int c, char *argv[]) {
  int retCode = 0;
  char *prova = "GATTO\0";
  int oc[256] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0}; // non li metto tutti, sei matto
  analyzerNode *fs = constructoranalyzerListItem(prova, 20, oc, true, NULL, NULL);
  printanalyzerListItem(fs);
  prova = "deleted\0";
  oc[0] = -1;
  printanalyzerListItem(fs);
  char *testo1 = "EL1";
  analyzerNode *f1 = constructoranalyzerListItem(testo1, 20, oc, true, NULL, NULL);
  printanalyzerListItem(f1);
  char *testo2 = "EL2";
  analyzerNode *f2 = constructoranalyzerListItem(testo2, 20, oc, true, NULL, NULL);
  printanalyzerListItem(f2);
  analyzerList *analyzerLista = malloc(sizeof(analyzerList));

  // constructoranalyzerListOne(analyzerLista,f1);
  constructoranalyzerListEmpty(analyzerLista);
  analyzerListAppend(analyzerLista, f1);
  analyzerListAppend(analyzerLista, f2);

  printanalyzerList(analyzerLista);
  printf("%d\n", analyzerLista->count);
  // analyzerListItem *f3 = getFirst(analyzerLista);
  // printanalyzerListItem(f3);
  // removeLast(analyzerLista);
  // removeLast(analyzerLista);
  // removeLast(analyzerLista);
  removeFirst(analyzerLista);
  removeFirst(analyzerLista);
  removeFirst(analyzerLista);
  printanalyzerList(analyzerLista);
  printf("elementi della analyzerLista %d\n", analyzerLista->count);
  return retCode;
  return 0;
}
*/
#endif
