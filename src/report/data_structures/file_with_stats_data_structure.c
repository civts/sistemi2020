#include "./file_with_stats_data_structure.h"

fileWithStats *constructorFWS(char *path, uint id, uint totalCharacters,
                              uint occorrenze[ASCII_LENGTH]) {
  fileWithStats *fs = (fileWithStats *)malloc(sizeof(fileWithStats));
   checkNotNull(fs);
  fs->path = (char *)malloc(strlen(path)+1);
   checkNotNull(fs->path);
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
  fs->m = -1;
  fs->gotData=false;
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
void fwsResetData(fileWithStats * fs){
  fs->m=-1;
  int i;
  for (i = 0; i < ASCII_LENGTH; i++) {
    fs->occorrenze[i]  = 0;
  }
  fs->readCharacters=0;
  fs->gotData=false;
}
void fwsUpdateFileData(fileWithStats *fs, uint totCharsFile, uint totCharsRead,
                       uint occorrenze[ASCII_LENGTH],uint m) {
  if (DEBUGGING)
    printf("Adding new stats to FWS object of file %s\n", fs->path);
  if(m!=fs->m){
    fwsResetData(fs);
  }
  // totale dei caratteri del file
  fs->totalCharacters = totCharsFile;
  // sommo la quantità letta in questa porzione
  fs->readCharacters += totCharsRead;
  int i;
  for (i = 0; i < ASCII_LENGTH; i++) {
    fs->occorrenze[i] += occorrenze[i];
  }
  fs->m=m;
  fs->gotData = true;
}

void fwsUpdateFilePath(fileWithStats *fs, char *path) {
  char *oldPath = fs->path;
  char *tmp = (char *)malloc(strlen(oldPath) + strlen(path) + 1);
  checkNotNull(tmp);
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
