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
typedef struct {
  // path to the file
  char *path;
  uint id;
  // how many characters are present in this file
  uint totalCharacters;

  // array where in position i we count how many chars w/ ASCII code i are in
  // the file
  uint occorrenze[ASCII_LENGTH];

  // if this file was analyzed because in a folder (for display purposes)
  bool fromFolder;
} fileWithStats;

// Creates a fileWithStats and returns pointer to it - TESTED
//
// -------------------------------
// TODO: ensure that **deleteFWS** frees path and fs correctly
fileWithStats *constructorFWS(char *path, uint id,uint totalCharacters,
                              uint occorrenze[ASCII_LENGTH], bool fromFolder) {
  fileWithStats *fs = malloc(sizeof(fileWithStats));
  fs->path = malloc(strlen(path));
  fs->id = id;
  strcpy(fs->path, path);
  fs->totalCharacters = totalCharacters;
  if(occorrenze !=NULL){
    int i;
    for (i = 0; i < ASCII_LENGTH; i++) {
      fs->occorrenze[i] = occorrenze[i];
    }
  }else{
    int i;
    for (i = 0; i < ASCII_LENGTH; i++) {
      fs->occorrenze[i] = 0;
    }
  }
  fs->fromFolder = fromFolder;
  if (DEBUGGING)
    printf("Creating a new FWS instance @%p for file w/ path %s\n", fs, path);
  return fs;
}

// Destructor for fileWithStats
// TODO: seems ok but we need to test for memory leaks
void deleteFWS(fileWithStats *fs) {
  if (DEBUGGING)
    printf("Deleting FWS instance @%p of file w/ path %s\n", fs, fs->path);
  free(fs->path);
  free(fs);
}

// Adds new stats to this fileWithStats
void addStatsToFWS(fileWithStats *fs, uint totCharsToAdd,
                   uint occorrenze[ASCII_LENGTH]) {
  if (DEBUGGING)
    printf("Adding new stats to FWS object of file %s\n", fs->path);
  fs->totalCharacters += totCharsToAdd;
  int i;
  for (i = 0; i < ASCII_LENGTH; i++) {
    fs->occorrenze[i] += occorrenze[i];
  }
}

// Prints the fileWithStats, just fore testing for now - TESTED
void printFileWithStats(fileWithStats *fs) {
  printf("path : %s\n", fs->path);
  printf("id %u\n",fs->id);
  printf("totlchar %u\n", fs->totalCharacters);
  // lo tolgo solo per debug più veloce
  /*
  printf("%d\n",fs->totalCharacters);
    int i;
    for(i=0;i<256;i++){
      printf("%d\n",fs->occorrenze[i]);
    }
    */
  printf("from folder %u\n", fs->fromFolder);
}
#endif
