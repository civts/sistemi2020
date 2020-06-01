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
  struct fwsNode_t *nextNode;
  struct fwsNode_t *previousNode;
} fileWithStats;

// Creates a fileWithStats and returns pointer to it - TESTED
fileWithStats *constructorFWS(char *path, uint id, uint totalCharacters,
                              uint occorrenze[ASCII_LENGTH]);
                          
// Destructor for fileWithStats
void destructorFWS(fileWithStats *fs);
// Adds new stats to this fileWithStats
void fwsUpdateFileData(fileWithStats *fs, uint totChars, uint totCharsToAdd,
                       uint occorrenze[ASCII_LENGTH]);
// appends the path
void fwsUpdateFilePath(fileWithStats *fs, char *path);

fileWithStats *constructorFWS(char *path, uint id, uint totalCharacters,
                              uint occorrenze[ASCII_LENGTH]) {
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
  printf("fws adr: %p\n", fs);
  printf("fws path: %s\n", fs->path);
  printf("fws id: %u\n", fs->id);
  printf("fws totalCharacters: %u\n", fs->totalCharacters);
  // lo tolgo solo per debug più veloce
  printf("fws readCharacters: %u\n", fs->readCharacters);
  printf("fws prev: %p\n", fs->previousNode);
  printf("fws next: %p\n", fs->nextNode);
  /*
  int i;
  for(i=0;i<256;i++){
    printf("fws char:'%c' number:%u\n",i,fs->occorrenze[i]);
  }
  */
}

// Returns the grouped stats for this file (used in print functions)
charGroupStats statsForFile(fileWithStats *fws) {
  const char spaceChars[] = {' ', '\t', '\r', '\n', '\f', '\v'};
  const char punctuationChars[] = {
    ',', ';', '.', ':', '-', '?', '!', '\'', '`', '"', '*', '(', ')', '_',
  };
  charGroupStats result;
  result.az = result.AZ = result.digits = result.punctuation = result.spaces =
      result.otherChars = result.totalChars = result.totalCharsRead = 0;
  short i;
  uint *oc = fws->occorrenze;
  for (i = 'a'; i <= 'z'; i++) {
    result.az += oc[i];
  }
  for (i = 'A'; i <= 'Z'; i++) {
    result.AZ += oc[i];
  }
  for (i = '0'; i <= '9'; i++) {
    result.digits += oc[i];
  }
  for (i = 0; i < 6; i++) {
    result.spaces += oc[spaceChars[i]];
  }
  for (i = 0; i < 14; i++) {
    result.punctuation += oc[punctuationChars[i]];
  }
  result.totalCharsRead += fws->readCharacters;
  result.totalChars += fws->totalCharacters;
  result.otherChars +=
      result.totalCharsRead - (result.az + result.AZ + result.digits +
                               result.spaces + result.punctuation);
  return result;
}

#endif
