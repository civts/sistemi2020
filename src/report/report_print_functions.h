#include "../packet_codes.h"
#include "../utils.c"
//#include "./file_with_stats_list.h"
#include "./analyzer_list.h"
#include "./report_utils.h"
#include <fcntl.h>
#ifndef REPORT_PRINT_FUNCTIONS_H
#define REPORT_PRINT_FUNCTIONS_H

const char spaceChars[] = {' ', '\t', '\r', '\n', '\f', '\v'};
const char punctuationChars[] = {
    ',', ';', '.', ':', '-', '?', '!', '\'', '`', '"', '*', '(', ')', '_',
};

// Prints the line "Analyzed X files [in Y folders] [w/ Z analyzers]:\n"
// AGGIORNATA CON LE FOLDER
void printFirstInfoLine(analyzerList *aList);
// Prints a progress bar with the percentage of a/b*100 [###    ]
void printPercentage(uint a, uint b);
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
// Print function for when the user specifies the --only flag. Accepts the
// analyzers list + list of paths of the files and a bool to group or not.
void printSelectedFiles(analyzerList *analyzers, int pathsLen, char *paths[],
                        bool group);
// Prints all the files in a given folder.
// If the user has analyzed the folder with more than one analyzer all the
// analysis are printed sequentially
void printFolder(analyzerList *analyzers, char *folderPath, bool group);

void printFirstInfoLine(analyzerList *aList) {
  uint totFiles = 0;
  uint totFolders = 0;
  int totAnalyzers = 0;
  analyzer *currentAnalyzer = aList->firstNode;
  while (currentAnalyzer != NULL) {
    totFiles += currentAnalyzer->files->count;
    currentAnalyzer = currentAnalyzer->nextNode;
  }
  printf("Analyzed %u files", totFiles);
  printf(" [in %u folders] ", totFolders);
  if (totAnalyzers > 1) {
    printf(" with %d analyzers", totAnalyzers);
  }
  printf(":\n");
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

void printRecapCompact(analyzerList *aList) {
  printFirstInfoLine(aList);
  analyzer *current = aList->firstNode;
  long az, AZ, digits, spaces, punctuation, otherChars, totalCharsRead,
      totalChars;
  az = AZ = digits = spaces = punctuation = otherChars = totalChars =
      totalCharsRead = 0;
  while (current != NULL) {
    fileWithStats *cursor = current->files->firstNode;
    charGroupStats fileStats = statsForFile(cursor);
    az += fileStats.az;
    AZ += fileStats.AZ;
    digits += fileStats.digits;
    punctuation += fileStats.punctuation;
    spaces += fileStats.spaces;
    otherChars += fileStats.otherChars;
    totalCharsRead += fileStats.totalCharsRead;
    totalChars += fileStats.totalChars;
    cursor = cursor->nextNode;
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
    fileWithStats *fNode = current->files->firstNode;
    while (fNode != NULL) {
      printf("%s\n", fNode->path);
      fNode = fNode->nextNode;
    }
    current = current->nextNode;
  }
  current = aList->firstNode;
  // Print stats of each file
  while (current != NULL) {
    fileWithStats *fNode = current->files->firstNode;
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

void printSelectedFiles(analyzerList *analyzers, int pathsLen, char *paths[],
                        bool group) {
  int i;
  // for each path I search it across the analyzers and print it if found
  for (i = 0; i < pathsLen; i++) {
    bool printed = false;
    char *path = paths[i];
    analyzer *analyzer = analyzers->firstNode;
    while (analyzer != NULL && !printed) {
      fileWithStats *fws = analyzer->files->firstNode;
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
      msg = strcat(msg, trimStringToLength(path, 80));
      msg = strcat(msg, "was not found\n");
      perror(msg);
    }
  }
}

void printFolder(analyzerList *analyzers, char *folderPath, bool group) {
  analyzer *a = analyzers->firstNode;
  bool foundFolder = false;
  while (a != NULL) {
    fileWithStats *f = a->files->firstNode;
    int i;
    bool foundInThisAnalyzer = false;
    for (i = 0; i < a->files->count; i++) {
      if (f == NULL)
        break; // Does not happen, but if it happens…
      if (strlen(folderPath) >= strlen(f->path))
        continue;
      int j;
      bool isInTheFolder = true;
      // check if file is in desired folder
      for (j = 0; j < strlen(folderPath); j++) {
        if (f->path[j] != folderPath[j]) {
          isInTheFolder = false;
          break;
        }
      }
      if (isInTheFolder) {
        foundFolder = true;
        if (!foundInThisAnalyzer) {
          foundInThisAnalyzer = true;
          printf("Files in the folder %s from the analyzer with pid %d:\n",
                 trimStringToLength(folderPath, 30), a->pid);
        }
        printSingleFile(f, group);
      }
      f = f->nextNode;
    }
    a = a->nextNode;
  }
  if (!foundFolder) {
    printf("Requested folder was not found in any known analysis. Please check "
           "the input for typos\n");
  }
};
#endif