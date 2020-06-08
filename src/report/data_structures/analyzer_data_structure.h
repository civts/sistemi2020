
#ifndef ANALYZER_DATA_STRUCTURE_H
#define ANALYZER_DATA_STRUCTURE_H
#include "./file_with_stats_list.h"
#include "../../common/datastructures/namesList.h"
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

void analyzerStart(analyzer *l);

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
                                uint occurrences[INT_SIZE],uint m);
//deletes all wifles contained within a folder from analyzer with that path
void analyzerDeleteFolder(analyzer *a, char * path);
// stores the partial path into incompletePathToDelete;
void analyzerIncompleteFolderDelete(analyzer *a, char * path);
// appends the partial path and proceed for deletion
void analyzerCompletionFolderDelete(analyzer *a, char * path);
// adds an error to the analyzer log and prints it to file if necessary
void analyzerAddError(analyzer * a, char* error);
// stampa debug
void analyzerPrint(analyzer *a);
//stampa degli errori
void analyzerPrintErrorMessages(const analyzer *a) ;
#endif
