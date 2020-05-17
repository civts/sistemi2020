#include "../packet_codes.h"
#include "../utils.c"
#include "analyzer_list.h"
#include "file_with_stats_list.h"
#include "report_print_functions.h"
#include "report_utils.h"

#include <fcntl.h>
#ifndef bool
typedef unsigned char bool;
#define false 0
#define true 1
#endif

// Path to the named pipe
const char *PATH_TO_PIPE = "./myfifo";

// How many bytes to read every time from the pipe
const int BATCH_SIZE = 128;


// analyzer (eventually creating the fws if needed)
void gotAddFilePacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for A_NEW_FILE_INCOMPLETE_PART1 packets.
//(2nd half of a file path)
void got1stPathPartPacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for A_NEW_FILE_INCOMPLETE_PART2 packets.
//(2nd half of a file path)
void got2ndPathPartPacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for Q_NEW_DATA_CODE packets.
void gotNewDataPacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for A_DELETE_FILE_CODE packets.
void gotDeleteFilePacket(int pipe, byte *header, analyzerList *analyzers);
// This is the function that implements report buisiness logic
int report(int argc, const char *argv[]);


// analyzer (eventually creating the fws if needed)
void gotAddFilePacket(int pipe, byte *header, analyzerList *analyzers) {
  int dimDati = fromBytesToInt(header + 1);
  byte *dati = (byte *)malloc(sizeof(byte) * dimDati);
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    bool isFromFolder = dati[INT_SIZE * 2];
    char *path = dati + 2 * INT_SIZE + 1;
    //funzione che aggiungo il file all'analyzer corretto, nel caso crea un analyzer se mancante
    analyzerListAddNewFile(analyzers,pid,constructorFWS(path, idFile, 0, NULL, isFromFolder));
  } else {
    perror("aggiunta file fallita");
  }
  free(dati);
}

// Callback for A_NEW_FILE_INCOMPLETE_PART1 packets.
//(2nd half of a file path)
void got1stPathPartPacket(int pipe, byte *header, analyzerList *analyzers) {
  int dimDati = fromBytesToInt(header + 1);
  byte *dati = (byte *)malloc(sizeof(byte) * dimDati);
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    bool isFromFolder = dati[INT_SIZE * 2];
    char *path = dati + 2 * INT_SIZE + 1;
    //funzione che aggiungo il file all'analyzer corretto, nel caso crea un analyzer se mancante
    analyzerListAddIncompleteFile(analyzers,pid,constructorFWS(path, idFile, 0, NULL, isFromFolder));
  } else {
    perror("aggiunta file p1 fallita");
  }
  free(dati);
}

// Callback for A_NEW_FILE_INCOMPLETE_PART2 packets.
//(2nd half of a file path)
void got2ndPathPartPacket(int pipe, byte *header, analyzerList *analyzers) {
  // printf("code : %u\n",header[0]);
  int dimDati = fromBytesToInt(header + 1);
  // printf("dati : %u\n",dimDati);
  byte *dati = (byte *)malloc(sizeof(byte) * dimDati);
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    char *path = dati + (2 * INT_SIZE);
    //funzione che aggiorna il path del file con idFIle
    analyzerListUpdateFilePath(analyzers,pid,idFile,path);
  } else {
    perror("update del path fallito\n");
  }
  // perché qui non va avanti ?
  // printAnalyzerList(analyzers);
  // printf("bananare");
  free(dati);
}

