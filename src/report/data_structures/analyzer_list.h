#include "../../common/utils.h"
#include "./analyzer_data_structure.h"
#ifndef ANALYZER_LIST_H
#define ANALYZER_LIST_H
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
  bool dumps;
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

void analyzerListStart(analyzerList *l, uint pid);
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
                                uint occurrences[INT_SIZE],uint m);
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
#endif
