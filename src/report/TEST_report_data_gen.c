#include "../utils.c"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const int FILESTOGEN = 1;

typedef char *string;

int main(int argc, char **argv) {
  int fd;

  // FIFO file path
  char *myfifo = "/tmp/myfifo";

  // Creating the named file(FIFO)
  // mkfifo(<pathname>, <permission>)
  mkfifo(myfifo, 0666);

  // Open FIFO for write only
  int i;
  for (i = 0; i < FILESTOGEN; i++) {
    fd = open(myfifo, O_WRONLY);
    // packet code
    write(fd, '0', 1);
    char *path = "/abcd/file";
    // path length
    byte bytes[INT_SIZE];
    fromIntToBytes(strlen(path), bytes);
    write(fd, bytes, INT_SIZE);

    // path
    write(fd, path, strlen(path) + 1);

    // from folder/not flag
    write(fd, 1, 1);

    // total chars
    uint totalChars = i;
    fromIntToBytes(totalChars, bytes);
    write(fd, bytes, INT_SIZE);
    int j;
    for (j = 0; j < ASCII_LENGTH; j++) {
      write(fd, bytes, INT_SIZE);
    }
    close(fd);
  }
  return 0;
}