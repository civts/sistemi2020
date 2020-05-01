#include "../utils.c"

typedef struct {
  char *path;
  int totalCharacters;
  int occorrenze[256];
  bool fromFolder;
  fileStructure(char* path,int totalCharacters, int *occorrenze,bool fromFolder);
} fileStructure;

typedef struct {
  fileStructure *nextFile;
} fileDoubleLinkedList;