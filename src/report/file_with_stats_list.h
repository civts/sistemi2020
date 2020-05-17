#include "../utils.c"
#include "./file_with_stats_data_structure.h"

#ifndef FWSLIST_DATA_STRUCTURE_H
#define FWSLIST_DATA_STRUCTURE_H
#include <stdio.h> //print etc
/*  File where we define the structure of the fwsList with all the fileWithStats
 *  nodes
*/

// fwsList of the fileWithStats nodes.
// It handles de-allocating nodes when they are removed.
//
// Methods:
//- constructorfwsListEmpty        -> Creates empty fwsList
//- constructorfwsListOne          -> Creates fwsList with one node (passed in params)
//- destructorfwsList              -> Destroys the fwsList (also de-allocates nodes)
//- isEmpty                     -> If fwsList is empty or not
//- append                      -> appends new node to the end of the fwsList
//- getNodeByIndex              -> returns reference to node with given index
//- getNodeWithPath             -> returns reference to node with given path
//- removeElementByPath         -> Remove (and de-allocates) node with give
// filePath
//- removeFirst                 -> Remove (and de-allocates) first node
//- removeLast                  -> Remove (and de-allocates) last node
//- updateFileData              -> Adds new stats to the relative fileWithStats
//                                 (if not present appends a new node)
//- removeLast                  -> Remove (and de-allocates) last node
//- printfwsList                   -> prints the fwsList
typedef struct {
  // pointer to the first node
  fileWithStats *firstNode;
  // how many nodes are currently in the fwsfwsList
  int count;
} fwsList;


// Returns reference to new empty fwsList
fwsList *constructorFwsListEmpty(); 
//  Returns reference to new fwsList w/ just one item - TESTED✔️
fwsList *constructorFwsListOne(fileWithStats *fs);
// Deletes l and everything within, de-allocating what's needed
void destructorFwsList(fwsList *l) ;
// Returns wether the fwsList is empty or not
bool fwsListIsEmpty(fwsList *l);
// Appends new node to end of fwsList - TESTED✔️
void fwsListAppend(fwsList *l, fileWithStats *fs); 
// Returns node in the given position (starting from 0) Returns NULL if not found
fileWithStats *fwsListGetElementByIndex(fwsList *l, int index);
// Returns reference to file  w/ given path Returns NULL if not found
fileWithStats *fwsListGetElementByPath(fwsList *l, char *path); 
// return reference to file w/ give id Returns NULL if not found
fileWithStats *fwsListGetElementByID(fwsList *fwsList, uint id);
// Removes first element from the fileWithStatsfwsList.
void fwsListRemoveFirst(fwsList *l);
// Removes the last element from the fwsList.
void fwsListRemoveLast(fwsList *fwsList);
// Adds the stats from newData to the right file in this fwsList. If file is not present it is ignored.
void fwsListUpdateFileData(fwsList *l,uint id,uint charTot,uint charsToAdd,uint *occourrrences);
// Adds new chars to file path. IF file with this id is not present, it is ignored
void fwsListUpdateFilePath(fwsList *fwsList, uint id, char *path);
// remove element with given id from fwsList, delete true if deletion of said node id necessary. Does nothing if element is not found
void fwsListRemoveElementByID(fwsList *fwsList, uint id,bool delete);
// remove element with given PATH from fwsList, delete true if deletion of said node id necessary. Does nothing if element is not found
void fwsListRemoveElementByPath(fwsList *fwsList, char *path,bool delete);

void fwsListPrint(fwsList * l);

// Returns reference to new empty fwsList
fwsList *constructorFwsListEmpty() {
  fwsList *l = (fwsList *)malloc(sizeof(fwsList));
  l->count = 0;
  l->firstNode = NULL;
}

//  Returns reference to new fwsList w/ just one item - TESTED✔️
fwsList *constructorFwsListOne(fileWithStats *fs) {
  fwsList *l = constructorFwsListEmpty();
  fwsListAppend(l,fs);
  return l;
}

// Deletes l and everything within, de-allocating what's needed
void destructorFwsList(fwsList *l) {
  fileWithStats *current = l->firstNode;
  // delete every node until none is left
  while (current != NULL) {
    fileWithStats *nextNode = current->nextNode;
    destructorFWS(current);
    current = nextNode;
  }
  free(l);
}

// Returns wether the fwsList is empty or not
bool fwsListIsEmpty(fwsList *l) { return l->count == 0; }

// Appends new node to end of fwsList - TESTED✔️
//
// ⚠️ do NOT deallocate fs, the fwsList will take care of it when needed
void fwsListAppend(fwsList *l, fileWithStats *fs) {
  if (fwsListIsEmpty(l)) {
    l->firstNode = fs;
  } else {
    fileWithStats *cursor = l->firstNode;
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    cursor->nextNode = fs;
    fs->previousNode = cursor;
  }
  l->count++;
}

