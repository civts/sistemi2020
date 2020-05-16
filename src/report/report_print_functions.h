#include "../packet_codes.h"
#include "../utils.c"
#include "./file_with_stats_list.h"
#include "./report_utils.h"
#include <fcntl.h>
#ifndef REPORT_PRINT_FUNCTIONS
#define REPORT_PRINT_FUNCTIONS

const char spaceChars[] = {' ', '\t', '\r', '\n', '\f', '\v'};
const char punctuationChars[] = {
    ',', ';', '.', ':', '-', '?', '!', '\'', '`', '"', '*', '(', ')', '_',
};

// Default print function (no additional argv). Should look ilke this:
//  Analyzed 50 files [in 2 folders] [w/ 6 analyzers]:
//  a-z: 2109
//  A-Z: 42
//  ....
//  total characters: 16432
void printRecapCompact(analyzerList *aList) {
  printFirstInfoLine(aList);
  analyzerNode *n = aList->firstNode;
  long unsigned az, AZ, digits, spaces, punctuation, otherChars, totalChars;
  az = AZ = digits = spaces = punctuation = otherChars = 0;
  while (n != NULL) {
    fwsNode *cursor = n->a->mainList->firstNode;
    while (cursor != NULL) {
      fileWithStats *fws = cursor->val;
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
      otherChars += fws->totalCharacters - thisaz - thisAZ - thisDigits -
                    thisPunct - thisSpaces;
      totalChars = fws->totalCharacters;
      cursor = cursor->nextNode;
    }
    n = n->nextNode;
  }
  printf(
      "a-z: %lu\nA-Z: %lu\ndigits: %lu\npunctuation: %lu\nspace: %lu\nother: "
      "%lu\n\nTotal charcters: %lu\n",
      az, AZ, digits, punctuation, spaces, otherChars, totalChars);
}

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
void printRecapVerbose(analyzerList *aList, bool shouldGroup) {
  printFirstInfoLine(aList);
  analyzerNode *n = aList->firstNode;
  // Print file paths
  while (n != NULL) {
    fwsNode *fNode = n->a->mainList->firstNode;
    while (fNode != NULL) {
      pritnf("%s\n", fNode->val->path);
      fNode = fNode->nextNode;
    }
    n = n->nextNode;
  }
  n = aList->firstNode;
  // Print stats of each file
  while (n != NULL) {
    fwsNode *fNode = n->a->mainList->firstNode;
    while (fNode != NULL) {
      printSingleFile(fNode->val, shouldGroup);
    }
    n = n->nextNode;
  }
}

// Prints the line "Analyzed X files [in Y folders] [w/ Z analyzers]:\n"
void printFirstInfoLine(analyzerList *aList) {
  long totFiles = 0;
  int totAnalyzers = 0;
  analyzerNode *n = aList->firstNode;
  while (n != NULL) {
    totFiles += n->a->mainList->count;
    n = n->nextNode;
    totAnalyzers++;
  }
  printf("Analyzed %d files", totFiles);
  if (totAnalyzers > 1) {
    pritnf(" with %d analyzers");
  }
  printf(":\n");
}

// Prints a signle file (you need to specify if you want to have letters grouped
// or
// not). If group is false it prints occourrences of each letter, else clusters.
void printSingleFile(fileWithStats *f, bool group) {
  printf("---------------%s---------------", f->path);
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
  long totalChars = f->totalCharacters;
  otherChars =
      totalChars - thisaz - thisAZ - thisDigits - thisPunct - thisSpaces;
  if (group) {
    printf("a-z: %u\nA-Z: %u\ndigits: %u\npunctuation: %u\nspace: %u\n", thisaz,
           thisAZ, thisDigits, thisPunct, thisSpaces);
  }
  printf("others: %d\n", otherChars);
  printf("\nTotal charcters: %lu\n", totalChars);
}

#endif