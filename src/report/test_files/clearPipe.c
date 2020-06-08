#include "../../packet_codes.h"
#include "../../utils.c"
#include "../file_with_stats_list.h"
#include "../report_utils.h"
#include <fcntl.h>
#ifndef bool
typedef unsigned char bool;
#define false 0
#define true 1
#endif

// Path to the named pipe
const char *PATH_TO_A = "../myfifo";
// How many bytes to read every time from the pipe
const int BATCH_SIZE = 128;

int main(int argc, const char *argv[]) {
  int retCode = 0;
  int fd = open(PATH_TO_A, O_RDONLY | O_NONBLOCK);
  int rd = 1;
  if (fd == -1) {
    retCode = 1;
    printf("nopipe\n");
  } else {
    do {
      // while(true)
      /*if (statusCode[0] == EOF) {
        // If there is no one on the writing end of the pipe previous read will
        // retur EOF (see
        // https://www.geeksforgeeks.org/non-blocking-io-with-pipes-in-c/)
        // TODO handle (no one is transmitting)
        return 1;
      }*/
      // Getting variables which I'll need anyway (path to the file and its
      // length)
      byte buffer[1] = {'0'};
      rd = read(fd, buffer, 1);
      // Will never have an else since when they are not valid  the program
      // terminates
    } while (rd != 0);
    close(fd);
    //}
  }
  return retCode;
}
