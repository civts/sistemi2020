#include "./file_with_stats_data_structure.c";
#include <stdio.h> //print etc
#include <stdlib.h>

// FileWithStats list node
typedef struct fwsNode_t {
  fileWithStats *val; // it's already in the heap anyway
  struct fwsNode_t *nextNode;
  struct fwsNode_t *previousNode;
} fwsNode;

// Create a new node with the given fileWithStats.
//
// previos and next are both initialized to NULL
fwsNode *constructorFwsNode(fileWithStats *fs) {
  fwsNode *result = (fwsNode *)malloc(sizeof(fwsNode));
  result->val = fs;
  result->previousNode = NULL;
  result->nextNode = NULL;
  return result;
}

// Delete the node AND its fileWithStats
void deleteFwsNode(fwsNode *fs) {
  deleteFWS(fs->val);
  free(fs);
}