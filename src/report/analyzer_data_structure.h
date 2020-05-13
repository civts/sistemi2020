#include "../utils.c"
#include <stdio.h>  //print etc
#include <stdlib.h> // malloc & free
#include <string.h> //strlen e strcpy
#include "./list_data_structure.h"
#ifndef ANALYZER_DATA_STRUCTURE_H
#define ANALYZER_DATA_STRUCTURE_H

/*  File where we define the structure of a fileWithStats and the functions that
 * enable us to work with it. (The representation of a File and its stats)
 *
 * We have:
 *  - a constructor
 *  - a destructor
 *  - a method for adding additional stats (that may come from other chunks of
 *    the same file)
*/

// The representation of a file and its stats
// Props:
// -
//
// Methods for this:
// constructorAnalyzer       -> constructor
// deleteAnalyzer            -> destructor
typedef struct {
  // pid
  uint pid;
  //file correntemente in uso
  list *mainList;
  //file rimossi
  list *deletedList;
} analyzer;

// Creates a fileWithStats and returns pointer to it - TESTED
//
// -------------------------------
// TODO: ensure that **deleteAnalyzer** frees path and fs correctly
analyzer *constructorAnalyzer(uint pid) {
  analyzer *a = malloc(sizeof(analyzer));
  a->pid = pid;
  a->mainList = constructorListEmpty();
  a->deletedList = constructorListEmpty();
  if (DEBUGGING)
    printf("Creating a new Analyzer instance @%p for Analyzer with pid %u\n", a, a->pid);
  return a;
}

// Destructor for fileWithStats
// TODO: seems ok but we need to test for memory leaks
void deleteAnalyzer(analyzer *a) {
  if (DEBUGGING)
    printf("Deleting Analyzer instance @%p for Analyzer with pid %u\n", a, a->pid);
  destructorList(a->mainList);
  destructorList(a->deletedList);
  free(a);
}
void printAnalyzer(analyzer *a){
  printf("Analyzer pid : %u",a->pid);
}
/*
int main(){
  analyzer *a = constructorAnalyzer(33);
  deleteAnalyzer(a);
}*/
#endif
