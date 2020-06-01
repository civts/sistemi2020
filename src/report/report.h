#include "../packet_codes.h"
#include "../utils.c"
#include "analyzer_list.h"
#include "file_with_stats_list.h"
#include "report_print_functions.h"
#include "report_utils.h"

#include <fcntl.h>


// Path to the named pipe
const char *PATH_TO_PIPE = "./myfifo";

// How many bytes to read every time from the pipe
const int BATCH_SIZE = 128;

// analyzer (eventually creating the fws if needed)
void gotAddFilePacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for A_NEW_FILE_INCOMPLETE_PART1 packets.
//(1st half of a file path)
void got1stPathPartPacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for A_NEW_FILE_INCOMPLETE_PART2 packets.
//(2nd half of a file path)
void got2ndPathPartPacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for Q_NEW_DATA_CODE packets.
void gotNewDataPacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for A_DELETE_FILE_CODE packets.
void gotDeleteFilePacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for A_DELETE_FOLDER_CODE
void gotDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for A_DELETE:FOLDER_INCOMPLETE_PART1 packets.
//(1st half of a file path)
void got1stPathPartDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers);
// Callback for A_NEW_FILE_INCOMPLETE_PART2 packets.
//(2nd half of a file path)
void got2ndPathPartDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers);

// This is the function that implements report buisiness logic
int report(int argc, const char *argv[]);

void gotAddFilePacket(int pipe, byte *header, analyzerList *analyzers) {
  int dimDati = fromBytesToInt(header + 1);
  byte *dati = (byte *)malloc(sizeof(byte) * dimDati);
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    char *path = dati + 2* INT_SIZE;
    //funzione che aggiungo il file all'analyzer corretto, nel caso crea un analyzer se mancante
    analyzerListAddNewFile(analyzers,pid,constructorFWS(path, idFile, 0, NULL));
  } else {
    perror("aggiunta file fallita");
  }
  free(dati);
}

void got1stPathPartPacket(int pipe, byte *header, analyzerList *analyzers) {
  int dimDati = fromBytesToInt(header + 1);
  byte *dati = (byte *)malloc(sizeof(byte) * dimDati);
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    char *path = dati + 2* INT_SIZE;
    //funzione che aggiungo il file all'analyzer corretto, nel caso crea un analyzer se mancante
    analyzerListAddIncompleteFile(analyzers,pid,constructorFWS(path, idFile, 0, NULL));
  } else {
    perror("aggiunta file p1 fallita");
  }
  free(dati);
}

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
void gotDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers) {
  // printf("code : %u\n",header[0]);
  int dimDati = fromBytesToInt(header + 1);
  // printf("dati : %u\n",dimDati);
  byte *dati = (byte *)malloc(sizeof(byte) * dimDati);
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    char* path;
    pid = fromBytesToInt(dati);
    path = dati + INT_SIZE;
    analyzerListDeleteFolder(analyzers,pid,path);
  } else {
    perror("errore in eliminazione di una cartella\n");
  }
  free(dati);
}
void got1stPathPartDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers) {
  // printf("code : %u\n",header[0]);
  int dimDati = fromBytesToInt(header + 1);
  // printf("dati : %u\n",dimDati);
  byte *dati = (byte *)malloc(sizeof(byte) * dimDati);
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    char* path;
    pid = fromBytesToInt(dati);
    path = dati + INT_SIZE;
    analyzerListIncompleteFolderDelete(analyzers,pid,path);
  } else {
    perror("errore in eliminazione di una cartella\n");
  }
  free(dati);
}
void got2ndPathPartDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers) {
  // printf("code : %u\n",header[0]);
  int dimDati = fromBytesToInt(header + 1);
  // printf("dati : %u\n",dimDati);
  byte *dati = (byte *)malloc(sizeof(byte) * dimDati);
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    char* path;
    pid = fromBytesToInt(dati);
    path = dati + INT_SIZE;
    analyzerListCompletionFolderDelete(analyzers,pid,path);
  } else {
    perror("errore in eliminazione di una cartella\n");
  }
  free(dati);
}