// Returns node in the given position (starting from 0)
// Returns NULL if not in range
fileWithStats *fwsListGetElementByIndex(fwsList *l, int index) {
  fileWithStats *result = NULL;
  if (index >= 0 && index < l->count) {
    result = l->firstNode;
    int i;
    for (i = 0; i < index; i++) {
      result = result->nextNode;
    }
  }
  return result;
}

// Returns reference to file  w/ given path or NULL if not FOUND
fileWithStats *fwsListGetElementByPath(fwsList *l, char *path) {
  fileWithStats *current = l->firstNode;
  while (current != NULL) {
    if (streq(current->path, path)) {
      return current;
    }
    current = current->nextNode;
  }
  //not found
  return NULL;
}

// return reference to file w/ give id or NULL if not found
fileWithStats *fwsListGetElementByID(fwsList *l, uint id) {
  fileWithStats *current = l->firstNode;
  while (current != NULL) {
    if (current->id == id) {
      return current;
    }
    current = current->nextNode;
  }
  //not found
  return NULL;
}

// Removes first element from the fileWithStatsfwsList.
// (and handles its de-allocation)
void fwsListRemoveFirst(fwsList *l) {
  if (!fwsListIsEmpty(l)) {
    fileWithStats *newFirstNode = l->firstNode->nextNode;
    destructorFWS(l->firstNode);
    l->firstNode = newFirstNode;
    l->count--;
  }
}

// Removes the last element from the fwsList.
// (and handles its de-allocation)
void fwsListRemoveLast(fwsList *l) {
  if (!fwsListIsEmpty(l)) {
    fileWithStats *cursor = l->firstNode;
    // Get cursor to the last fwsList node
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    // Detach from the fwsList
    fileWithStats *penultimate = cursor->previousNode;
    if(penultimate!=NULL)
      penultimate->nextNode = NULL;
    // Delete
    destructorFWS(cursor);
    l->count--;
  }
}

// Adds the stats from newData to the right file in this fwsList. if not present, is appended to the end of the list ? change the comment if required, now it is not
void fwsListUpdateFileData(fwsList *l,uint id,uint charTot,uint charsToAdd,uint *occourrrences) {
  fileWithStats *target = fwsListGetElementByID(l, id);
  if (target != NULL) {
    // if already exists
    fwsUpdateFileData(target,charTot, charsToAdd, occourrrences);
  }
  /*
  else{
    // if does not exists
    fileWithStats *newNode = constructorFWS(path,id,charTot,occourrrences,fromFolder);
    fwsListAppend(l,newNode);
  }
  */
}

// Adds new chars to file path. IF file with this id is not present, it is ignored
void fwsListUpdateFilePath(fwsList *l, uint id, char *path) {
  fileWithStats *target = fwsListGetElementByID(l, id);
  if (target != NULL) {
    fwsUpdateFilePath(target,path);
  }
}

// remove element with given id from fwsList, delete true if deletion of said node id necessary. Does nothing if element is not found
void fwsListRemoveElementByID(fwsList *l, uint id,bool delete) {
  fileWithStats *target = fwsListGetElementByID(l, id);
  if (target != NULL) {
    fileWithStats *prev = target->previousNode;
    fileWithStats *next = target->nextNode;
    if(delete)
      destructorFWS(target);

    //se l'elemento rimosso è in testa devo cambiare anche il puntatore di lista
    if (prev != NULL)
      prev->nextNode = next;
    else
      l->firstNode = next;
    if (next != NULL)
      next->previousNode = prev;
    l->count--;
  }
}

// remove element with given PATH from fwsList, delete true if deletion of said node id necessary. Does nothing if element is not found
void fwsListRemoveElementByPath(fwsList *l, char *path,bool delete) {
  fileWithStats *target = fwsListGetElementByPath(l, path);
  if (target != NULL) {
    fileWithStats *prev = target->previousNode;
    fileWithStats *next = target->nextNode;
    if(delete)
      destructorFWS(target);
    if (prev != NULL)
      prev->nextNode = next;
    else
      l->firstNode = next;
    if (next != NULL)
      next->previousNode = prev;
    l->count--;
  }
}
// Prints the fwsList debug
void fwsListPrint(fwsList *l) {
  fileWithStats *cursor = l->firstNode;
  printf("fwsList count: %d\n",l->count);
  while (cursor != NULL) {
    fwsPrint(cursor);
    cursor = cursor->nextNode;
  }
}

#endif
/*
// main di prova per testarefileWithStats
int main(int c, char *argv[]) {
  fwsList * fwsLista =  constructorfwsListEmpty();
  char * path = "patate\0";
  append(fwsLista, constructorFWS(path,1,0,NULL,false));
  printfwsList(fwsLista);
  printf("elementi della fwsLista %d\n", fwsLista->count);

  //removeElementByPath(fwsLista,path);
  //printFileWithStats(getFWSByPath(fwsLista,path));
  //printFileWithStats(getFWSByIndex(fwsLista,0));
  //printFileWithStats(getFWSByID(fwsLista,1));
  //removeElementByID(fwsLista,1);
  removeElementByPath(fwsLista,path);
  return 0;
}*/