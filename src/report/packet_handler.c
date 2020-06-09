#include "packet_handler.h"

// How many bytes to read every time from the pipe
const int BATCH_SIZE = 10;

void gotAddFilePacket(int pipe, byte *header, analyzerList *analyzers,
                      uint dimDati) {
  byte dati[dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    char *path = (char *)dati + 2 * INT_SIZE;
    // funzione che aggiungo il file all'analyzer corretto, nel caso crea un
    // analyzer se mancante
    analyzerListAddNewFile(analyzers, pid,
                           constructorFWS(path, idFile, 0, NULL));
  } else {
    perror("aggiunta file fallita");
  }
}

void gotErrorFilePacket(int pipe, byte *header, analyzerList *analyzers,
                        uint dimDati) {
  byte dati[dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    analyzerListErrorFile(analyzers, pid, idFile);
  } else {
    perror("file con errore perso");
  }
}

void got1stPathPartPacket(int pipe, byte *header, analyzerList *analyzers,
                          uint dimDati) {
  byte dati[dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    char *path = (char *)dati + 2 * INT_SIZE;
    // funzione che aggiungo il file all'analyzer corretto, nel caso crea un
    // analyzer se mancante
    analyzerListAddIncompleteFile(analyzers, pid,
                                  constructorFWS(path, idFile, 0, NULL));
  } else {
    perror("aggiunta file p1 fallita");
  }
}

void got2ndPathPartPacket(int pipe, byte *header, analyzerList *analyzers,
                          uint dimDati) {
  byte dati[dimDati];

  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    char *path = (char *)dati + (2 * INT_SIZE);
    // funzione che aggiorna il path del file con idFIle
    analyzerListUpdateFilePath(analyzers, pid, idFile, path);
  } else {
    perror("update del path fallito\n");
  }
}

void gotNewDataPacket(int pipe, byte *header, analyzerList *analyzers,
                      uint dimDati) {
  byte dati[dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    uint m = fromBytesToInt(dati + 2 * INT_SIZE);
    uint i = fromBytesToInt(dati + 3 * INT_SIZE);
    i++;
    // dimensione totale del file
    uint dimFile = fromBytesToInt(dati + 4 * INT_SIZE);
    // caratteri letti in questa porzione
    uint totCharsRead = fromBytesToInt(dati + 5 * INT_SIZE);
    uint *occurrences = malloc(sizeof(uint) * ASCII_LENGTH);
    checkNotNull(occurrences);
    int j;
    for (j = 0; j < ASCII_LENGTH; j++) {
      occurrences[j] = fromBytesToInt(dati + (6 + j) * INT_SIZE);
    }
    analyzerListUpdateFileData(analyzers, pid, idFile, dimFile, totCharsRead,
                               occurrences, m);
    free(occurrences);
  } else {
    perror("updateDelFileFallito\n");
  }
}

void gotDeleteFilePacket(int pipe, byte *header, analyzerList *analyzers,
                         uint dimDati) {
  byte dati[dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    uint idFile;
    pid = fromBytesToInt(dati);
    idFile = fromBytesToInt(dati + INT_SIZE);
    analyzerListDeleteFile(analyzers, pid, idFile);
  } else {
    perror("errore in eliminazione\n");
  }
}

void gotDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers,
                           uint dimDati) {
  byte dati[dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    char *path;
    pid = fromBytesToInt(dati);
    path = (char *)dati + INT_SIZE;
    analyzerListDeleteFolder(analyzers, pid, path);
  } else {
    perror("errore in eliminazione di una cartella\n");
  }
}

void got1stPathPartDeleteFolderPacket(int pipe, byte *header,
                                      analyzerList *analyzers, uint dimDati) {
  byte dati[dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    char *path;
    pid = fromBytesToInt(dati);
    path = (char *)dati + INT_SIZE;
    analyzerListIncompleteFolderDelete(analyzers, pid, path);
  } else {
    perror("errore in eliminazione di una cartella\n");
  }
}

void got2ndPathPartDeleteFolderPacket(int pipe, byte *header,
                                      analyzerList *analyzers, uint dimDati) {
  byte dati[dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    char *path;
    pid = fromBytesToInt(dati);
    path = (char *)dati + INT_SIZE;
    analyzerListCompletionFolderDelete(analyzers, pid, path);
  } else {
    perror("errore in eliminazione di una cartella\n");
  }
}

void gotErrorLogPacket(int pipe, byte *header, analyzerList *analyzers,
                       uint dimDati) {
  byte dati[dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    char *path;
    pid = fromBytesToInt(dati);
    path = (char *)dati + INT_SIZE;
    analyzerListAddError(analyzers, pid, path);
  } else {
    perror("errore in fase di aggiunta errore al log\n");
  }
}

void gotStartPacket(int pipe, byte *header, analyzerList *analyzers,
                    uint dimDati) {
  byte dati[dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    pid = fromBytesToInt(dati);
    analyzerListStart(analyzers, pid);
  } else {
    perror("errore in fase di start\n");
  }
}

int reportReadBatch(int pipe, analyzerList *analyzers, int batch) {
  int i = 0;
  int rd = 1;
  for (i = 0; i < batch && rd > 0; i++) {
    rd = reportReadOnePacket(pipe, analyzers);
  }
  return rd;
}

int reportReadOnePacket(int pipe, analyzerList *analyzers) {
  // Reading new packet and taking appropriate action
  byte header[INT_SIZE + 1];
  int rdHeader = read(pipe, header, INT_SIZE + 1);
  if (rdHeader == INT_SIZE + 1) {
    switch (header[0]) {
    case CODE_FILE_OCCURENCES:
      gotNewDataPacket(pipe, header, analyzers, fromBytesToInt(header + 1));
      break;
    case CODE_REPORT_ERROR_ON_FILE:
      gotErrorFilePacket(pipe, header, analyzers, fromBytesToInt(header + 1));
      break;
    case CODE_START_ANALYSIS:
      gotStartPacket(pipe, header, analyzers, fromBytesToInt(header + 1));
      break;
    case CODE_NEW_FILENAME_TO_REPORT_FULL:
      gotAddFilePacket(pipe, header, analyzers, fromBytesToInt(header + 1));
      break;
    case CODE_NEW_FILENAME_TO_REPORT_PT1:
      got1stPathPartPacket(pipe, header, analyzers, fromBytesToInt(header + 1));
      break;
    case CODE_NEW_FILENAME_TO_REPORT_PT2:
      got2ndPathPartPacket(pipe, header, analyzers, fromBytesToInt(header + 1));
      break;
    case CODE_REMOVE_FILE_BY_ID:
      gotDeleteFilePacket(pipe, header, analyzers, fromBytesToInt(header + 1));
      break;
    case CODE_DELETE_FOLDER_FROM_REPORT_FULL:
      gotDeleteFolderPacket(pipe, header, analyzers,
                            fromBytesToInt(header + 1));
      break;
    case CODE_DELETE_FOLDER_FROM_REPORT_PT1:
      got1stPathPartDeleteFolderPacket(pipe, header, analyzers,
                                       fromBytesToInt(header + 1));
      break;
    case CODE_DELETE_FOLDER_FROM_REPORT_PT2:
      got2ndPathPartDeleteFolderPacket(pipe, header, analyzers,
                                       fromBytesToInt(header + 1));
      break;
    case CODE_MESSAGE_TEXT:
      gotErrorLogPacket(pipe, header, analyzers, fromBytesToInt(header + 1));
      break;
    }
  }
  return rdHeader;
}
