#ifndef ANALYZER_NODE_H
#define ANALYZER_NODE_H
#include "./analyzer_data_structure.h"

// analyzer list node
typedef struct analyzerNode_t {
  //The analyzer of this node
  analyzer *a; // it's already in the heap anyway
  struct analyzerNode_t *nextNode;
  struct analyzerNode_t *previousNode;
} analyzerNode;

// Create a new node with the given analyzer. TESTED✔️
//
// previos and next are both initialized to NULL
analyzerNode *constructorAnalyzerNode(analyzer *a) {
  analyzerNode *result = (analyzerNode *)malloc(sizeof(analyzerNode));
  result->a = a;
  result->previousNode = NULL;
  result->nextNode = NULL;
  if (DEBUGGING)
    printf(
        "Creating a new AnalyzerNode instance @%p for Analyzer %p w/ pid %d\n",
        result, a, a->pid);
  return result;
}

// Delete the node AND its analyzer - TESTED✔️
// Also de-allocates the memory
// (does NOT delete the next and prevoius nodes otherwise it would cascade)
void deleteAnalyzerNode(analyzerNode *a) {
  if (DEBUGGING)
    printf("deleting AnalyzerNode @%p (pid %d)\n", a, a->a->pid);
  deleteAnalyzer(a->a);
  free(a);
}
/*
// UNIT TEST MAIN
void main() {
  // test constructor: should print only 1s
  analyzer *a = constructorAnalyzer(55);
  analyzerNode *node = constructorAnalyzerNode(a);
  printf("%d\n", a == node->a);
  printf("%d\n", NULL == node->previousNode);
  printf("%d\n", NULL == node->nextNode);
  deleteAnalyzerNode(node);
}
*/
#endif
