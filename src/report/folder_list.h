#include "../utils.c"
#include "./cartella.h"
#include "./file_with_stats_data_structure.h"
#include <stdio.h> //print etc
#ifndef FOLDER_LIST_DATA_STRUCTURE_H
#define FOLDER_LIST_DATA_STRUCTURE_H
/*  File where we define the structure of the folderList with all the folder
 *  nodes
*/

// folderList of the folder nodes.
// It handles de-allocating nodes when they are removed.
//
// Methods:
//- constructorfolderListEmpty        -> Creates empty folderList
//- constructorfolderListOne          -> Creates folderList with one node
//(passed in
//                                    params)
//- destructorfolderList              -> Destroys the folderList (also
//de-allocates
//                                    nodes)
//- isEmpty                     -> If folderList is empty or not
//- append                      -> appends new node to the end of the folderList
//- getNodeByIndex              -> returns reference to node with given index
//- getNodeWithPath             -> returns reference to node with given path
//- removeElementByPath         -> Remove (and de-allocates) node with given
//                                 filePath
//- removeFirst                 -> Remove (and de-allocates) first node
//- removeLast                  -> Remove (and de-allocates) last node
//- updateFileData {}             -> Adds new stats to the relative folder
//                                 (if not present appends a new node)
//- removeLast                  -> Remove (and de-allocates) last node
//- printfolderList                   -> prints the folderList
typedef struct {
  // pointer to the first node
  folder *firstNode;
  // how many nodes are currently in the fwsfolderList
  int count;
} folderList;

// Returns reference to new empty folderList
folderList *constructorFolderListEmpty();
//  Returns reference to new folderList w/ just one item - TESTED✔️
folderList *constructorFolderListOne(folder *fs);
// Deletes l and everything within, de-allocating what's needed
void destructorFolderList(folderList *l);
// Returns wether the folderList is empty or not
bool folderListIsEmpty(folderList *l);
// Appends new node to end of folderList - TESTED✔️
void folderListAppend(folderList *l, folder *fs);
// Returns node in the given position (starting from 0) Returns NULL if not
// found
folder *folderListGetElementByIndex(folderList *l, int index);
// Returns reference to file  w/ given path Returns NULL if not found
folder *folderListGetElementByName(folderList *l, char *path);
// return reference to file w/ give id Returns NULL if not found
folder *folderListGetElementByID(folderList *folderList, uint id);
// Removes first element from the folderfolderList.
void folderListRemoveFirst(folderList *l);
// Removes the last element from the folderList.
void folderListRemoveLast(folderList *folderList);
// Adds the stats from newData to the right file in this folderList. If file is
// not
// present it is ignored.
void folderListUpdateFileData(folderList *l, uint id, uint charTot,
                              uint charsToAdd, uint *occourrrences);
// Adds new chars to file path. IF file with this id is not present, it is
// ignored
void folderListUpdateFilePath(folderList *folderList, uint id, char *path);
// remove element with given id from folderList, delete true if deletion of said
// node id necessary. Does nothing if element is not found
void folderListRemoveElementByID(folderList *folderList, uint id, bool delete);
// remove element with given PATH from folderList, delete true if deletion of
// said
// node id necessary. Does nothing if element is not found
void folderListRemoveElementByPath(folderList *folderList, char *path,
                                   bool delete);

void folderListPrint(folderList *l);

folderList *constructorFolderListEmpty() {
  folderList *l = (folderList *)malloc(sizeof(folderList));
  l->count = 0;
  l->firstNode = NULL;
}

folderList *constructorFolderListOne(folder *fs) {
  folderList *l = constructorFolderListEmpty();
  folderListAppend(l, fs);
  return l;
}

void destructorFolderList(folderList *l) {
  folder *current = l->firstNode;
  // delete every node until none is left
  while (current != NULL) {
    folder *nextNode = current->nextNode;
    destructorFolder(current);
    current = nextNode;
  }
  free(l);
}

bool folderListIsEmpty(folderList *l) { return l->count == 0; }

void folderListAppend(folderList *l, folder *fs) {
  if (folderListIsEmpty(l)) {
    l->firstNode = fs;
  } else {
    folder *cursor = l->firstNode;
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    cursor->nextNode = fs;
    fs->previousNode = cursor;
  }
  l->count++;
}

