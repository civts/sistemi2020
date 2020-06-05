#include "../../utils.c"
#include "./file_with_stats_list.h"
#include "./namesList.h"
//#include "int_list.h" per semplificare i debugging per adesso non uso una
//intList
#include <stdio.h>  //print etc
#include <stdlib.h> // malloc & free
#include <time.h>
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
  // Wether this analyzer dumps errors to file or not
  bool dumps;
  // Dump file descriptor
  FILE* dumpFD;
  // List of the files that were analyzed individually
  fwsList *files;
  // lista di file parziali
  fwsList *incompleteList;
  //buffer of paths for folder deletion
  char * incompletePathToDelete;
  // files with errors
  fwsList *errors;
  // need a list of strings for error messages
  NamesList* errorMessages;
  //path to filter
  NamesList* filters; 
  struct analyzer_t *nextNode;
  struct analyzer_t *previousNode;
} analyzer;

// constructor for Analyzer
analyzer *constructorAnalyzer(uint pid, bool dumps);
// Destructor for Analyzer
void destructorAnalyzer(analyzer *a);

// funzioni che gestiscono i pacchetti

// function that adds new file to the mainList. If already present, it is ignored. It automatically manages the tree
void analyzerAddNewFile(analyzer *a, fileWithStats *fs);
//function that blacklist a file
void analyzerErrorFile(analyzer *a,uint id);
// function that adds new file to the incompleteList. If already present, it is ignored
void analyzerAddIncompleteFile(analyzer *a, fileWithStats *fs);
// updatas the path of the file with id and places it into mainList from
// incompleteList. if analyzes does not exits, packet is discarded
void analyzerUpdateFilePath(analyzer *a, uint idFile, char *path);
// delete a file with given id of pid, IF there are no matches, the packet is
// discarded
void analyzerDeleteFile(analyzer *a, uint idFile);
// update file with new data. If no file with given id is found, nothing is done
void analyzerUpdateFileData(analyzer *a, uint idFile,
                                uint totChars, uint readChars,
                                uint occurrences[INT_SIZE]);
//deletes all wifles contained within a folder from analyzer with that path
void analyzerDeleteFolder(analyzer *a, char * path);
// stores the partial path into incompletePathToDelete;
void analyzerIncompleteFolderDelete(analyzer *a, char * path);
// appends the partial path and proceed for deletion
void analyzerCompletionFolderDelete(analyzer *a, char * path);
// // add a filter to that specific name
// void analyzerAddFilter(analyzer * a, char* path);
// // add remove filter
// void analyzerRemoveFilter(analyzer * a, char* path);
// adds an error to the analyzer log and prints it to file if necessary
void analyzerAddError(analyzer * a, char* error);
//Prints the last 3 errors of this analyzer (or less if there are not enough)
void analyzerPrintErrorMessages(const analyzer *a);
// stampa debug
void analyzerPrint(analyzer *a);

// constructor for Analyzer
analyzer *constructorAnalyzer(uint pid, bool dumps) {
  analyzer *a = (analyzer *)malloc(sizeof(analyzer));
  checkNotNull(a);
  a->pid = pid;
  a->dumps = dumps;
  a->dumpFD = NULL;
  a->files = constructorFwsListEmpty();
  a->incompleteList = constructorFwsListEmpty();
  a->incompletePathToDelete = NULL;
  a->errors = constructorFwsListEmpty();
  a->errorMessages = constructorNamesList();
  a->filters = constructorNamesList();
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
  destructorFwsList(a->files);
  destructorFwsList(a->incompleteList);
  if(a->incompletePathToDelete!=NULL)
    free (a->incompletePathToDelete);
  destructorFwsList(a->errors);
  deleteNamesList(a->errorMessages);
  deleteNamesList(a->filters);
  fflush(a->dumpFD);
  close(a->dumpFD);
  free(a);
}

