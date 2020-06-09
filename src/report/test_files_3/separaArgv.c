#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int bool;
#define false 0;
#define true 1;

bool streq(const char *a, const char *b) { return !strcmp(a, b); }

typedef struct {
  int argc1, argc2;
  char **argv1, **argv2;
} separatedArgs;

// returns position of magicoseparatore
separatedArgs splitArgv(int argc, char **argv) {
  separatedArgs m;
  int i = 1;
  int posSeparatore;
  const char separatore[] = "magico";
  m.argv1 = malloc(sizeof(char *) * (argc + 1));
  //checkNotNull(m.argv1);
  m.argv2 = malloc(sizeof(char *) * (argc + 1));
  // checkNotNull(m.argv2);
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

int main(int argc, char **argv) {
  separatedArgs m = splitArgv(argc, argv);
  int i;
  printf("%d ", m.argc1);
  for (i = 0; i < m.argc1; i++) {
    printf("%s ", m.argv1[i]);
  }
  printf("\n");
  printf("%d ", m.argc2);
  for (i = 0; i < m.argc2; i++) {
    printf("%s ", m.argv2[i]);
  }
  printf("\n");
}