folder *folderListGetElementByIndex(folderList *l, int index) {
  folder *result = NULL;
  if (index >= 0 && index < l->count) {
    result = l->firstNode;
    int i;
    for (i = 0; i < index; i++) {
      result = result->nextNode;
    }
  }
  return result;
}

folder *folderListGetElementByName(folderList *l, char *name) {
  folder *current = l->firstNode;
  while (current != NULL) {
    if (streq(current->name, name)) {
      return current;
    }
    current = current->nextNode;
  }
  // not found
  return NULL;
}
/*
folder *folderListGetElementByID(folderList *l, uint id) {
  folder *current = l->firstNode;
  while (current != NULL) {
    if (current->id == id) {
      return current;
    }
    current = current->nextNode;
  }
  // not found
  return NULL;
}
*/
void folderListRemoveFirst(folderList *l) {
  if (!folderListIsEmpty(l)) {
    folder *newFirstNode = l->firstNode->nextNode;
    destructorFWS(l->firstNode);
    l->firstNode = newFirstNode;
    l->count--;
  }
}

void folderListRemoveLast(folderList *l) {
  if (!folderListIsEmpty(l)) {
    folder *cursor = l->firstNode;
    // Get cursor to the last folderList node
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    // Detach from the folderList
    folder *penultimate = cursor->previousNode;
    if (penultimate != NULL)
      penultimate->nextNode = NULL;
    // Delete
    destructorFWS(cursor);
    l->count--;
  }
}

// void folderListUpdateFileData(folderList *l, uint id, uint charTot, uint
// charsRead,
//                            uint *occourrrences) {
//   folder *target = folderListGetElementByID(l, id);
//   if (target != NULL) {
//     // if already exists
//     fwsUpdateFileData(target, charTot, charsRead, occourrrences);
//   }
//   /*
//   else{
//     // if does not exists
//     folder *newNode =
//   constructorFWS(path,id,charTot,occourrrences,fromFolder);
//     folderListAppend(l,newNode);
//   }
//   */
// }

// void folderListUpdateFilePath(folderList *l, uint id, char *path) {
//   folder *target = folderListGetElementByID(l, id);
//   if (target != NULL) {
//     fwsUpdateFilePath(target, path);
//   }
// }

// void folderListRemoveElementByID(folderList *l, uint id, bool delete) {
//   folder *target = folderListGetElementByID(l, id);
//   if (target != NULL) {
//     folder *prev = target->previousNode;
//     folder *next = target->nextNode;
//     if (delete)
//       destructorFWS(target);

//     // se l'elemento rimosso è in testa devo cambiare anche il puntatore di
//     // lista
//     if (prev != NULL)
//       prev->nextNode = next;
//     else
//       l->firstNode = next;
//     if (next != NULL)
//       next->previousNode = prev;
//     l->count--;
//   }
// }
//

void folderListRemoveElementByName(folderList *l, char *name, bool delete) {
  folder *target = folderListGetElementByName(l, name);
  if (target != NULL) {
    folder *prev = target->previousNode;
    folder *next = target->nextNode;
    if (delete)
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
// Prints the folderList debug
void folderListPrint(folderList *l) {
  folder *cursor = l->firstNode;
  printf("folderList count: %d\n", l->count);
  while (cursor != NULL) {
    folderPrint(cursor);
    cursor = cursor->nextNode;
  }
}

#endif
/*
// main di prova per testarefolder
int main(int c, char *argv[]) {
  folderList * folderLista =  constructorfolderListEmpty();
  char * path = "patate\0";
  append(folderLista, constructorFWS(path,1,0,NULL,false));
  printfolderList(folderLista);
  printf("elementi della folderLista %d\n", folderLista->count);

  //removeElementByPath(folderLista,path);
  //printfolder(getFWSByPath(folderLista,path));
  //printfolder(getFWSByIndex(folderLista,0));
  //printfolder(getFWSByID(folderLista,1));
  //removeElementByID(folderLista,1);
  removeElementByPath(folderLista,path);
  return 0;
}*/
