#include "../utils.c"
#include <stdio.h>  //print etc
#include <stdlib.h> // malloc & free
#include <string.h> //strlen e strcpy
#ifndef FILE_WITH_STATS_DATA_STRUCTURE_H
#define FILE_WITH_STATS_DATA_STRUCTURE_H

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
// constructorFWS       -> constructor
// addStatsToFWS        -> add new data (from the other parts of this file)
// deleteFWS            -> destructor
// FileWithStats list node
typedef struct fwsNode_t {
  // path to the file
  char *path;
  uint id;
  // how many characters are present in this file
  uint totalCharacters;
  // how characters hav been rdHeader
  uint readCharacters;
  // array where in position i we count how many chars w/ ASCII code i are in
  // the file
  uint occorrenze[ASCII_LENGTH];
  // if this file was analyzed because in a folder (for display purposes)
  bool fromFolder;
  struct fwsNode_t *nextNode;
  struct fwsNode_t *previousNode;
} fileWithStats;

// Creates a fileWithStats and returns pointer to it - TESTED
fileWithStats *constructorFWS(char *path, uint id, uint totalCharacters,
                              uint occorrenze[ASCII_LENGTH], bool fromFolder);
// Destructor for fileWithStats
void destructorFWS(fileWithStats *fs);
// Adds new stats to this fileWithStats
void fwsUpdateFileData(fileWithStats *fs, uint totChars, uint totCharsToAdd,
                       uint occorrenze[ASCII_LENGTH]);
// appends the path
void fwsUpdateFilePath(fileWithStats *fs, char *path);

fileWithStats *constructorFWS(char *path, uint id, uint totalCharacters,
                              uint occorrenze[ASCII_LENGTH], bool fromFolder) {
  fileWithStats *fs = (fileWithStats *)malloc(sizeof(fileWithStats));
  fs->path = (char *)malloc(strlen(path));
  fs->id = id;
  strcpy(fs->path, path);
  fs->totalCharacters = totalCharacters;
  fs->readCharacters = 0;
  if (occorrenze != NULL) {
    int i;
    for (i = 0; i < ASCII_LENGTH; i++) {
      fs->occorrenze[i] = occorrenze[i];
    }
  } else {
    int i;
    for (i = 0; i < ASCII_LENGTH; i++) {
      fs->occorrenze[i] = 0;
    }
  }
  fs->fromFolder = fromFolder;
  fs->previousNode = NULL;
  fs->nextNode = NULL;
  if (DEBUGGING)
    printf("Creating a new FWS instance @%p for file w/ path %s\n", fs, path);
  return fs;
}

void destructorFWS(fileWithStats *fs) {
  if (DEBUGGING)
    printf("Deleting FWS instance @%p of file w/ path %s\n", fs, fs->path);
  free(fs->path);
  free(fs);
}

void fwsUpdateFileData(fileWithStats *fs, uint totCharsFile, uint totCharsRead,
                       uint occorrenze[ASCII_LENGTH]) {
  if (DEBUGGING)
    printf("Adding new stats to FWS object of file %s\n", fs->path);
  // totale dei caratteri del file
  fs->totalCharacters = totCharsFile;
  // sommo la quantità letta in questa porzione
  fs->readCharacters += totCharsRead;
  int i;
  for (i = 0; i < ASCII_LENGTH; i++) {
    fs->occorrenze[i] += occorrenze[i];
  }
}

void fwsUpdateFilePath(fileWithStats *fs, char *path) {
  char *oldPath = fs->path;
  char *tmp = (char *)malloc(strlen(oldPath) + strlen(path) + 1);
  strcpy(tmp, oldPath);
  strcat(tmp, path);
  fs->path = tmp;
  free(oldPath);
};
// Prints the fileWithStats, just fore testing for now - TESTED
void fwsPrint(fileWithStats *fs) {
  printf("fws path: %s\n", fs->path);
  printf("fws id: %u\n", fs->id);
  printf("fws totalCharacters: %u\n", fs->totalCharacters);
  // lo tolgo solo per debug più veloce
  printf("fws readCharacters: %u\n", fs->readCharacters);

  /*
  int i;
  for(i=0;i<256;i++){
    printf("fws char:'%c' number:%u\n",i,fs->occorrenze[i]);
  }
  */
  printf("fws from folder: %u\n", fs->fromFolder);
}
#endif