void analyzerAddNewFile(analyzer *a, fileWithStats *fs) {
  if(DEBUGGING) {printf("ADD file calle for %s\n", fs->path);
  fwsPrint(fs);}
  //controllo non sia già presente
  fileWithStats *isInFiles = fwsListGetElementByID(a->files, fs->id);
  fileWithStats *isInErrors = fwsListGetElementByID(a->errors, fs->id);
  
  if (isInFiles == NULL && isInErrors==NULL) {
    fwsListAppend(a->files,fs);
  } else {
    destructorFWS(fs);
  }
}
void analyzerErrorFile(analyzer *a,uint id){
  // controllo non sia già  presente
  fileWithStats *isInFiles = fwsListGetElementByID(a->errors,id);
  if (isInFiles == NULL) {
    isInFiles = fwsListGetElementByID(a->files,id);
    if(isInFiles != NULL){
      fwsListRemoveElementByID(a->files,id,false);
      fwsListAppend(a->errors,isInFiles);
    }
  } 
}
void analyzerAddIncompleteFile(analyzer *a, fileWithStats *fs) {
  //controllo non sia già presente
  fileWithStats *n = fwsListGetElementByID(a->incompleteList, fs->id);
  fileWithStats *isInErrors = fwsListGetElementByID(a->errors, fs->id);
  if (n == NULL && isInErrors==NULL) {
    fwsListAppend(a->incompleteList, fs);
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
  // add to files/folders
  if (updatedNode != NULL){
    analyzerAddNewFile(a,updatedNode);
  }
}

void analyzerDeleteFile(analyzer *a, uint idFile) {
  //remove from mainlist
  bool removed = fwsListRemoveElementByID(a->files,idFile,true);
  if(!removed)
    removed = fwsListRemoveElementByID(a->errors,idFile,true);
}

void analyzerUpdateFileData(analyzer *a, uint idFile,
                                uint totChars, uint readChars,
                                uint occurrences[INT_SIZE]) {
  fileWithStats * searched = fwsListGetElementByID(a->files,idFile);
  if(searched!=NULL){
    fwsUpdateFileData(searched,totChars,readChars,occurrences);
  }
}

void analyzerDeleteFolder(analyzer *a, char * path){
  fwsListDeleteFolder(a->files,path);
}

void analyzerIncompleteFolderDelete(analyzer *a, char * path){
  a->incompletePathToDelete = malloc(sizeof(char)*strlen(path)+1);
  checkNotNull(a->incompletePathToDelete);
  strcpy(a->incompletePathToDelete,path);
}

void analyzerCompletionFolderDelete(analyzer *a, char * path){
  char *oldPath = a->incompletePathToDelete;
  char *completePath = (char *)malloc(strlen(oldPath) + strlen(path) + 1);
  checkNotNull(completePath);
  strcpy(completePath, oldPath);
  strcat(completePath, path);
  analyzerDeleteFolder(a,completePath);
  free(oldPath);
  free(completePath);
  a->incompletePathToDelete = NULL;
}

// void analyzerAddFilter(analyzer * a, char* path){
//   appendToNamesList(a->filters,path);
// }

// void analyzerRemoveFilter(analyzer * a, char* path){
//   removeNodeNameByName(a->filters,path);
// }

void analyzerAddError(analyzer *a, char *error) {
  appendToNamesList(a->errorMessages, constructorNodeName(error));
  if (a->dumps) {
    if (a->dumpFD == NULL) {
      char name[100];
      sprintf(name, "./analyzer_");
      sprintf(name + strlen(name), "%d", a->pid);
      sprintf(name + strlen(name), "_dump.txt");
      a->dumpFD = fopen(name, "w");
    }
    // Various reasons could cause us to not be able to open the file now.
    // in this case we will not print this error and try to open again the
    // next time an error arrives
    if (a->dumpFD != NULL) {
      time_t t = time(NULL);
      struct tm tm = *localtime(&t);
      fprintf(a->dumpFD, "%d-%02d-%02d %02d:%02d:%02d\t", tm.tm_year + 1900,
              tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
      fprintf(a->dumpFD, "%s", error);
      char lastChar = error[strlen(error) - 1];
      if (lastChar != '\n' && lastChar != '\r' && lastChar != '\f')
        fprintf(a->dumpFD, '\n');
    }
  }
}

void analyzerPrintErrorMessages(const analyzer *a) {
  NodeName *n = a->errorMessages->last;
  if(n==NULL) return;
  int i;
  // go back 2 steps
  for (i = 0; i < 2; i++) {
    if (n->prev == NULL)
      break;
    n = n->prev;
  }
  // forward while printing
  for (i = 0; i < 3; i++) {
    if (n == NULL)
      break;
    printf("%s", n->name);
    // if message does not end with \n we add it
    if (n->name[strlen(n->name - 1)] != '\n')
      printf('\n');

    n = n->next;
  }
}

// stampa debug
void analyzerPrint(analyzer *a) {
  printf("analyzer pid: %u\n", a->pid);
  printf("analyzer filesList:\n");
  fwsListPrint(a->files);
  printf("analyzer incompleteList:\n");
  fwsListPrint(a->incompleteList);  
}
#endif
