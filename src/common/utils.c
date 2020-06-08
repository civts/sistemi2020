#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "utils.h"

// Check if two strings are the same TESTED✔️
// true: p1 is equal to p2; false otherwise
bool streq(const char *p1, const char *p2) { return !strcmp(p1, p2); }

// Ensures the given string is long at most maxLen. If it is longer the middle
// part is cut out and three dots are inserted instead.
// MODIFIES the original string
// Please use maxLen >= 6 (else it returns src). TESTED✔️
void trimStringToLength(char *src, int maxLen) {
  int pathLen = strlen(src);
  if (pathLen > maxLen && maxLen >= 6) {
    char tp[maxLen + 1];
    int i, split = maxLen / 2 - 3;
    for (i = 0; i < split; i++) {
      tp[i] = src[i];
    }
    for (i = split; i < split + 3; i++) {
      tp[i] = '.';
    }
    for (i = split + 3; i < maxLen; i++) {
      tp[i] = src[pathLen - (maxLen - i)];
    }
    tp[maxLen] = '\0';
    strcpy(src, tp);
  }
}

// Check if malloc has failed
void checkNotNull(void *ptr) {
  if (ptr == NULL) {
    perror("\nNon è disponibile abbastanza memoria RAM per permettere "
           "l'esecuzione di questo programma.\n");
    exit(1);
  }
}

/**
 * Controlla se la parte iniziale della stringa fileName corrisponde con 
 * la stringa folderName. Se sì ritrona true.
 */
bool isInFolder(string fileName, string folderName){
    printf("Comparing file '%s' with folder '%s'\n", fileName, folderName);
    bool ret = false;
    int folderLen = strlen(folderName);
    string toCompare = (string)malloc(folderLen+1);
    memcpy(toCompare, fileName, folderLen);
    toCompare[folderLen]='\0';
    
    string result = strstr(toCompare, folderName);
    
    if (result != NULL){
        ret = true;
    }
    return ret;
}

// Given a path to a file/folder it returns:
// -1 : if it does not exist
//  0 : if it is a file and it exists
//  1 : if it is is a folder and it exists
int inspectPath(const char *path) {
    struct stat path_stat;
    int returnCode = -1;
    if (path != NULL && stat(path, &path_stat) == 0) {
        if (S_ISREG(path_stat.st_mode)) {
            returnCode = 0;
        } else if (S_ISDIR(path_stat.st_mode)) {
            returnCode = 1;
        }
    }
    return returnCode;
}