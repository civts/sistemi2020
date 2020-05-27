#include "../utils.c"
#include "./file_with_stats_list.h"
#include "./folder_list.h"
//#include "int_list.h" per semplificare i debugging per adesso non uso una
//intList
#include <stdio.h>  //print etc
#include <stdlib.h> // malloc & free
#ifndef ANALYZER_DATA_STRUCTURE_H
#define ANALYZER_DATA_STRUCTURE_H

/*  File where we define the structure of an Analyzer and the functions that
 * enable us to work with it. The analyzer has a pid of its process, a list of
 * the files it has analyzed complete w/ their stats and a second list which we
 * use to keep track of removed files.
 *
 * We have:
 *  - a constructor
 *  - a destructor
 *  - a method for adding additional stats (that may come from other chunks of
 *    the same file)
*/

// The representation of a file and its stats
// Props:
// - id: Analyzer process pid
// - mainList: List of the files with stats relative to this analyzer
// - deletedList: ids of the files that have been removed from this analyzer
// which we
// should not include in the final stats (blacklist)
//
// Methods for this:
// constructorAnalyzer       -> constructor
// deleteAnalyzer            -> destructor
// analyzer list node
typedef struct analyzer_t {
  // The analyzer of this node
  // Analyzer process pid
  uint pid;
  // List of the files with stats relative to this analyzer
  folder_list *mainList; //main list diventa la lista doppia

  // lista di file parziali
  fwsList *incompleteList;

  struct analyzer_t *nextNode;
  struct analyzer_t *previousNode;
} analyzer;

// constructor for Analyzer
analyzer *constructorAnalyzer(uint pid);
// Destructor for Analyzer
void destructorAnalyzer(analyzer *a);

// funzioni che gestiscono i pacchetti

// function that adds new file to the mainList
void analyzerAddNewFile(analyzer *a, fileWithStats *fs);
// function that adds new file to the incompleteList
void analyzerAddIncompleteFile(analyzer *a, fileWithStats *fs);
// function that adds new file to a list. if already present, ignored
void analyzerAddFile(analyzer *a, fwsList *l, fileWithStats *fs);
// updatas the path of the file with id and places it into mainList from
// incompleteList. if analyzes does not exits, packet is discarded
void analyzerUpdateFilePath(analyzer *a, uint idFile, char *path);
// delete a file with given id of pid, IF there are no matches, the packet is
// discarded
void analyzerDeleteFile(analyzer *a, uint idFile);

// stampa debug
void analyzerPrint(analyzer *a);

// constructor for Analyzer
analyzer *constructorAnalyzer(uint pid) {
  analyzer *a = (analyzer *)malloc(sizeof(analyzer));
  a->pid = pid;
  a->mainList = constructorFwsListEmpty();
  a->incompleteList = constructorFwsListEmpty();
  a->previousNode = NULL;
  a->nextNode = NULL;
  if (DEBUGGING)
    printf("Creating a new Analyzer instance @%p for Analyzer with pid %u\n", a,
           a->pid);
  return a;
}

// Destructor for Analyzer
// TODO: seems ok but we need to test for memory leaks
void destructorAnalyzer(analyzer *a) {
  if (DEBUGGING)
    printf("Deleting Analyzer instance @%p for Analyzer with pid %u\n", a,
           a->pid);
  destructorFwsList(a->mainList);
  destructorFwsList(a->incompleteList);
  free(a);
}

void analyzerAddNewFile(analyzer *a, fileWithStats *fs) {
  analyzerAddFile(a, a->mainList, fs);
}

void analyzerAddIncompleteFile(analyzer *a, fileWithStats *fs) {
  analyzerAddFile(a, a->incompleteList, fs);
}

void analyzerAddFile(analyzer *a, fwsList *l, fileWithStats *fs) {
  //controllo non sia già presente
  fileWithStats *n = fwsListGetElementByID(a->mainList, fs->id);
  if (n == NULL) {
    // aggiungo alla lista, mainList o IncompleteList a seconda della chiamata
    fwsListAppend(l, fs);
  } else {
    destructorFWS(fs);
  }
}

void analyzerUpdateFilePath(analyzer *a, uint idFile, char *path) {
  // update the path
  fwsListUpdateFilePath(a->incompleteList, idFile, path);
  // get the FWS
  fileWithStats *updatedNode = fwsListGetElementByID(a->incompleteList, idFile);
  // remove from incompleteList
  fwsListRemoveElementByID(a->incompleteList, idFile, false);
  // add to mainList
  if (updatedNode != NULL)
    fwsListAppend(a->mainList, updatedNode);
}

void analyzerDeleteFile(analyzer *a, uint idFile) {
  //remove from mainlist
  fwsListRemoveElementByID(a->mainList, idFile, true);

}
// stampa debug
void analyzerPrint(analyzer *a) {
  printf("analyzer pid: %u\n", a->pid);
  printf("analyzer mainList:\n");
  fwsListPrint(a->mainList);
  printf("analyzer incompleteList:\n");
  fwsListPrint(a->incompleteList);  
}
#endif
