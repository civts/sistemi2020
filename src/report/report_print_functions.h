#include "../packet_codes.h"
#include "../utils.c"
//#include "./file_with_stats_list.h"
//#include "./analyzer_list.h"
#include "./report_utils.h"
#include <fcntl.h>
#ifndef REPORT_PRINT_FUNCTIONS_H
#define REPORT_PRINT_FUNCTIONS_H

const char spaceChars[] = {' ', '\t', '\r', '\n', '\f', '\v'};
const char punctuationChars[] = {
    ',', ';', '.', ':', '-', '?', '!', '\'', '`', '"', '*', '(', ')', '_',
};

// Prints the line "Analyzed X files [in Y folders] [w/ Z analyzers]:\n"
void printFirstInfoLine(analyzerList *aList);
// Default print function (no additional argv). Should look ilke this:
//  Analyzed 50 files [in 2 folders] [w/ 6 analyzers]:
//  a-z: 2109
//  A-Z: 42
//  ....
//  total characters read: 16432 over 56600 (POTENZIALMENTE PROGRESSBAR)
// 50 % complete
// TESTED
void printRecapCompact(analyzerList *aList);
// Print function for the -v flag and possibly -g.
//
//---------------------------------------------------------------
// If shouldGroup is TRUE output should look like the following:
//
// Analyzed 50 files [in 2 folders] [w/ 6 analyzers]:
//      text1.txt
//      text2.txt
//      ....
//      textN.txt
//    ------------text1.txt------------
//    a-z: 2109
//    A-Z: 42
//    ....
//    total characters: 16432
//    ------------text2.txt------------
//    a-z: 912
//    ...
//---------------------------------------------------------------
// If shouldGroup is FALSE output should look like the following:
//
// Analyzed 50 files [in 2 folders] [w/ 6 analyzers]:
//      text1.txt
//      text2.txt
//      ....
//      textN.txt
//    ------------text1.txt------------
//    a: 2109
//    b: 42
//    ....
//    total characters: 16432
//    ------------text2.txt------------
//    a: 912
//    ...
void printRecapVerbose(analyzerList *aList, bool shouldGroup);
// Prints a single file (you need to specify if you want to have letters grouped
// or not). If group is false it prints occourrences of each letter, else
// clusters.
void printSingleFile(fileWithStats *f, bool group);
// PRInts a progressbar numbers from 0 to 100 ?
void printProgressBar(uint percentage);
// Prints a progress bar with the percentage of a/b*100
void printPercentage(uint a, uint b);
// Print function for when the user specifies the --only flag. Accepts the
// analyzers list + list of paths of the files and a bool to group or not.
void printSelectedFiles(analyzerList *analyzers, int pathsLen, char *paths[],
                        bool group);

void printFirstInfoLine(analyzerList *aList) {
  uint totFiles = 0;
  int totAnalyzers = 0;
  analyzer *current = aList->firstNode;
  while (current != NULL) {
    totFiles += current->mainList->count;
    current = current->nextNode;
    totAnalyzers++;
  }
  printf("Analyzed %u files", totFiles);
  if (totAnalyzers > 1) {
    printf(" with %d analyzers", totAnalyzers);
  }
  printf(":\n");
}

void printRecapCompact(analyzerList *aList) {
  printFirstInfoLine(aList);
  analyzer *current = aList->firstNode;
  uint az, AZ, digits, spaces, punctuation, otherChars, totalCharsRead,
      totalChars;
  az = AZ = digits = spaces = punctuation = otherChars = totalChars =
      totalCharsRead = 0;
  while (current != NULL) {
    fileWithStats *cursor = current->mainList->firstNode;
    while (cursor != NULL) {
      fileWithStats *fws = cursor;
      short i;
      uint *oc = fws->occorrenze;
      uint thisaz, thisAZ, thisDigits, thisSpaces, thisPunct;
      thisaz = thisAZ = thisDigits = thisSpaces = thisPunct = 0;
      for (i = 'a'; i <= 'z'; i++) {
        thisaz += oc[i];
      }
      az += thisaz;
      for (i = 'A'; i <= 'Z'; i++) {
        thisAZ += oc[i];
      }
      AZ += thisAZ;
      for (i = '0'; i <= '9'; i++) {
        thisDigits += oc[i];
      }
      digits += thisDigits;
      for (i = 0; i < 6; i++) {
        thisSpaces += oc[spaceChars[i]];
      }
      spaces += thisSpaces;
      for (i = 0; i < 14; i++) {
        thisPunct += oc[punctuationChars[i]];
      }
      punctuation += thisPunct;
      otherChars += fws->readCharacters - thisaz - thisAZ - thisDigits -
                    thisPunct - thisSpaces;
      totalCharsRead = fws->readCharacters;
      totalChars += fws->totalCharacters;
      cursor = cursor->nextNode;
    }
    current = current->nextNode;
  }
  printf("a-z: %u\nA-Z: %u\ndigits: %u\npunctuation: %u\nspace: %u\nother: "
         "%u\n\nTotal characters read: %u over %u\n",
         az, AZ, digits, punctuation, spaces, otherChars, totalCharsRead,
         totalChars);
  printPercentage(totalCharsRead, totalChars);
}

