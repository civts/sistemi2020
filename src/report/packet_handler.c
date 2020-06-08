#include "packet_handler.h"
#include "../packet_codes.h"
void gotAddFilePacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati) {
  byte dati [dimDati];
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
  
}

void gotErrorFilePacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati) {
  byte dati [dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    analyzerListErrorFile(analyzers,pid,idFile);
  } else {
    perror("file con errore perso");
  }
}
void got1stPathPartPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati) {
  byte dati [dimDati];
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

}

void got2ndPathPartPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati) {
  byte dati [dimDati];

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

}

void gotNewDataPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati) {
  byte dati [dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid = fromBytesToInt(dati);
    uint idFile = fromBytesToInt(dati + INT_SIZE);
    //adesso butto i
    uint m = fromBytesToInt(dati + 2 * INT_SIZE);
    uint i = fromBytesToInt(dati + 3 * INT_SIZE);
    //dimensione totale del file
    uint dimFile = fromBytesToInt(dati + 4 * INT_SIZE);
    //caratteri letti in questa porzione
    uint totCharsRead = fromBytesToInt(dati + 5 * INT_SIZE);
    uint *occurrences = malloc(sizeof(uint)*ASCII_LENGTH);
    checkNotNull(occurrences);
    int j;
    for (j = 0; j < ASCII_LENGTH; j++) {
      occurrences[j] = fromBytesToInt(dati + (6+j)* INT_SIZE);
      //print("occurrences :'%c' n:%u",j,occurrences[j])
    }
    //funzione che aggiorna i dati del corrispondente file
    analyzerListUpdateFileData(analyzers,pid,idFile,dimFile,totCharsRead,occurrences,m);
    free(occurrences);
  } else {
    perror("updateDelFileFallito\n");
  }

}

void gotDeleteFilePacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati) {
  byte dati [dimDati];
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
 
}
void gotDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati) {
  byte dati [dimDati];
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
 
}
void got1stPathPartDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati) {
  byte dati [dimDati];
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
 
}
void got2ndPathPartDeleteFolderPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati) {
  byte dati [dimDati];
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
 
}
void gotErrorLogPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati){
  byte dati [dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    char* path;
    pid = fromBytesToInt(dati);
    path = dati + INT_SIZE;
    analyzerListAddError(analyzers,pid,path);
  } else {
    perror("errore in fase di aggiunta errore al log\n");
  }
 
}
void gotStartPacket(int pipe, byte *header, analyzerList *analyzers,uint dimDati){
  byte dati [dimDati];
  int rdDati = read(pipe, dati, dimDati);
  if (rdDati == dimDati) {
    uint pid;
    pid = fromBytesToInt(dati);
    analyzerListStart(analyzers,pid);
  } else {
    perror("errore in fase di start\n");
  }
  
}
int reportReadBatch(int pipe, analyzerList *analyzers,int batch){
  int i=0;
  int rd = 1;
  for(i=0;i<batch && rd>0;i++){
    rd = reportReadOnePacket(pipe,analyzers);
  }
}
// // This is the function that implements report buisiness logic
int reportReadOnePacket(int pipe, analyzerList *analyzers) {
    // Reading new packet and taking appropriate action
    byte header[INT_SIZE + 1];
    int rdHeader = read(pipe, header, INT_SIZE + 1);
    if (rdHeader == INT_SIZE + 1) {
      if (DEBUGGING)
        {printf("Got new packet with code %d\n", header[0]); sleep(1);}
      switch (header[0]) {
      case Q_NEW_DATA_CODE:
        gotNewDataPacket(pipe, header, analyzers,fromBytesToInt(header+1));
        break;
      case Q_FILE_ERROR_CODE:
        gotErrorFilePacket(pipe, header, analyzers,fromBytesToInt(header+1));
        break;
      case A_ANALYZER_START:
        gotStartPacket(pipe,header,analyzers,fromBytesToInt(header+1));
        break;
      case A_NEW_FILE_COMPLETE:
        gotAddFilePacket(pipe, header, analyzers,fromBytesToInt(header+1));
        break;
      case A_NEW_FILE_INCOMPLETE_PART1:
        got1stPathPartPacket(pipe, header, analyzers,fromBytesToInt(header+1));
        break;
      case A_NEW_FILE_INCOMPLETE_PART2:
        got2ndPathPartPacket(pipe, header, analyzers,fromBytesToInt(header+1));
        break;
      case A_DELETE_FILE_CODE:
        gotDeleteFilePacket(pipe, header, analyzers,fromBytesToInt(header+1));
        break;
      case A_DELETE_FOLDER:
        gotDeleteFolderPacket(pipe, header, analyzers,fromBytesToInt(header+1));
        break;
      case A_DELETE_FOLDER_INCOMPLETE_PART1:
        got1stPathPartDeleteFolderPacket(pipe, header, analyzers,fromBytesToInt(header+1));
        break;
      case A_DELETE_FOLDER_INCOMPLETE_PART2:
        got2ndPathPartDeleteFolderPacket(pipe, header, analyzers,fromBytesToInt(header+1));
        break;
      case A_ERROR_LOG:
        gotErrorLogPacket(pipe,header,analyzers,fromBytesToInt(header+1));   
        break;
      }
  }
  return rdHeader;
}
//       if (system("clear")) {
//         printf("\n----------------------------------------\n");
//       }
//       // Print right recap info based on argv
//       analyzerListPrint(analyzers);
//       //printFirstInfoLine(analyzers);
//       //printRecapCompact(analyzers);
//       if (argc == 1) {
//         printRecapCompact(analyzers);
//       } else if (contains(argc, argv, onlyFlag)) {
//         // Get file paths
//         int firstPathIdx = 2 + streq(argv[1], groupFlag) ? 1 : 0;
//         int pathsLen = argc - firstPathIdx;
//         char *paths[pathsLen];
//         short i;
//         //(I am sure of the format of the data because agv went through
//         // argsAreValid). NO...
//         for (i = 0; i < pathsLen; i++) {
//           paths[i] = argv[i + firstPathIdx];
//         }
//         printSelectedFiles(analyzers, pathsLen, paths,
//                            contains(argc, argv, groupFlag));
//       } else if (contains(argc, argv, verboseFlag)) {
//         printRecapVerbose(analyzers, contains(argc, argv, groupFlag));
//       } else {
//         // Should never get here if args are valid
//         perror("Arguments are not valid, please use --help for reference");
//         retCode = 1;
//         break;
//       }
//       //debug
//       //analyzerListPrint(analyzers);
//     }
//   }
//   return retCode;
// }

// // int main(int argc, const char *argv[]) {
// //   int retCode = 0;
// //   if (contains(argc, argv, helpFlag)) {
// //     printf("this is the help text"); // TODO
// //   } else {
// //     if (argsAreValid(argc, argv)) {
// //       retCode = report(argc, argv);
// //     }
// //     // Will never have an else since when args are not valid the program
// //     // terminates (argsAreValid calls exit w/ appropriate error code).
// //   }
// //   return retCode;
// // }
