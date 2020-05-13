#include "../../packet_codes.h"
#include "../../utils.c"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef char *string;

int main(int argc, char **argv) {
  int fd;

  // FIFO file path
  char *myfifo = "../myfifo";

  // Creating the named file(FIFO)
  // mkfifo(<pathname>, <permission>)
  mkfifo(myfifo, 0666);

  // fd = open(myfifo, O_WRONLY |  O_NONBLOCK);

  fd = open(myfifo, O_WRONLY);

  byte header[INT_SIZE + 1] = {'\0', '\0', '\0', '\0', '\0'};
  fromIntToBytes(INT_SIZE * 2 + 6, header + 1);
  int i = 0;
  // header[0]= NEW_FILE_CODE_P1;

  header[0] = A_NEW_FILE_INCOMPLETE_PART2;
  printf("%c\n", header[0]);
  /*
  uint sei = fromBytesToInt(header+1);

  printf("%u\n",sei);
  */
  write(fd, header, INT_SIZE + 1);
  byte dati[INT_SIZE * 2 + 6] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0',
                                 '\0', 'a',  'b',  'a',  'c',  'o',  '\0'};
  uint pid = 55;
  fromIntToBytes(pid, dati);
  uint idFile = 333;
  fromIntToBytes(idFile, dati + INT_SIZE);
  write(fd, dati, INT_SIZE * 2 + 6);
  close(fd);
  return 0;
}
