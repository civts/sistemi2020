#include "../../common/packets.c"
#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
int main() {
  int fd[2];
  const char *myfifo = "/tmp/fifo";
  const int pid = 31;

  int occurences[256];
  int i;
  for (i = 0; i < 256; i++) {
    occurences[i] = 0;
  }
  occurences['a'] = 5;
  int v = mkfifo(myfifo, 0666);
  printf("dummytext",errno);
  if(( v!= 0)){
    perror("dummytext\n");
  }
  // mkfifo(myfifo, 0666);
  fd[WRITE] = open(myfifo, O_WRONLY);
  sendStartAnalysisPacket(fd,pid);
  newFileNameToReportPacket(fd, pid, 0, "/Documents/sistemi2020/src/report/test_files_3/packets.h");
  //sendErrorMessage(fd, pid, "fsadfsdafasd eorrore dsfsadf "); 
  sendOccurencesPacketToReport(fd, pid, 0, 4, 0, 40, 5, occurences);
  // newFileNameToReportPacket(fd, pid, 1, "/tmp/file2.txt");
  // // newFileNameToReportPacket(fd, pid, 2, "/home/michele/Documents/SISTEMI/"
  // //                                       "sistemi2020-report/src/report/"
  // //                                       "test_files_2/dummy.txt");
  // // newFileNameToReportPacket(fd, pid, 3, "/home/michele/Documents/SISTEMI/"
  // //                                       "sistemi2020-report/src/report/"
  // //                                       "test_files_2/isaond.txt");
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
  // sleep(60);
  sendTextMessageToReport(fd,"adsadsasddsad");
  int ex = close(pipe);
  // if(ex!=0) perror("Pipe non chiusa\n");
  //   ex = remove(myfifo);
  return 0;
}