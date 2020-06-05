#include "../../utils.c"
#include "./analyzer_data_structure.h"
//#include "./file_with_stats_list.h"
#ifndef ANALYZER_LIST_H
#define ANALZYER_LIST_H
#include <stdio.h> //print etc
/*  File where we define the structure of the analyzerList with all the analyzer
 *  nodes
*/

// analyzerList of the analyzer nodes.
// It handles de-allocating nodes when they are removed.
//
// Methods:
//- constructoranalyzerListEmpty        -> Creates empty analyzerList
//- constructoranalyzerListOne          -> Creates analyzerList with one node
//(passed in params)
//- destructoranalyzerList              -> Destroys the analyzerList (also
//de-allocates nodes)
//- analyzerListisEmpty                     -> If analyzerList is empty or not
//- analyzerListAppend                      -> appends new node to the end of
//the analyzerList
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
  analyzer *firstNode;
  // how many nodes are currently in the analyzerList
  int count;
} analyzerList;

// Returns reference to new empty analyzerList
analyzerList *constructorAnalyzerListEmpty();
//  Returns reference to new analyzerList w/ just one item - TESTED✔️
analyzerList *constructoranalyzerListOne(analyzer *a);
// Deletes l and everything within, de-allocating what's needed
void destructoraAnalyzerList(analyzerList *l);
// Returns wether the analyzeranalyzerList is empty or not
bool analyzerListIsEmpty(analyzerList *l);
// appends an to the end of the list
void analyzerListAppend(analyzerList *l, analyzer *an);
// Returns reference to the analyzer in the list w/ the given pid or NULL if not
// found
analyzer *analyzerListGetElementByPid(analyzerList *l, uint pid);
// Removes the element with the specified pid (first occourrence only). Does
// nothing if element is not found. Returns true if the element was successfully removed from the list
bool analyzerListRemoveElementByPid(analyzerList *l, uint pid);
// Removes first element from the analyzerNodeanalyzerList.
void analyzerListRemoveFirst(analyzerList *l);
// Removes the last element from the analyzerList.
void analyzerListRemoveLast(analyzerList *l);

// funzioni per i packet, richiamano la corrispondente funzione cercando
// l'analyzer corretto

// adds a new file to the analyzer with given pid. Creates a new analyer if none
// is found.
void analyzerListAddNewFile(analyzerList *l, uint pid, fileWithStats *fs);

// removes any file with given pid from the list of files in current analysis, and places them into a "blacklist"
void analyzerListErrorFile(analyzerList *l, uint pid,uint idFile);

// adds a new incomplete file to the analyzer with given pid. Creates a new
// analyer if none is found.
void analyzerListAddIncompleteFile(analyzerList *l, uint pid,
                                   fileWithStats *fs);
// updatas the path of the file with id and places it into mainList from
// incompleteList if analyzerss or file does not exits, packet is discarded
void analyzerListUpdateFilePath(analyzerList *l, uint pid, uint idFile,
                                char *path);
// updates the file corresponding to the file with that ID of the analyzer with
// pid. IF there are no matches, the packet is
// discarded
void analyzerListUpdateFileData(analyzerList *l, uint pid, uint idFile,
                                uint totChars, uint readChars,
                                uint occurrences[INT_SIZE]);
// delete a file with given id of pid, If there are no matches, the packet is
// discarded
void analyzerListDeleteFile(analyzerList *l, uint pid, uint idFile);
// delete all files that are contained in that folder. Does nothing if folder does not exists
void analyzerListDeleteFolder(analyzerList *l,uint pid, char* path);
// stores the path in a buffer
void analyzerListIncompleteFolderDelete(analyzerList *l,uint pid, char* path);
// append the first part and the second path, then it proceed for removal
void analyzerListCompletionFolderDelete(analyzerList *l,uint pid, char* path);
// // add a filter to that specific name
// void analyzerListAddFilter(analyzerList * l,uint pid, char* path);
// // add remove filter
// void analyzerListRemoveFilter(analyzerList * l,uint pid, char* path);
// add an error to the log
void analyzerListAddError(analyzerList * l,uint pid, char* error);
// funzione di stampa per il debug
void analyzerListPrint(analyzerList *l);

analyzerList *constructorAnalyzerListEmpty() {
  analyzerList *l = (analyzerList *)malloc(sizeof(analyzerList));
  l->count = 0;
  l->firstNode = NULL;
  return l;
}

analyzerList *constructoranalyzerListOne(analyzer *a) {
  analyzerList *l = constructorAnalyzerListEmpty();
  analyzerListAppend(l, a);
  return l;
}

void destructoraAnalyzerList(analyzerList *l) {
  analyzer *current = l->firstNode;
  // delete every node until none is left
  while (current != NULL) {
    analyzer *nextNode = current->nextNode;
    if (nextNode != NULL) {
      destructorAnalyzer(current);
    }
    current = nextNode;
  }
  free(l);
}

bool analyzerListIsEmpty(analyzerList *l) { return l->count == 0; }

// Appends new analyzer node to end of analyzeranalyzerList - TESTED✔️
//
// ⚠️ do NOT deallocate an, the analyzerList will take care of it when
// needed
void analyzerListAppend(analyzerList *l, analyzer *an) {
  if (analyzerListIsEmpty(l)) {
    l->firstNode = an;
  } else {
    analyzer *cursor = l->firstNode;
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    cursor->nextNode = an;
    an->previousNode = cursor;
  }
  l->count++;
}

