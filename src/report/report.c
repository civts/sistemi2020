#include "../packet_codes.h"
#include "../utils.c"
#include "./list_data_structure.h"
#include "./report_utils.h"
#include <fcntl.h>
#ifndef bool
typedef unsigned char bool;
#define false 0
#define true 1
#endif

// Path to the named pipe
const char *PATH_TO_REPORT_PIPE = "/path/to/pipe";
// How many bytes to read every time from the pipe
const int BATCH_SIZE = 128;

void stampaGruppi(int dati[], int caratteriTot) {
  char c;
  int i = 0;
  int az = 0;
  for (i = 'a'; i <= 'z'; i++) {
    az += dati[i];
  }
  int AZ = 0;
  for (i = 'A'; i <= 'Z'; i++) {
    AZ += dati[i];
  }

  for (i = 0; i < 256; i++) {
    printf("%c: %d\n", c, dati[i]);
  }
}

void stampaGruppiNonVerbosa(list *list) {
  printf("Analyzed %d files:\n", list->count);
  long az, AZ, digits, spaces, punctuation, otherChars;
  long long totalChars;
  az = AZ = digits = spaces = punctuation = otherChars = totalChars = 0;
  fwsNode *cursor = list->firstNode;
  while (cursor != NULL) {
    fileWithStats *fws = cursor->val;
    int i;
    int *oc = fws->occorrenze;
    long thisaz, thisAZ, thisDigits, thisSpaces, thisPunct;
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
    const char spaceChars[] = {' ', '\t', '\r', '\n', '\f', '\v'};
    for (i = 0; i < 6; i++) {
      thisSpaces += oc[i];
    }
    spaces += thisSpaces;
    const char punctuationChars[] = {
        ',', ';', '.', ':', '-', '?', '!', '\'', '`', '"', '*', '(', ')', '_',
    };
    for (i = 0; i < 14; i++) {
      thisPunct += oc[punctuationChars[i]];
    }
    punctuation += thisPunct;
    otherChars += fws->totalCharacters - thisaz - thisAZ - thisDigits -
                  thisPunct - thisSpaces;
    totalChars += fws->totalCharacters;
    cursor = cursor->nextNode;
  }
  printf("a-z: %d\nA-Z: %d\ndigits: %d\npunctuation: %d\nother: %d\n\nTotal "
         "charcters: %d\n",
         az, AZ, digits, punctuation, otherChars, totalChars);
}

void stampaDefault(list *list) {
  printf("Analyzed %d files:\n", list->count);
  int i;
  long az, AZ, digits, spaces, punctuation, otherChars;
  long long totalChars;
  long occCount[ASCII_LENGTH];
  fwsNode *cursor = list->firstNode;
  while (cursor != NULL) {
    fileWithStats *fws = cursor->val;
    int *oc = fws->occorrenze;
    for (i = 0; i < ASCII_LENGTH; i++) {
      occCount[i] += oc[i];
    }
    totalChars += fws->totalCharacters;
    cursor = cursor->nextNode;
  }
  for (i = 0; i < ASCII_LENGTH; i++) {
    if (i >= '!' && i < 254 && i != 127) {
      printf('%c', i);
    } else {
      printf("character with extendedASCII code %d", i);
    }
    printf(": %d\n");
  }
  printf("\nTotal "
         "charcters: %d\n",
         totalChars);
}

// Stampa su stdout le statistiche di un file
void stampaSingoloFile(char *nomeFile, int dati[], int caratteriTot, int argc,
                       char *argv) {
  printf("---------------%s---------------", nomeFile);

  printf("Caratteri totali: %d\n", caratteriTot);
}


// This is the function that implements report buisiness logic
int report(int argc,const char *argv[]) {
  int retCode = 0;
  // This is where the state is stored: it contains the references to the
  // "objects" representing the files and their stats.
  list *mainList = constructorListEmpty();
  while (1) {
    // open the pipe
    int fd = open(PATH_TO_REPORT_PIPE, O_RDONLY);
    if (fd == -1) {
      retCode = 1;
      perror("No pipe");
      break;
    } else {
      // get statusCode
      byte statusCode;
      read(fd, &statusCode, 1); // status code is in firts byte
      if (statusCode == EOF) {
        // If there is no one on the writing end of the pipe previous read will
        // retur EOF (see
        // https://www.geeksforgeeks.org/non-blocking-io-with-pipes-in-c/)
        // TODO handle (no one is transmitting)
      }

      // Getting variables which I'll need anyway (path to the file and its
      // length)
      byte buffer[4];
      read(fd, buffer, INT_SIZE);
      // path length (in bytes)
      uint pathLength = fromBytesToInt(buffer);
      char pathToFile[pathLength + 1];
      read(fd, &pathToFile, pathLength + 1);
      pathToFile[pathLength] = '\0';

      if (DEBUGGING)
        printf("Got packet w/ status code %d for file %s\n", statusCode,
               pathToFile);

      switch (statusCode) {
      // nuovi dati per il file
      case NEW_PACKET_CODE: {
        //----Getting remaining data from the pipe

        // Wether this file was passed directly or analyzed because it was
        // inside of a folder
        bool cameFromFolder;
        read(fd, &cameFromFolder, 1);

        read(fd, buffer, INT_SIZE);
        // Total number of chars in the file
        uint totalCharsInFile = fromBytesToInt(buffer);

        // Array where in each position i we have the occourrences of the letter
        // w/ ASCII code i
        int occourrences[ASCII_LENGTH];
        int i;
        for (i = 0; i < ASCII_LENGTH; i++) {
          read(fd, buffer, INT_SIZE);
          occourrences[i] = fromBytesToInt(buffer);
        }

        //----Finished getting data, now adding to mainList

        fileWithStats *newFwsData = constructorFWS(
            pathToFile, totalCharsInFile, occourrences, cameFromFolder);
        updateFileData(mainList, pathToFile, newFwsData);
        free(newFwsData);
        break;
      }
      // rimuovi questo file
      case DELETE_FILE_CODE: {
        removeElementByPath(mainList, pathToFile);
        break;
      }
      default: {
        printf("Huston, we have a problem: status code (packet code) %d is "
               "unknown",
               statusCode);
      }
      }
      stampaGruppiNonVerbosa(mainList);
    }
  }
  destructorList(mainList);
  return retCode;
}

int main(int argc,const char *argv[]) {
  int retCode = 0;
  if (contains(argc, argv, helpFlag)) {
    printf("this is the help text"); // TODO
  } else {
    // WTF ? argsAreValid mi da un bool...cosa dovrei fare?
    bool message = argsAreValid(argc, argv);
    if (message ) {
      //dove è report? 
      retCode = report(argc, argv);
    } else {
      retCode = 1;
      perror(message);
      free(message);
    }
  }
  return retCode;
}
