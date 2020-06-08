#include "./packets.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int fd[2];
  const char *myfifo = "../myfifo";
  const int pid = 31;

  int occurences[256];
  int i;
  for (i = 0; i < 256; i++) {
    occurences[i] = 0;
  }
  occurences['a'] = 5;
  mkfifo(myfifo, 0666);
  fd[WRITE] = open(myfifo, O_WRONLY);
  newFileNameToReportPacket(fd, pid, 0, "/tmp/file1.txt");
  sendErrorMessage(fd, pid, "fsadfsdafasd eorrore dsfsadf "); 
  // sendOccurencesPacketToReport(fd, pid, 0, 4, 0, 40, 5, occurences);
  // newFileNameToReportPacket(fd, pid, 1, "/tmp/file2.txt");
  // // newFileNameToReportPacket(fd, pid, 2, "/home/michele/Documents/SISTEMI/"
  // //                                       "sistemi2020-report/src/report/"
  // //                                       "test_files_2/patate.txt");
  // // newFileNameToReportPacket(fd, pid, 3, "/home/michele/Documents/SISTEMI/"
  // //                                       "sistemi2020-report/src/report/"
  // //                                       "test_files_2/formaggio.txt");
  // // // sendOccurencesPacketToReport(fd, 40, 0, 5, 0, 50, 10, occurences);
  // // sendOccurencesPacketToReport(fd, pid, 3, 3, 0, 40, 5, occurences);
  // // sendOccurencesPacketToReport(fd, pid, 3, 9, 0, 40, 6, occurences);
  // // reportErrorOnFilePacket(fd, pid, 0);
  // // deleteFolderFromReportPacket(fd, pid, "/tmp/");
  // // sendOccurencesPacketToReport(fd, pid, 3, 3, 0, 45, 5, occurences);
  // // sendOccurencesPacketToReport(fd, pid, 3, 9, 0, 40, 6, occurences);
  // // newFileNameToReportPacket(fd, pid, 6, "/tmp/f.txt");
  // // sendOccurencesPacketToReport(fd, pid, 3, 9, 0, 0, 0, occurences); 
  // sendOccurencesPacketToReport(fd, pid, 1, 3, 0, 0, 0, occurences);
  return 0;
}