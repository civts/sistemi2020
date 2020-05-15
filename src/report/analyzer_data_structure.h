#include "../utils.c"
#include "./file_with_stats_list.h"
#include "int_list.h"
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
// - deletedList: ids of the files that have been removed from this analyzer which we
// should not include in the final stats (blacklist)
//
// Methods for this:
// constructorAnalyzer       -> constructor
// deleteAnalyzer            -> destructor
typedef struct {
  // Analyzer process pid
  uint pid;
  // List of the files with stats relative to this analyzer
  list *mainList;
  // blacklist: files that have been removed from this analyzer which we should
  // not include in the final stats
  intList *deletedList;
  //lista di file parziali
  list *incompleteList;
} analyzer;

// Creates an Analyzer and returns pointer to it - TESTED
analyzer *constructorAnalyzer(uint pid) {
  analyzer *a = (analyzer *)malloc(sizeof(analyzer));
  a->pid = pid;
  a->mainList = constructorListEmpty();
  a->deletedList = constructorIntList();
  a->incompleteList = constructorListEmpty();
  if (DEBUGGING)
    printf("Creating a new Analyzer instance @%p for Analyzer with pid %u\n", a,
           a->pid);
  return a;
}

// Destructor for fileWithStats
// TODO: seems ok but we need to test for memory leaks
void deleteAnalyzer(analyzer *a) {
  if (DEBUGGING)
    printf("Deleting Analyzer instance @%p for Analyzer with pid %u\n", a,
           a->pid);
  destructorList(a->mainList);
  destructorIntList(a->deletedList);
  destructorList(a->incompleteList);
  free(a);
}

void printAnalyzer(analyzer *a) { printf("Analyzer pid : %u", a->pid); }
#endif