analyzer *analyzerListGetElementByPid(analyzerList *l, uint pid) {
  analyzer *current = l->firstNode;
  while (current != NULL) {
    if ((current->pid) == pid) {
      return current;
    }
    current = current->nextNode;
  }
  return NULL;
}

bool analyzerListRemoveElementByPid(analyzerList *l, uint pid) {
  bool deleted = false;
  if (DEBUGGING)
    printf("Getting element with pid %u for deletion\n", pid);
  analyzer *targetNode = analyzerListGetElementByPid(l, pid);
  if (targetNode != NULL) {
    if (DEBUGGING)
      printf("Found element with pid %u, its @%p\n", pid, targetNode);
    analyzer *prev = targetNode->previousNode;
    analyzer *next = targetNode->nextNode;
    if (prev != NULL)
      prev->nextNode = next;
    else
      l->firstNode = next;

    if (next != NULL)
      next->previousNode = prev;
    destructorAnalyzer(targetNode);
    l->count--;
    deleted=true; 
  } else {
    if (DEBUGGING)
      printf(
          "Element with pid \"%u\" is not in this analyzerList so it was not "
          "deleted\n",
          pid);
  }
  return deleted;
}

void analyzerListRemoveFirst(analyzerList *l) {
  if (!analyzerListIsEmpty(l)) {
    analyzer *newFirstNode = l->firstNode->nextNode;
    destructorAnalyzer(l->firstNode);
    l->firstNode = newFirstNode;
    l->count--;
  }
}

void analyzerListRemoveLast(analyzerList *l) {
  if (!analyzerListIsEmpty(l)) {
    analyzer *cursor = l->firstNode;
    // Get cursor to the last analyzerList node
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    // Detach from the analyzerList
    analyzer *penultimate = cursor->previousNode;
    if (penultimate != NULL)
      penultimate->nextNode = NULL;
    // Delete
    destructorAnalyzer(cursor);
    l->count--;
  }
}

void analyzerListAddNewFile(analyzerList *l, uint pid, fileWithStats *fs) {
  analyzer *a = analyzerListGetElementByPid(l, pid);
  // if no analyzer with given pid is found
  if (a == NULL) {
    a = constructorAnalyzer(pid);
    analyzerListAppend(l, a);
  }
  // function that adds the file to the mainList
  analyzerAddNewFile(a, fs);
}
void analyzerListErrorFile(analyzerList *l, uint pid,uint idFile){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  // if no analyzer with given pid is found
  if (a == NULL) {
    a = constructorAnalyzer(pid);
    analyzerListAppend(l, a);
  }
  // function that adds the file to the mainList
  analyzerErrorFile(a, pid);
}
void analyzerListAddIncompleteFile(analyzerList *l, uint pid,
                                   fileWithStats *fs) {
  analyzer *a = analyzerListGetElementByPid(l, pid);
  // if no analyzer with given pid is found
  if (a == NULL) {
    a = constructorAnalyzer(pid);
    analyzerListAppend(l, a);
  }
  // function that adds the file to the incompleteList
  analyzerAddIncompleteFile(a, fs);
}

void analyzerListUpdateFilePath(analyzerList *l, uint pid, uint idFile,
                                char *path) {
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    // funzione che esegue l'update
    analyzerUpdateFilePath(a, idFile, path);
  } else {
    // perror("analyzer non esistente\n");
  }
}

void analyzerListUpdateFileData(analyzerList *l, uint pid, uint idFile,
                                uint totChars, uint readChars,
                                uint occurrences[INT_SIZE]) {
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    // update
    analyzerUpdateFileData(a, idFile, totChars, readChars,
                          occurrences);
    }else {
    // perror("analyzer non esistente\n");
  }
}

void analyzerListDeleteFile(analyzerList *l, uint pid, uint idFile) {
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    analyzerDeleteFile(a, idFile);
  } else {
    // file is not here, we do nothing
  }
}
void analyzerListDeleteFolder(analyzerList *l,uint pid, char* path){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    analyzerDeleteFolder(a, path);
  } else {
    // perror("analyzer non esistente\n");
  }
}
void analyzerListIncompleteFolderDelete(analyzerList *l,uint pid, char* path){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    analyzerIncompleteFolderDelete(a, path);
  } else {
    // perror("analyzer non esistente\n");
  }
}
void analyzerListCompletionFolderDelete(analyzerList *l,uint pid, char* path){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    analyzerCompletionFolderDelete(a, path);
  } else {
    // perror("analyzer non esistente\n");
  }
}
// void analyzerListAddFilter(analyzerList * l,uint pid, char* path){
//   analyzer *a = analyzerListGetElementByPid(l, pid);
//   if (a != NULL) {
//     analyzerAddFilter(a, path);
//   } else {
//     // perror("analyzer non esistente\n");
//   }
// }
// void analyzerListRemoveFilter(analyzerList * l,uint pid, char* path){
//   analyzer *a = analyzerListGetElementByPid(l, pid);
//   if (a != NULL) {
//     analyzerRemoveFilter(a, path);
//   } else {
//     // perror("analyzer non esistente\n");
//   }
// }
void analyzerListAddError(analyzerList * l,uint pid, char* path){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    analyzerAddError(a, path);
  } else {
    // perror("analyzer non esistente\n");
  }
}
void analyzerListPrint(analyzerList *l) {
  analyzer *cursor = l->firstNode;
  while (cursor != NULL) {
    analyzerPrint(cursor);
    cursor = cursor->nextNode;
  }
  printf("analyzerList count: %d\n", l->count);
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
  //int x = analyzerListGetElementByPid(0);
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
