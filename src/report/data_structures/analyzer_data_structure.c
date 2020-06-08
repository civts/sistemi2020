#include "./analyzer_data_structure.h"
#include <time.h>
// constructor for Analyzer
analyzer *constructorAnalyzer(uint pid, bool dumps) {
  analyzer *a = (analyzer *)malloc(sizeof(analyzer));
  checkNotNull(a);
  a->pid = pid;
    // a->dumpFD = NULL;
  a->files = constructorFwsListEmpty();
  a->incompleteList = constructorFwsListEmpty();
  a->incompletePathToDelete = NULL;
  a->errors = constructorFwsListEmpty();
  a->errorMessages = constructorNamesList();
  a->filters = constructorNamesList();
  a->dumps = dumps;
  a->dumpFD = NULL;
  a->previousNode = NULL;
  a->nextNode = NULL;
  if (DEBUGGING)
    printf("Creating a new Analyzer instance @%p for Analyzer with pid %u\n", a,
           a->pid);
  return a;
}

// Destructor for Analyzer
// TODO: seems ok but we need to test for memory leaks
void destructorAnalyzer(analyzer *a) {
  if (DEBUGGING)
    printf("Deleting Analyzer instance @%p for Analyzer with pid %u\n", a,
           a->pid);
  destructorFwsList(a->files);
  destructorFwsList(a->incompleteList);
  if(a->incompletePathToDelete!=NULL)
    free (a->incompletePathToDelete);
  destructorFwsList(a->errors);
  deleteNamesList(a->errorMessages);
  deleteNamesList(a->filters);
  fflush(a->dumpFD);
  fclose(a->dumpFD);
  free(a);
}

void analyzerStart(analyzer *a){
  fwsListResetData(a->files);
}
void analyzerAddNewFile(analyzer *a, fileWithStats *fs) {
  if(DEBUGGING) {printf("ADD file calle for %s\n", fs->path);
  fwsPrint(fs);}
  //controllo non sia già presente
  fileWithStats *isInFiles = fwsListGetElementByID(a->files, fs->id);
  fileWithStats *isInErrors = fwsListGetElementByID(a->errors, fs->id);
  
  if (isInFiles == NULL && isInErrors==NULL) {
    fwsListAppend(a->files,fs);
  } else {
    destructorFWS(fs);
  }
}
void analyzerErrorFile(analyzer *a,uint id){
  // controllo non sia già  presente
  fileWithStats *isInFiles = fwsListGetElementByID(a->errors,id);
  if (isInFiles == NULL) {
    isInFiles = fwsListGetElementByID(a->files,id);
    if(isInFiles != NULL){
      fwsListRemoveElementByID(a->files,id,false);
      fwsListAppend(a->errors,isInFiles);
    }
  } 
}
void analyzerAddIncompleteFile(analyzer *a, fileWithStats *fs) {
  //controllo non sia già presente
  fileWithStats *n = fwsListGetElementByID(a->incompleteList, fs->id);
  fileWithStats *isInErrors = fwsListGetElementByID(a->errors, fs->id);
  if (n == NULL && isInErrors==NULL) {
    fwsListAppend(a->incompleteList, fs);
  } else {
    destructorFWS(fs);
  }
}

void analyzerUpdateFilePath(analyzer *a, uint idFile, char *path) {
  // update the path
  fwsListUpdateFilePath(a->incompleteList, idFile, path);
  // get the FWS
  fileWithStats *updatedNode = fwsListGetElementByID(a->incompleteList, idFile);
  // remove from incompleteList
  fwsListRemoveElementByID(a->incompleteList, idFile, false);
  // add to files/folders
  if (updatedNode != NULL){
    analyzerAddNewFile(a,updatedNode);
  }
}

void analyzerDeleteFile(analyzer *a, uint idFile) {
  //remove from mainlist
  bool removed = fwsListRemoveElementByID(a->files,idFile,true);
  if(!removed)
    removed = fwsListRemoveElementByID(a->errors,idFile,true);
}

void analyzerUpdateFileData(analyzer *a, uint idFile,
                                uint totChars, uint readChars,
                                uint occurrences[INT_SIZE],uint m) {
  fileWithStats * searched = fwsListGetElementByID(a->files,idFile);
  if(searched!=NULL){
    fwsUpdateFileData(searched,totChars,readChars,occurrences,m);
  }
}

void analyzerDeleteFolder(analyzer *a, char * path){
  fwsListDeleteFolder(a->files,path);
  fwsListDeleteFolder(a->errors,path);
}

void analyzerIncompleteFolderDelete(analyzer *a, char * path){
  a->incompletePathToDelete = malloc(sizeof(char)*strlen(path)+1);
   checkNotNull(a->incompletePathToDelete);
  strcpy(a->incompletePathToDelete,path);
}

void analyzerCompletionFolderDelete(analyzer *a, char * path){
  char *oldPath = a->incompletePathToDelete;
  char *completePath = (char *)malloc(strlen(oldPath) + strlen(path) + 1);
   checkNotNull(completePath);
  strcpy(completePath, oldPath);
  strcat(completePath, path);
  analyzerDeleteFolder(a,completePath);
  free(oldPath);
  free(completePath);
  a->incompletePathToDelete = NULL;
}

// void analyzerAddFilter(analyzer * a, char* path){
//   appendToNamesList(a->filters,path);
// }

// void analyzerRemoveFilter(analyzer * a, char* path){
//   removeNodeNameByName(a->filters,path);
// }

void analyzerAddError(analyzer *a, char *error) {
  appendToNamesList(a->errorMessages, constructorNodeName(error));
  if (a->dumps) {
    if (a->dumpFD == NULL) {
      char name[100];
      sprintf(name, "./analyzer_");
      sprintf(name + strlen(name), "%d", a->pid);
      sprintf(name + strlen(name), "_dump.txt");
      a->dumpFD = fopen(name, "w");
    }
    // Various reasons could cause us to not be able to open the file now.
    // in this case we will not print this error and try to open again the
    // next time an error arrives
    if (a->dumpFD != NULL) {
      time_t t = time(NULL);
      struct tm tm = *localtime(&t);
      fprintf(a->dumpFD, "%d-%02d-%02d %02d:%02d:%02d\t", tm.tm_year + 1900,
              tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
      fprintf(a->dumpFD, "%s", error);
      char lastChar = error[strlen(error) - 1];
      if (lastChar != '\n' && lastChar != '\r' && lastChar != '\f')
        fprintf(a->dumpFD, "\n");
      fflush(a->dumpFD);
    }
  }
}

void analyzerPrintErrorMessages(const analyzer *a) {
  NodeName *n = a->errorMessages->last;
  if(n==NULL) return;
  int i;
  // go back 2 steps
  for (i = 0; i < 2; i++) {
    if (n->prev == NULL)
      break;
    n = n->prev;
  }
  // forward while printing
  for (i = 0; i < 3; i++) {
    if (n == NULL)
      break;
    printf("%s", n->name);
    // if message does not end with \n we add it
    if (n->name[strlen(n->name - 1)] != '\n')
      printf("\n");
    n = n->next;
  }
}
// stampa debug
void analyzerPrint(analyzer *a) {
  printf("analyzer pid: %u\n", a->pid);
  printf("analyzer filesList:\n");
  fwsListPrint(a->files);
  printf("analyzer incompleteList:\n");
  fwsListPrint(a->incompleteList);  
}
