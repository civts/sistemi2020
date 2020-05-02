#include "../packet_codes.c"
#include "../utils.c"
#include "./report_utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef bool
typedef unsigned char bool;
#define false 0
#define true 1
#endif

// Path to the named pipe
const char *PATH_TO_REPORT_PIPE = "/path/to/pipe";
// How many bytes to read every time from the pipe
const int BATCH_SIZE = 128;

// TODO: candidate for ../utils.c
// Returns if argv contains "--help" or not
bool doesTheUserNeedHelp(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  int retCode = 0;
  if (doesTheUserNeedHelp(argc, argv)) {
    printf("this is the help text"); // TODO
  } else {
    char *message = argsAreValid(argc, argv);
    if (message != NULL) {
      retCode = report(argc, argv);
    } else {
      retCode = 1;
      perror(message);
      free(message);
    }
  }
  return retCode;
}

// This is the function that implements report buisiness logic
int report(int argc, char *argv[]) {
  int retCode = 0;
  // open the pipe
  int fd = open(PATH_TO_REPORT_PIPE, O_RDONLY);
  if (fd == -1) {
    retCode = 1;
    perror("No pipe");
  } else {
    // While the pipe is open
    int readBytes = BATCH_SIZE;

    int numeroFilesAnalizzati = 15; // TODO
    int numeroCartelle = 15;        // TODO
    do {
      byte statusCode;
      // statusCode
      read(fd, &statusCode, 1);

      // name length (in bytes)
      byte buffer[4];
      read(fd, buffer, INT_SIZE);
      uint nameLength = fromBytesToInt(buffer);

      // name
      char fileName[nameLength + 1];
      read(fd, &fileName, nameLength + 1);
      fileName[nameLength] = '\0';

      switch (statusCode) {
      case 0:
        // nuovo pacchetto con dati
      }

      // data
      // TODO leggere sti dati
      int frequenze[256];
      // N° caratteri nel file
      int lunghezzaFile;
    } while (readBytes != 0);

    print("Analizzati %d files e %d cartelle", numeroFilesAnalizzati,
          numeroCartelle);
    stampaListaFiles();
  }
  return retCode;
}

bool doesTheUserNeedHelp(int argc, char *argv[]) {
  bool result = false;
  for (int i = 1; i < argc; i++) {
    if (argv[i] == "--help") {
      result = true;
      break;
    }
  }
  return result;
}

// Stampa su stdout le statistiche di un file
void stampaSingoloFile(char *nomeFile, int dati[], int caratteriTot, int argc,
                       char *argv) {
  print("---------------%s---------------", nomeFile);

  print("Caratteri totali: %d\n", caratteriTot);
}

// Stampa ogni carattere
void stampaSingoliCaratteri(int dati[], int caratteriTot) {
  char c;
  int i = 0;
  for (i = 0; i < 256; i++) {
    print("%c: %d\n", c, dati[i]);
  }
}

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
    print("%c: %d\n", c, dati[i]);
  }
}
