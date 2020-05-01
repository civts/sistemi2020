#include "./file_with_stats_data_structure.c"
#include <stdio.h> //print etc
#include <stdlib.h>

// FileWithStats list node
typedef struct fwsNode_t {
  fileWithStats *val; // it's already in the heap anyway
  struct fwsNode_t *nextNode;
  struct fwsNode_t *previousNode;
} fwsNode;

// Create a new node with the given fileWithStats. TESTED✔️
//
// previos and next are both initialized to NULL
fwsNode *constructorFwsNode(fileWithStats *fs) {
  fwsNode *result = (fwsNode *)malloc(sizeof(fwsNode));
  result->val = fs;
  result->previousNode = NULL;
  result->nextNode = NULL;
  return result;
}

// Delete the node AND its fileWithStats - TESTED✔️
// Also de-allocates the memory
// (does NOT delete the next and prevoius nodes otherwise it would cascade)
void deleteFwsNode(fwsNode *fs) {
  deleteFWS(fs->val);
  free(fs);
}

// UNIT TEST MAIN
void main() {
  // test constructor: should print only 1s
  fileWithStats *fs =
      (fileWithStats *)malloc(sizeof(int)); // dont' need a real one here
  fwsNode *node = constructorFwsNode(fs);
  printf("%d\n", fs == node->val);
  printf("%d\n", NULL == node->previousNode);
  printf("%d\n", NULL == node->nextNode);
  // test destructor: should not eat up your memory, check in sys console
  int i;
  for (i = 0; i < 1000000; i++) {
    char *a = "saba";
    int oc[256];
    int j;
    for (j = 0; j < 256; j++) {
      oc[j] = 6;
    }
    fs = constructorFWS(a, 0, oc, false);
    node = constructorFwsNode(fs);
    deleteFwsNode(node);
  }
}
