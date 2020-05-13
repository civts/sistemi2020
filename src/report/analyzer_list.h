#include "../utils.c"
#include "./file_with_stats_list.h"
#include "./analyzer_node.h"

#ifndef ANALYZER_LIST_DATA_STRUCTURE_H
#define ANALZYER_LIST_DATA_STRUCTURE_H
#include <stdio.h> //print etc
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
  analyzerList *l =(analyzerList *)malloc(sizeof(analyzerList));
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
bool analyzerListIsEmpty(analyzerList *l) { return l->count == 0; }

// Appends new analyzer node to end of analyzeranalyzerList - TESTED✔️
//
// ⚠️ do NOT deallocate an, the analyzeranalyzerList will take care of it when
// needed
void analyzerListAppend(analyzerList *analyzerList, analyzer *an) {
  analyzerNode *node = constructorAnalyzerNode(an);
  if (analyzerListIsEmpty(analyzerList)) {
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

// Returns reference to the analyzer in the list w/ the given pid or NULL
analyzer *analyzerListGetAnalyzerByPid(analyzerList *analyzerList, uint pid) {
  analyzerNode *current = analyzerList->firstNode;
  while (current != NULL) {
    analyzer *a = current->a;
    if (a != NULL) {
      if ((a->pid) == pid) {
        return a;
      }
    }
    current = current->nextNode;
  }
  return NULL;
}

// Returns reference to the node of the list which analyzer has the given pid
// (or NULL if it's not found)
analyzerNode *analyzerListGetNodeByPid(analyzerList *analyzerList, uint pid) {
  analyzerNode *current = analyzerList->firstNode;
  while (current != NULL) {
    analyzer *a = current->a;
    if (a != NULL) {
      if ((a->pid) == pid) {
        return current;
      }
    }
    current = current->nextNode;
  }
  return NULL;
}

// Removes the element with the specified pid (first occourrence only).
// Also handles its de-allocation.
// TODO test!
void analyzerListRemoveElementByPid(analyzerList *analyzerList, uint pid) {
  if (DEBUGGING)
    printf("Getting element with pid %u for deletion\n", pid);
  analyzerNode *targetNode = analyzerListGetNodeByPid(analyzerList, pid);
  if (targetNode != NULL) {
    if (DEBUGGING)
      printf("Found element with pid %u, its @%p\n", pid, targetNode);
    analyzerNode *prev = targetNode->previousNode;
    analyzerNode *next = targetNode->nextNode;
    if (prev != NULL)
      prev->nextNode = next;
    if (next != NULL)
      next->previousNode = prev;
    deleteAnalyzerNode(targetNode);
    analyzerList->count--;
  } else {
    if (DEBUGGING)
      printf(
          "Element with pid \"%u\" is not in this analyzerList so it was not "
          "deleted\n",
          pid);
  }
}

// Removes first element from the analyzerNodeanalyzerList.
// (and handles its de-allocation)
void analyzerListRemoveFirst(analyzerList *analyzerLista) {
  if (!analyzerListIsEmpty(analyzerLista)) {
    analyzerNode *newFirstNode = analyzerLista->firstNode->nextNode;
    deleteAnalyzerNode(analyzerLista->firstNode);
    analyzerLista->firstNode = newFirstNode;
    analyzerLista->count--;
  }
}

// Removes the last element from the analyzerList.
// (and handles its de-allocation)
void analyzerListRemoveLast(analyzerList *analyzerList) {
  if (!analyzerListIsEmpty(analyzerList)) {
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

// Prints the analyzerList on stdOut TESTED✔️
void printAnalyzerList(analyzerList *analyzerList) {
  analyzerNode *cursor = analyzerList->firstNode;
  while (cursor != NULL) {
    printAnalyzer(cursor->a);
    cursor = cursor->nextNode;
  }
}

/*
// main di prova per testare
int main(int c, char *argv[]) {
  analyzerList * list = constructorAnalyzerListEmpty();
  analyzer *  a1 = constructorAnalyzer(1);
  analyzer *  a2 = constructorAnalyzer(2);
  analyzer *  a3 = constructorAnalyzer(3);
  analyzer *  a4 = constructorAnalyzer(4);
  analyzerListAppend(list,a1);
  analyzerListAppend(list,a2);
  analyzerListAppend(list,a3);
  //int x = analyzerListGetAnalyzerByPid(0);
  //analyzerListRemoveElementByPid(list,1);
  printAnalyzerList(list);
  analyzerListRemoveFirst(list);
  analyzerListRemoveLast(list);
  printAnalyzerList(list);
  //
  printAnalyzer(analyzerListGetAnalyzerByIndex(list,0));
  analyzerListRemoveElementByPid(list,2);
  return 0;
}
*/
#endif
