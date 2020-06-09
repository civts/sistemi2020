#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h> //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>
#include <unistd.h> //STDIN_FILENO

void clear() { system("clear"); }

typedef struct {
  int argc1, argc2;
  char **argv1, **argv2;
} separatedArgs;
separatedArgs splitArgv(int argc, char **argv);

int main(int argc, char **argv) {
  int returnCode = 0;
  int analyzer;
  int pipeReportOUT[2];
  int pipeAnalyzerOUT[2];
  int pipeReportIN[2];
  int pipeAnalyzerIN[2];
  pipe2(pipeReportOUT, O_NONBLOCK);
  pipe2(pipeReportIN, O_NONBLOCK);
  pipe2(pipeAnalyzerOUT, O_NONBLOCK);
  pipe2(pipeAnalyzerIN, O_NONBLOCK);
  separatedArgs argomenti = splitArgv(argc, argv);
  //primo analyzer, secondo report
  analyzer = fork();
  if (analyzer < 0) {
    fprintf(stderr, "Found an error creating the Analyzer\n");
    returnCode = 2;
  } else if (analyzer == 0) {
    if (dup2(pipeAnalyzerOUT[0], STDOUT_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    if (dup2(pipeAnalyzerIN[1], STDIN_FILENO) == -1) {
      perror("dup2");
      exit(EXIT_FAILURE);
    }
    argomenti.argv1[0]="analyzer";
    execv("./bin/analyzer", argomenti.argv1);
  } else {
    argomenti.argv2[0]="report";
    execv("./bin/report", argomenti.argv2);
    return 0;
  }
}

separatedArgs splitArgv(int argc, char **argv) {
  separatedArgs m;
  int i = 1;
  int posSeparatore;
  const char separatore[] = "---";
  m.argv1 = malloc(sizeof(char *) * (argc + 1));
  checkNotNull(m.argv1);
  m.argv2 = malloc(sizeof(char *) * (argc + 1));
  checkNotNull(m.argv2);
  while (i < argc) {
    if (streq(argv[i], separatore)) {
      break;
    } else {
      m.argv1[i] = argv[i];
      i++;
    }
  }
  posSeparatore = i;
  m.argv1[i] = NULL;
  m.argc1 = i + 1;
  if (i < argc) {
    i++;
    while (i < argc) {
      m.argv2[i - posSeparatore] = argv[i];
      i++;
    }
    i++;
    m.argc2 = i - posSeparatore;
    m.argv2[i - posSeparatore] = NULL;
  } else {
    m.argc2 = 2;
    m.argv2[1] = m.argv2[0] = NULL;
  }
  return m;
}
