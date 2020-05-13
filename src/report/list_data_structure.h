#include "../utils.c"
#include "./file_with_stats_data_structure.h"
#include "./file_with_stats_node.h"

#ifndef LIST_DATA_STRUCTURE_H
#define LIST_DATA_STRUCTURE_H
#include <stdio.h>  //print etc
/*  File where we define the structure of the list with all the fileWithStats
 *  nodes
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

// Deletes l and everything within, de-allocating what's needed
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
fileWithStats *getFWSByIndex(list *lista, int index) {
  fwsNode *result = NULL;
  if (index >= 0 && index < lista->count) {
    result = lista->firstNode;
    int i;
    for (i = 0; i < index; i++) {
      result = result->nextNode;
    }
  }
  return result->val;
}

// Returns reference to file  w/ given path or NULL
fileWithStats *getFWSByPath(list *list, char *path) {
  fwsNode *current = list->firstNode;
  while (current != NULL) {
    fileWithStats * tmp = current->val;
    if(tmp!=NULL){
      if ( streq(tmp->path, path) ) {
        return tmp;
      }
    }
    current = current->nextNode;
  }
}
//returns reference to the actual node, pretty useless
fwsNode *getNodeByPath(list *list, char *path) {
  fwsNode *current = list->firstNode;
  while (current != NULL) {
    fileWithStats * tmp = current->val;
    if(tmp!=NULL){
      if ( streq(tmp->path, path) ) {
        return current;
      }
    }
    current = current->nextNode;
  }
}
// return reference to file w/ give id
fileWithStats *getFWSByID(list *list, uint id) {
  fwsNode *current = list->firstNode;
  while (current != NULL) {
    if (current->val->id == id) {
      return current->val;
    }
    current = current->nextNode;
  }
}
// return reference to node w/ give id
fwsNode *getNodeByID(list *list, uint id) {
  fwsNode *current = list->firstNode;
  while (current != NULL) {
    if (current->val->id == id) {
      return current;
    }
    current = current->nextNode;
  }
}
// Removes first element from the fwsNodeList.
// (and handles its de-allocation)
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
// Removes the last element from the list.
// (and handles its de-allocation)
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
// If file is not present it is appended to the end of the list.
// return 0=false new element non appended
// return 1=true if  if new element is appended
// ⚠️ After this you should deallocate the newData fileWithStats accordingly.
bool updateFileData(list *list, uint id, fileWithStats *newData) {
  fwsNode *targetNode = getNodeByID(list, id);
  if (targetNode != NULL) {
    uint charsToAdd = newData->totalCharacters;
    uint *occourrrences = newData->occorrenze;
    addStatsToFWS(targetNode->val, charsToAdd, occourrrences);
    return false;
  } else {
    append(list, newData);
    return true;
  }
}

void updateFilePath(list *list,uint id,char* path){
  fileWithStats *target = getFWSByID(list, id);
  if (target != NULL) {
    char * oldPath = target->path;
    char * tmp = malloc(strlen(oldPath)+strlen(oldPath));
    strcpy(tmp,oldPath);
    strcat(tmp,path);
    target->path = tmp;
    free(oldPath);
  }
}
void removeElementByID(list *list, uint id){
  fwsNode *target = getNodeByID(list,id);
  if(target!=NULL){
    fwsNode *prev = target->previousNode;
    fwsNode *next = target->nextNode;
    deleteFwsNode(target);
    if(prev!=NULL)
      prev->nextNode = next;
    if(next!=NULL)
      next->previousNode = prev;
    list->count--;
  }
}
void removeElementByPath(list * list,char* path){
  fwsNode *target = getNodeByPath(list,path);
  if(target!=NULL){
    fwsNode *prev = target->previousNode;
    fwsNode *next = target->nextNode;
    deleteFwsNode(target);
    if(prev!=NULL)
      prev->nextNode = next;
    if(next!=NULL)
      next->previousNode = prev;
    list->count--;
  }
}
/*
// Adds the stats from newData to the right file in this list.
// If file is not present it is appended to the end of the list.
// return 0=false new element non appended
// return 1=true if  if new element is appended
// ⚠️ After this you should deallocate the newData fileWithStats.
bool updateFileData(list *list, char *filePath, fileWithStats *newData) {
  fwsNode *targetNode = getNodeWithPath(list, filePath);
  if (targetNode != NULL) {
    uint charsToAdd = newData->totalCharacters;
    uint *occourrrences = newData->occorrenze;
    addStatsToFWS(targetNode->val, charsToAdd, occourrrences);
    return false;
  } else {
    append(list, newData);
    return true;
  }
}
*/
// Prints the list on stdOut TESTED✔️
void printList(list *list) {
  fwsNode *cursor = list->firstNode;
  while (cursor != NULL) {
    printFileWithStats(cursor->val);
    cursor = cursor->nextNode;
  }
}

#endif
/*
// main di prova per testarefwsNode
int main(int c, char *argv[]) {
  list * lista =  constructorListEmpty();
  char * path = "patate\0";
  append(lista, constructorFWS(path,1,0,NULL,false));
  printList(lista);
  printf("elementi della lista %d\n", lista->count);

  //removeElementByPath(lista,path);
  //printFileWithStats(getFWSByPath(lista,path));
  //printFileWithStats(getFWSByIndex(lista,0));
  //printFileWithStats(getFWSByID(lista,1));
  //removeElementByID(lista,1);
  removeElementByPath(lista,path);
  return 0;
}*/