// Callback for Q_NEW_DATA_CODE packets.
void gotNewDataPacket(int pipe, byte *header, analyzerList *analyzers) {
  int dimDati = fromBytesToInt(header + 1);
  byte *dati = (byte *)malloc(sizeof(byte) * dimDati);
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    //adesso butto via m ed i
    uint m = fromBytesToInt(dati + 2 * INT_SIZE);
    uint i = fromBytesToInt(dati + 3 * INT_SIZE);
    //dimensione totale del file
    uint dimFile = fromBytesToInt(dati + 4 * INT_SIZE);
    //caratteri letti in questa porzione
    uint totCharsRead = fromBytesToInt(dati + 5 * INT_SIZE);
    uint *occurrences = malloc(sizeof(uint)*ASCII_LENGTH);
    int j;
    for (j = 0; j < ASCII_LENGTH; j++) {
      occurrences[j] = fromBytesToInt(dati + (6+i)* INT_SIZE);
      //print("occurrences :'%c' n:%u",j,occurrences[j])
    }
    //funzione che aggiorna i dati del corrispondente file
    analyzerListUpdateFileData(analyzers,pid,idFile,dimFile,totCharsRead,occurrences);
    free(occurrences);
  } else {
    perror("updateDelFileFallito\n");
  }
  free(dati);
}

// Callback for A_DELETE_FILE_CODE packets.
void gotDeleteFilePacket(int pipe, byte *header, analyzerList *analyzers) {
  // printf("code : %u\n",header[0]);
  int dimDati = fromBytesToInt(header + 1);
  // printf("dati : %u\n",dimDati);
  byte *dati = (byte *)malloc(sizeof(byte) * dimDati);
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    uint idFile;
    pid = fromBytesToInt(dati);
    idFile = fromBytesToInt(dati + INT_SIZE);
    analyzerListDeleteFile(analyzers,pid,idFile);
  } else {
    perror("errore in eliminazione\n");
  }
  free(dati);
}

// This is the function that implements report buisiness logic
int report(int argc, const char *argv[]) {
  int retCode = 0;
  // This is where the state is stored: it contains the references to the
  // "objects" representing the files and their stats.
  analyzerList *analyzers = constructorAnalyzerListEmpty();
  int pipe = open(PATH_TO_PIPE, O_RDONLY);
  if (pipe == -1) {
    retCode = 1;
    perror("No pipe");
  }
  while (pipe != -1) {
    // Reading new packet and taking appropriate action
    byte header[INT_SIZE + 1];
    int rdHeader = read(pipe, header, INT_SIZE + 1);
    if (rdHeader == INT_SIZE + 1) {
      if (DEBUGGING)
        printf("Got new packet with code %d\n", header[0]);
      switch (header[0]) {
      case Q_NEW_DATA_CODE:
        gotNewDataPacket(pipe, header, analyzers);
        break;
      case Q_FILE_ERROR_CODE:
        // TODO (What do we do on error? print to the user and forget about it?)
        break;
      case A_NEW_FILE_COMPLETE:
        gotAddFilePacket(pipe, header, analyzers);
        break;
      case A_NEW_FILE_INCOMPLETE_PART1:
        got1stPathPartPacket(pipe, header, analyzers);
        break;
      case A_NEW_FILE_INCOMPLETE_PART2:
        got2ndPathPartPacket(pipe, header, analyzers);
        break;
      case A_DELETE_FILE_CODE:
        gotDeleteFilePacket(pipe, header, analyzers);
        break;
      }
      if (system("clear")) {
        printf("\n----------------------------------------\n");
      }
      // Print right recap info based on argv
      
      if (argc == 1) {
        printRecapCompact(analyzers);
      } else if (contains(argc, argv, verboseFlag)) {
        printRecapVerbose(analyzers, contains(argc, argv, groupFlag));
      } else {
        // Should never get here if args are valid
        perror("Arguments are not valid, please use --help for reference");
        retCode = 1;
        break;
      }
      //debug
      //analyzerListPrint(analyzers);
    }
  }
  return retCode;
}

int main(int argc, const char *argv[]) {
  int retCode = 0;
  if (contains(argc, argv, helpFlag)) {
    printf("this is the help text"); // TODO
  } else {
    if (argsAreValid(argc, argv)) {
      retCode = report(argc, argv);
    }
    // Will never have an else since when args are not valid the program
    // terminates (argsAreValid calls exit w/ appropriate error code).
  }
  return retCode;
}
