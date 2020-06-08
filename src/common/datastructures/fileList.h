#ifndef __FILE_LIST_H__
#define __FILE_LIST_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "../utils.h"

typedef struct {
  char *fileName;
  int  idFile;
  int  numOfRemainingPortionsToRead;
  int  pIndex;
} FileState;

typedef struct NodeFileState {
    FileState *data;
    struct NodeFileState *next;
    struct NodeFileState *prev;
} NodeFileState;

typedef struct FileList {
    struct NodeFileState *first;
    struct NodeFileState *last;
    long long int number_of_nodes;
} FileList;


/**
 * Create a new FileState, returns the pointer
 */
FileState *constructorFileState(char *fileName, int fileId, int portions, int indexOfP);

void printFileState(FileState *state);

void deleteFileState(FileState *state);


/**
 * Create a new fileNode (given a filestate)
 * next and prev will be added at the moment of appending the node in the list
 */
NodeFileState *constructorFileNode(FileState *newFileState);

void printNode(NodeFileState *node);

void deleteNode(NodeFileState *node);


/**
 * Create a new empty fileList
 */
FileList *constructorFileNameList();

void printList(FileList *list);

void deleteList(FileList *list);

/**
 * Append a NodeFileState to the list 
 */
void appendFileState(FileList *list, struct NodeFileState *newNode);
/**
 * Returns the first NodeFileState with a file with the argument name
 * returns NULL if there's no file with that name
 */
NodeFileState *getNodeByName(FileList *list, char *nodeName);

/**
 * Returns the NodeFileState given the id of the FileState
 * returns NULL if there's no FileState with that id
 * returns the only first NodeFileState if more nodes have that id
 */
NodeFileState *getNodeById(FileList *list, int id);

/**
 * Removes a node given the name of the file.
 * If there's no node with that name it does noting.
 * If there are more nodes with that name it deletes the first one.
 */
int removeNode(FileList *list, char *name);

/**
 * Decrements the value of numOfRemainingPortionsToRead of the first file with id idFile
 * Returns the new value of numOfRemainingPortionsToRead, or -1 in the case that the 
 * file was not found
 */
int decrementRemainingPortionsById(FileList *list, int idFile);

/**
 * Returns true if numOfRemainingPortionsToRead is equal to 0 for every file,
 * false otherwise.
 */
bool isAnalisiFinita(FileList *list);

/**
 * Prende in input il nome di una cartella e due NamesList, toglie dalla prima tutti i 
 * nodi che contengono file che si trovano nella cartella e li aggiunge alla seconda.
 * Il nome della cartella deve essere in percorso assoluto.
 */
void deleteFolderFileList(string folder, FileList *previousList, FileList *deletedList);

#endif