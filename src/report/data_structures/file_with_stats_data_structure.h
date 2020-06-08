#include "../../utils.c"
#ifndef FILE_DATA_STRUCTURE_H
#define FILE_DATA_STRUCTURE_H
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
  // number of parts
  uint m;
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
                       uint occorrenze[ASCII_LENGTH],uint m);
// appends the path
void fwsUpdateFilePath(fileWithStats *fs, char *path);
// resets
void fwsResetData(fileWithStats * fs);

// Prints the fileWithStats, just fore testing for now - TESTED
void fwsPrint(fileWithStats *fs);

// counts  by group
charGroupStats statsForFile(fileWithStats *fws);

#endif
