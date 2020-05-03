#include "../utils.c"

#include <stdio.h>  //print etc
#include <stdlib.h> // malloc & free
#include <string.h> //strlen e strcpy

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

  // how many characters are present in this file
  int totalCharacters;

  // array where in position i we count how many chars w/ ASCII code i are in
  // the file
  int occorrenze[ASCII_LENGTH];

  // if this file was analyzed because in a folder (for display purposes)
  bool fromFolder;
} fileWithStats;

// Creates a fileWithStats and returns pointer to it - TESTED
//
// -------------------------------
// TODO: ensure that **deleteFWS** frees path and fs correctly
fileWithStats *constructorFWS(char *path, int totalCharacters,
                              int occorrenze[ASCII_LENGTH], bool fromFolder) {
  fileWithStats *fs = malloc(sizeof(fileWithStats));
  fs->path = malloc(strlen(path));
  strcpy(fs->path, path);
  fs->totalCharacters = totalCharacters;
  int i;
  for (i = 0; i < ASCII_LENGTH; i++) {
    fs->occorrenze[i] = occorrenze[i];
  }
  fs->fromFolder = fromFolder;
  if (DEBUGGING)
    printf("Creating a new FWS instance @%d for file w/ path %s\n", fs, path);
  return fs;
}

// Destructor for fileWithStats
// TODO: seems ok but we need to test for memory leaks
void deleteFWS(fileWithStats *fs) {
  if (DEBUGGING)
    printf("Deleting FWS instance @%d of file w/ path %s\n", fs, fs->path);
  free(fs->path);
  free(fs);
}

// Adds new stats to this fileWithStats
void addStatsToFWS(fileWithStats *fs, int totCharsToAdd,
                   int occorrenze[ASCII_LENGTH]) {
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
  printf("%s\n", fs->path);
  // lo tolgo solo per debug più veloce
  /*
  printf("%d\n",fs->totalCharacters);
    int i;
    for(i=0;i<256;i++){
      printf("%d\n",fs->occorrenze[i]);
    }
    */
  printf("%d\n", fs->fromFolder);
}
