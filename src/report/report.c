#include "../packet_codes.h"
#include "../utils.c"
#include "./list_data_structure.h"
#include "./report_utils.h"
#include "./analyzer_list.h"
#include <fcntl.h>
#ifndef bool
typedef unsigned char bool;
#define false 0
#define true 1
#endif

// Path to the named pipe
const char *PATH_TO_Q = "./Q";

const char *PATH_TO_A = "./A";

// How many bytes to read every time from the pipe
const int BATCH_SIZE = 128;

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

void stampaGruppiNonVerbosa(list *list) {
  printf("Analyzed %d files:\n", list->count);
  uint az, AZ, digits, spaces, punctuation, otherChars;
  uint totalChars;
  az = AZ = digits = spaces = punctuation = otherChars = totalChars = 0;
  fwsNode *cursor = list->firstNode;
  while (cursor != NULL) {
    fileWithStats *fws = cursor->val;
    int i;
    int *oc = fws->occorrenze;
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
  printf(
      "a-z: %u\nA-Z: %u\ndigits: %u\npunctuation: %u\nother: %u\n\nTotal "
      "charcters: %u\n",
      az, AZ, digits, punctuation, otherChars, totalChars);
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
    int *oc = fws->occorrenze;
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
//funzione che gestisce l'aggiunta di nuovi file
void newFilePacket(int pipeFromA,byte *header,analyzerList * analyzers){
  //printf("code : %u\n",header[0]);
  int dimDati = fromBytesToInt(header+1);
  //printf("dati : %u\n",dimDati);
  byte *dati = malloc(sizeof(byte)*dimDati);
  int rdDati = read(pipeFromA,dati,dimDati);
  if (rdDati == dimDati){
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati+INT_SIZE);
    bool isFromFolder = dati[INT_SIZE*2];
    char* path = dati + ((2*INT_SIZE)+1);
    //printf("pid %u\nidFile %u\n",pid,idFile);
    //printf("path %s\n",path);
    analyzer* a = analyzerListGetAnalyzerByID(analyzers,pid);
    if(a==NULL){
      a = constructorAnalyzer(pid);
      analyzerListAppend(analyzers,a);
    }
    //printAnalyzerList(analyzers);
    //controllo di non aggiungere più volte lo stesso file
    if(getNodeByID(a->mainList,idFile)==NULL){
      //devo creare il file nella lista degli analyzer
      //from folder da discutere
      fileWithStats * file = constructorFWS(path,idFile,0,NULL,isFromFolder);
      //printFileWithStats(file);
      append(a->mainList,file);
    }
  }else{
    perror("errore da banane\n");
  }
  // perché qui non va avanti ?
  //printAnalyzerList(analyzers);
  //printf("bananare");
  free(dati);
}
//funzione che gestisce l'aggiunta di nuovi file
void updateFilePathPacket(int pipeFromA,byte *header,analyzerList * analyzers){
  //printf("code : %u\n",header[0]);
  int dimDati = fromBytesToInt(header+1);
  //printf("dati : %u\n",dimDati);
  byte *dati = malloc(sizeof(byte)*dimDati);
  int rdDati = read(pipeFromA,dati,dimDati);
  if (rdDati == dimDati){
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati+INT_SIZE);
    char* path = dati + (2*INT_SIZE);
    analyzer* a = analyzerListGetAnalyzerByID(analyzers,pid);
    updateFilePath(a->mainList,idFile,path);
    //printf("path %s",getFWSByID(a->mainList,idFile)->path);

  }else{
    perror("errore da banane\n");
  }
  // perché qui non va avanti ?
  //printAnalyzerList(analyzers);
  //printf("bananare");
  free(dati);
}
void deleteFilePacket(int pipeFromA,byte *header,analyzerList * analyzers){
  //printf("code : %u\n",header[0]);
  int dimDati = fromBytesToInt(header+1);
  //printf("dati : %u\n",dimDati);
  byte *dati = malloc(sizeof(byte)*dimDati);
  int rdDati = read(pipeFromA,dati,dimDati);
  if (rdDati == dimDati){
    uint pid;
    uint idFile;
    pid = fromBytesToInt(dati);
    idFile = fromBytesToInt(dati+INT_SIZE);
    //printf("pid %u\nidFile %u\n",pid,idFile);
    analyzer* a = analyzerListGetAnalyzerByID(analyzers,pid);
    if(a!=NULL){
      //printAnalyzer(a);
      removeElementByID(a->mainList,idFile);
      //printAnalyzerList(analyzers);
      //printAnalyzer(a);
      //printList(a->mainList);
      //printf("mainlista :%p \n %d",a->mainList,(a->mainList)->count);
      //printf("mainlista :%d\n",(a->mainList)->count);
    }
  }else{
    perror("errore da banane\n");
  }
  free(dati);
}
// This is the function that implements report buisiness logic
int report(int argc, const char *argv[]) {
  int retCode = 0;
  // This is where the state is stored: it contains the references to the
  // "objects" representing the files and their stats.
  analyzerList *analyzers = constructorAnalyzerListEmpty();

  int pipeFromA = open(PATH_TO_A, O_RDONLY );
  if (pipeFromA == -1) {
    retCode = 1;
    perror("No pipe A");
  }
  int pipeFromQ = open(PATH_TO_Q, O_RDONLY );
  if (pipeFromQ == -1) {
    retCode = 1;
    perror("No pipe Q");
  }
  while (1) {
    // lettura dalla pipeA sistemare in dinamica
    byte header [INT_SIZE+1] = {'\0','\0','\0','\0','\0'};
    int rdHeader = read(pipeFromA,header,INT_SIZE+1);
    if (rdHeader == INT_SIZE+1){
      switch(header[0]){
        //NUOVO FILE
        case NEW_FILE_CODE :
          printf("nuovo pacchetto\n");
          newFilePacket(pipeFromA,header,analyzers);
          break;
        // file parziale
        case NEW_FILE_CODE_P1:

          printf("nuovo pacchetto p1\n");
          newFilePacket(pipeFromA,header,analyzers);
          break;
        //file parziale
        case NEW_FILE_CODE_P2:

          printf("nuovo pacchetto p2\n");
          updateFilePathPacket(pipeFromA,header,analyzers);
          break;
        //eliminazione di file
        case DELETE_FILE_CODE:

          printf("elimina\n");
          deleteFilePacket(pipeFromA,header,analyzers);
          break;

      }

    }
    //lettura dalla pipeQ
  }
  return retCode;
}
/*
// This is the function that implements report buisiness logic
int report(int argc, const char *argv[]) {
  int retCode = 0;
  // This is where the state is stored: it contains the references to the
  // "objects" representing the files and their stats.
  list *mainList = constructorListEmpty();
  while (1) {
    // open the pipe
    int fd = open(PATH_TO_REPORT_PIPE, O_RDONLY | O_NONBLOCK);
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
      //printf("pathlength %u\n", pathLength);
      char pathToFile[pathLength + 1];
      read(fd, &pathToFile, pathLength + 1);
      pathToFile[pathLength] = '\0';

      if (DEBUGGING)
        printf("Got packet w/ status code %d for file %s\n", statusCode,
               pathToFile);
      /*
      int j=0;
      for(j=0;j<pathLength+1;j++){
        printf("%c\n",pathToFile[j]);
      }
      */
      /*
      switch (statusCode) {
      // nuovi dati per il file
      case NEW_PACKET_CODE: {
        //----Getting remaining data from the pipe

        // Wether this file was passed directly or analyzed because it was
        // inside of a folder
        // è un byte, non è un bool
        byte cameFromFolder;
        read(fd, &cameFromFolder, 1);
        //printf("folder %u",cameFromFolder);
        read(fd, buffer, INT_SIZE);
        // Total number of chars in the file
        uint totalCharsInFile = fromBytesToInt(buffer);
        //printf("totalCharacters %u \n",buffer[3]);
        //printf("totalCharacters %u \n",totalCharsInFile);
        // Array where in each position i we have the occourrences of the letter
        // w/ ASCII code i
        uint occourrences[ASCII_LENGTH];

        int i;
        for (i = 0; i < ASCII_LENGTH; i++) {
          read(fd, buffer, INT_SIZE);
          occourrences[i] = fromBytesToInt(buffer);
        }

        //----Finished getting data, now adding to mainList

        fileWithStats *newFwsData = constructorFWS(
            pathToFile, totalCharsInFile, occourrences, cameFromFolder);
        //printFileWithStats(newFwsData);
        int appended = updateFileData(mainList, pathToFile, newFwsData);
        if(!appended)
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
*/
int main(int argc, const char *argv[]) {
  int retCode = 0;
  if (contains(argc, argv, helpFlag)) {
    printf("this is the help text"); // TODO
  } else {
    // WTF ? argsAreValid mi da un bool...cosa dovrei fare?
    bool message = argsAreValid(argc, argv);
    if (message) {
      // dove è report?
      retCode = report(argc, argv);
    }
    // Will never have an else since when they are not valid  the program
    // terminates
  }
  return retCode;
}
