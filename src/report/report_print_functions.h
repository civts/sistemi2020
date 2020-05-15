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

void printAnalyzers(analyzerList *aList) {
  long totFiles = 0;
  int analyzers = 0;
  analyzerNode *n = aList->firstNode;
  while (n != NULL) {
    fwsNode *fws = n->a->mainList->firstNode;
    while (fws != NULL) {
      totFiles++;
      fws = fws->nextNode;
    }
    n = n->nextNode;
    analyzers++;
  }
  printf("Analyzed %d files with %d analyzers", totFiles, analyzers);
  //TODO continue printing stats for each analyzer
}

void stampaGruppi(uint dati[], uint caratteriTot) {
  char c;
  int i = 0;
  uint az = 0;
  for (i = 'a'; i <= 'z'; i++) {
    az += dati[i];
  }
  uint AZ = 0;
  for (i = 'A'; i <= 'Z'; i++) {
    AZ += dati[i];
  }

  for (i = 0; i < 256; i++) {
    printf("%c: %u\n", c, dati[i]);
  }
}

// Prints the stats of ALL the files (not each one) grouped by the categories
// (a-z,A-Z etc)
void stampaGruppiNonVerbosa(list *list) {
  printf("Analyzed %d files:\n", list->count);
  uint az, AZ, digits, spaces, punctuation, otherChars;
  uint totalChars;
  az = AZ = digits = spaces = punctuation = otherChars = totalChars = 0;
  fwsNode *cursor = list->firstNode;
  while (cursor != NULL) {
    fileWithStats *fws = cursor->val;
    int i;
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
    totalChars += fws->totalCharacters;
    cursor = cursor->nextNode;
  }
  printf("a-z: %u\nA-Z: %u\ndigits: %u\npunctuation: %u\nspace: %u\nother: "
         "%u\n\nTotal "
         "charcters: %u\n",
         az, AZ, digits, punctuation, spaces, otherChars, totalChars);
}

void stampaDefault(list *list) {
  printf("Analyzed %d files:\n", list->count);
  int i;
  uint az, AZ, digits, spaces, punctuation, otherChars;
  uint totalChars;
  uint occCount[ASCII_LENGTH];
  fwsNode *cursor = list->firstNode;
  while (cursor != NULL) {
    fileWithStats *fws = cursor->val;
    uint *oc = fws->occorrenze;
    for (i = 0; i < ASCII_LENGTH; i++) {
      occCount[i] += oc[i];
    }
    totalChars += fws->totalCharacters;
    cursor = cursor->nextNode;
  }
  for (i = 0; i < ASCII_LENGTH; i++) {
    if (i >= '!' && i < 254 && i != 127) {
      printf("%c", i);
    } else {
      printf("character with extendedASCII code %d", i);
    }
    printf(": %u\n", occCount[i]);
  }
  printf("\nTotal "
         "charcters: %u\n",
         totalChars);
}

// Stampa su stdout le statistiche di un file
void stampaSingoloFile(char *nomeFile, int dati[], int caratteriTot, int argc,
                       char *argv) {
  printf("---------------%s---------------", nomeFile);

  printf("Caratteri totali: %u\n", caratteriTot);
}

#endif