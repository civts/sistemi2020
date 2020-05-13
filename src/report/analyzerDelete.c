#include "../utils.c"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../packet_codes.h"

typedef char *string;

int main(int argc, char **argv) {
  int fd;

  // FIFO file path
  char *myfifo = "./A";

  // Creating the named file(FIFO)
  // mkfifo(<pathname>, <permission>)
  mkfifo(myfifo, 0666);

  //fd = open(myfifo, O_WRONLY |  O_NONBLOCK);

  fd = open(myfifo, O_WRONLY );
  byte header[INT_SIZE+1] = {'\0','\0','\0','\0','\0'};
  header[0] = DELETE_FILE_CODE;
  uint dataSize = 2*INT_SIZE;
  fromIntToBytes(dataSize,header+1);
  int i=0;
  printf("%c\n",header[0]);
  /*
  uint sei = fromBytesToInt(header+1);

  printf("%u\n",sei);
  */
  write(fd,header,INT_SIZE+1);
  byte dati[INT_SIZE*2] = {'\0','\0','\0','\0',
                              '\0','\0','\0','\0'};
  uint pid = 55;
  fromIntToBytes(pid,dati);
  uint idFile = 333;
  fromIntToBytes(idFile,dati+INT_SIZE);
  write(fd,dati,INT_SIZE*2);
  close(fd);
  return 0;
}