void printRecapVerbose(analyzerList *aList, bool shouldGroup) {
  printFirstInfoLine(aList);
  analyzer *current = aList->firstNode;
  // Print file paths
  while (current != NULL) {
    fileWithStats *fNode = current->mainList->firstNode;
    while (fNode != NULL) {
      printf("%s\n", fNode->path);
      fNode = fNode->nextNode;
    }
    current = current->nextNode;
  }
  current = aList->firstNode;
  // Print stats of each file
  while (current != NULL) {
    fileWithStats *fNode = current->mainList->firstNode;
    while (fNode != NULL) {
      printSingleFile(fNode, shouldGroup);
      fNode = fNode->nextNode;
    }
    current = current->nextNode;
  }
}

void printSingleFile(fileWithStats *f, bool group) {
  printf("---------------%s---------------\n", trimStringToLength(f->path, 70));
  short i;
  uint *oc = f->occorrenze;
  uint thisaz, thisAZ, thisDigits, thisSpaces, thisPunct, otherChars;
  thisaz = thisAZ = thisDigits = thisSpaces = thisPunct = 0;
  for (i = 'a'; i <= 'z'; i++) {
    if (!group) {
      printf("%c: %d\n", i, oc[i]);
    }
    thisaz += oc[i];
  }
  for (i = 'A'; i <= 'Z'; i++) {
    if (!group) {
      printf("%c: %d\n", i, oc[i]);
    }
    thisAZ += oc[i];
  }
  for (i = '0'; i <= '9'; i++) {
    if (!group) {
      printf("%c: %d\n", i, oc[i]);
    }
    thisDigits += oc[i];
  }
  for (i = 0; i < 6; i++) {
    if (!group) {
      printf("%c: %d\n", spaceChars[i], oc[spaceChars[i]]);
    }
    thisSpaces += oc[spaceChars[i]];
  }
  for (i = 0; i < 14; i++) {
    if (!group) {
      printf("%c: %d\n", punctuationChars[i], oc[punctuationChars[i]]);
    }
    thisPunct += oc[punctuationChars[i]];
  }
  uint totalCharsRead = f->readCharacters;
  uint totalChars = f->totalCharacters;
  otherChars =
      totalCharsRead - thisaz - thisAZ - thisDigits - thisPunct - thisSpaces;
  if (group) {
    printf("a-z: %u\nA-Z: %u\ndigits: %u\npunctuation: %u\nspace: %u\n", thisaz,
           thisAZ, thisDigits, thisPunct, thisSpaces);
  }
  printf("others: %d\n", otherChars);
  printf("\nTotal characters read: %u over %u\n", totalCharsRead, totalChars);
  printPercentage(totalCharsRead, totalChars);
}

void printPercentage(uint a, uint b) {
  const int barWidth = 30;
  float percentage = b == 0 ? 0 : a / (float)b;
  printf("\n[");
  int i, pos = barWidth * percentage;
  for (i = 0; i < barWidth; i++) {
    if (i < pos)
      printf("=");
    else if (i == pos && percentage != 0) {
      printf(">");
    } else
      printf(" ");
  }
  printf("] %.2f%% complete\n", percentage * 100);
}

void printSelectedFiles(analyzerList *analyzers, int pathsLen, char *paths[],
                        bool group) {
  int i;
  // for each path I search it across the analyzers and print it if found
  for (i = 0; i < pathsLen; i++) {
    bool printed = false;
    char *path = paths[i];
    analyzer *analyzer = analyzers->firstNode;
    while (analyzer != NULL && !printed) {
      fileWithStats *fws = analyzer->mainList->firstNode;
      while (fws != NULL) {
        if (streq(fws->path, path)) {
          printSingleFile(fws, group);
          printed = true;
        }
        fws = fws->nextNode;
      }
      analyzer = analyzer->nextNode;
    }
    if (!printed) {
      char *msg = "File with path ";
      strcat(msg, trimStringToLength(path, 80));
      strcat(msg, "was not found\n");
      perror(msg);
    }
  }
}

#endif