#include "../../utils.c"
#include "./file_with_stats_data_structure.h"
#ifndef FILE_WITH_STATS_LIST_H
#define FILE_WITH_STATS_LIST_H
/*  File where we define the structure of the fwsList with all the fileWithStats
 *  nodes
*/

// fwsList of the fileWithStats nodes.
// It handles de-allocating nodes when they are removed.
//
// Methods:
//- constructorfwsListEmpty        -> Creates empty fwsList
//- constructorfwsListOne          -> Creates fwsList with one node (passed in
//                                    params)
//- destructorfwsList              -> Destroys the fwsList (also de-allocates
//                                    nodes)
//- isEmpty                     -> If fwsList is empty or not
//- append                      -> appends new node to the end of the fwsList
//- getNodeByIndex              -> returns reference to node with given index
//- getNodeWithPath             -> returns reference to node with given path
//- removeElementByPath         -> Remove (and de-allocates) node with given
//                                 filePath
//- removeFirst                 -> Remove (and de-allocates) first node
//- removeLast                  -> Remove (and de-allocates) last node
//- updateFileData {}             -> Adds new stats to the relative fileWithStats
//                                 (if not present appends a new node)
//- removeLast                  -> Remove (and de-allocates) last node
//- printfwsList                   -> prints the fwsList
typedef struct {
  // pointer to the first node
  fileWithStats *firstNode;
  // how many nodes are currently in the fwsList
  int count;
} fwsList;

// Returns reference to new empty fwsList
fwsList *constructorFwsListEmpty();
//  Returns reference to new fwsList w/ just one item - TESTED✔️
fwsList *constructorFwsListOne(fileWithStats *fs);
// Deletes l and everything within, de-allocating what's needed
void destructorFwsList(fwsList *l);
// Returns wether the fwsList is empty or not
bool fwsListIsEmpty(fwsList *l);
// Appends new node to end of fwsList - TESTED✔️
void fwsListAppend(fwsList *l, fileWithStats *fs);
//inserts at a given position index, as in arrays from [0....n-1]
void fwsListInsertAt(fwsList *l, fileWithStats *fs, int index);
//inserts in order
void fwsListInsertOrder(fwsList *l, fileWithStats *fs);
// Returns node in the given position (starting from 0) Returns NULL if not
// found
fileWithStats *fwsListGetElementByIndex(fwsList *l, int index);
// Returns reference to file  w/ given path Returns NULL if not found
fileWithStats *fwsListGetElementByPath(fwsList *l, char *path);
// return reference to file w/ give id Returns NULL if not found
fileWithStats *fwsListGetElementByID(fwsList *fwsList, uint id);
// Returns reference to file  w/ given path Returns NULL if not found
fileWithStats *fwsListGetElementByPathContained(fwsList *l, char *path);
// Removes first element from the fileWithStatsfwsList.
void fwsListRemoveFirst(fwsList *l);
// Removes the last element from the fwsList.
void fwsListRemoveLast(fwsList *fwsList);
// Adds the stats from newData to the right file in this fwsList. If file is not
// present it is ignored.
void fwsListUpdateFileData(fwsList *l, uint id, uint charTot, uint charsToAdd,
                           uint *occourrrences,uint m);
// Adds new chars to file path. IF file with this id is not present, it is
// ignored
void fwsListUpdateFilePath(fwsList *fwsList, uint id, char *path);
// remove element with given id from fwsList, delete true if deletion of said
// node id necessary. Returns true if the element was successfully removed from the list
bool fwsListRemoveElementByID(fwsList *fwsList, uint id, bool del);
// remove element with given PATH from fwsList, delete true if deletion of said
// node id necessary. Returns true if the element was successfully removed from the list
bool fwsListRemoveElementByPath(fwsList *fwsList, char *path, bool del);
// function that retunrs true if path1 is alphabetically before path2, if not false. 
bool comparePaths(char* path1,char*path2);
// if path1 is contained in path2. IE if path 2 is a file within path1
bool pathIsContained(char* path1, char* path2);
//deletes all elements in a folder
void fwsListDeleteFolder(fwsList * l, char* path);
// returns a list of items( copy) ) that have that belong to that folder
// fwsList* fwsListGetFolder(fwsList * l, char* path);
// resets all data in the list
void fwsListResetData(fwsList * l);
void fwsListPrint(fwsList *l);
#endif