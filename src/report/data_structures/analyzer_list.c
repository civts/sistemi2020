#include "./analyzer_list.h"

analyzerList *constructorAnalyzerListEmpty() {
  analyzerList *l = (analyzerList *)malloc(sizeof(analyzerList));
  checkNotNull(l);
  l->count = 0;
  l->dumps = false;
  l->firstNode = NULL;
  return l;
}

analyzerList *constructoranalyzerListOne(analyzer *a) {
  analyzerList *l = constructorAnalyzerListEmpty();
  analyzerListAppend(l, a);
  return l;
}

void destructoraAnalyzerList(analyzerList *l) {
  analyzer *current = l->firstNode;
  // delete every node until none is left
  while (current != NULL) {
    analyzer *nextNode = current->nextNode;
    if (nextNode != NULL) {
      destructorAnalyzer(current);
    }
    current = nextNode;
  }
  free(l);
}

bool analyzerListIsEmpty(analyzerList *l) { return l->count == 0; }

// Appends new analyzer node to end of analyzeranalyzerList - TESTED✔️
//
// ⚠️ do NOT deallocate an, the analyzerList will take care of it when
// needed
void analyzerListAppend(analyzerList *l, analyzer *an) {
  if (analyzerListIsEmpty(l)) {
    l->firstNode = an;
  } else {
    analyzer *cursor = l->firstNode;
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    cursor->nextNode = an;
    an->previousNode = cursor;
  }
  l->count++;
}

analyzer *analyzerListGetElementByPid(analyzerList *l, uint pid) {
  analyzer *current = l->firstNode;
  while (current != NULL) {
    if ((current->pid) == pid) {
      return current;
    }
    current = current->nextNode;
  }
  return NULL;
}

bool analyzerListRemoveElementByPid(analyzerList *l, uint pid) {
  bool deleted = false;

  analyzer *targetNode = analyzerListGetElementByPid(l, pid);
  if (targetNode != NULL) {

    analyzer *prev = targetNode->previousNode;
    analyzer *next = targetNode->nextNode;
    targetNode->nextNode=NULL;
    targetNode->previousNode=NULL;
    if (prev != NULL)
      prev->nextNode = next;
    else
      l->firstNode = next;

    if (next != NULL)
      next->previousNode = prev;
    destructorAnalyzer(targetNode);
    l->count--;
    deleted=true; 
  } else {

  }
  return deleted;
}

void analyzerListRemoveFirst(analyzerList *l) {
  if (!analyzerListIsEmpty(l)) {
    analyzer *newFirstNode = l->firstNode->nextNode;
    destructorAnalyzer(l->firstNode);
    l->firstNode = newFirstNode;
    l->count--;
  }
}

void analyzerListRemoveLast(analyzerList *l) {
  if (!analyzerListIsEmpty(l)) {
    analyzer *cursor = l->firstNode;
    // Get cursor to the last analyzerList node
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    // Detach from the analyzerList
    analyzer *penultimate = cursor->previousNode;
    if (penultimate != NULL)
      penultimate->nextNode = NULL;
    // Delete
    destructorAnalyzer(cursor);
    l->count--;
  }
}


void analyzerListStart(analyzerList *l, uint pid){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  // if no analyzer with given pid is found
  if (a == NULL) {
    a = constructorAnalyzer(pid, l->dumps);
    analyzerListAppend(l, a);
  }
  analyzerStart(a);
}

void analyzerListAddNewFile(analyzerList *l, uint pid, fileWithStats *fs) {
  analyzer *a = analyzerListGetElementByPid(l, pid);
  // if no analyzer with given pid is found
  if (a == NULL) {
    a = constructorAnalyzer(pid, l->dumps);
    analyzerListAppend(l, a);
  }
  // function that adds the file to the mainList
  analyzerAddNewFile(a, fs);
}
void analyzerListErrorFile(analyzerList *l, uint pid,uint idFile){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  // if no analyzer with given pid is found
  if (a == NULL) {
    a = constructorAnalyzer(pid, l->dumps);
    analyzerListAppend(l, a);
  }
  // function that adds the file to the mainList
  analyzerErrorFile(a, idFile);
}
void analyzerListAddIncompleteFile(analyzerList *l, uint pid,
                                   fileWithStats *fs) {
  analyzer *a = analyzerListGetElementByPid(l, pid);
  // if no analyzer with given pid is found
  if (a == NULL) {
    a = constructorAnalyzer(pid, l->dumps);
    analyzerListAppend(l, a);
  }
  // function that adds the file to the incompleteList
  analyzerAddIncompleteFile(a, fs);
}

void analyzerListUpdateFilePath(analyzerList *l, uint pid, uint idFile,
                                char *path) {
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    // funzione che esegue l'update
    analyzerUpdateFilePath(a, idFile, path);
  } else {
    // perror("analyzer non esistente\n");
  }
}

void analyzerListUpdateFileData(analyzerList *l, uint pid, uint idFile,
                                uint totChars, uint readChars,
                                uint occurrences[INT_SIZE],uint m) {
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    // update
    analyzerUpdateFileData(a, idFile, totChars, readChars,
                          occurrences,m);
    }else {
    // perror("analyzer non esistente\n");
  }
}

void analyzerListDeleteFile(analyzerList *l, uint pid, uint idFile) {
  // printf("pid %u\nidFile %u\n",pid,idFile);
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    analyzerDeleteFile(a, idFile);
  } else {
    // perror("analyzer non esistente\n");
  }
}
void analyzerListDeleteFolder(analyzerList *l,uint pid, char* path){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    analyzerDeleteFolder(a, path);
  } else {
    // perror("analyzer non esistente\n");
  }
}
void analyzerListIncompleteFolderDelete(analyzerList *l,uint pid, char* path){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    analyzerIncompleteFolderDelete(a, path);
  } else {
    // perror("analyzer non esistente\n");
  }
}
void analyzerListCompletionFolderDelete(analyzerList *l,uint pid, char* path){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    analyzerCompletionFolderDelete(a, path);
  } else {
    // perror("analyzer non esistente\n");
  }
}
// void analyzerListAddFilter(analyzerList * l,uint pid, char* path){
//   analyzer *a = analyzerListGetElementByPid(l, pid);
//   if (a != NULL) {
//     analyzerAddFilter(a, path);
//   } else {
//     // perror("analyzer non esistente\n");
//   }
// }
// void analyzerListRemoveFilter(analyzerList * l,uint pid, char* path){
//   analyzer *a = analyzerListGetElementByPid(l, pid);
//   if (a != NULL) {
//     analyzerRemoveFilter(a, path);
//   } else {
//     // perror("analyzer non esistente\n");
//   }
// }
void analyzerListAddError(analyzerList * l,uint pid, char* path){
  analyzer *a = analyzerListGetElementByPid(l, pid);
  if (a != NULL) {
    analyzerAddError(a, path);
  } else {
    // perror("analyzer non esistente\n");
  }
}
void analyzerListPrint(analyzerList *l) {
  analyzer *cursor = l->firstNode;
  while (cursor != NULL) {
    analyzerPrint(cursor);
    cursor = cursor->nextNode;
  }
  printf("analyzerList count: %d\n", l->count);
}

/*
// main di prova per testare
int main(int c, char *argv[]) {
  analyzerList * list = constructorAnalyzerListEmpty();
  analyzer *  a1 = constructorAnalyzer(1);
  analyzer *  a2 = constructorAnalyzer(2);
  analyzer *  a3 = constructorAnalyzer(3);
  analyzer *  a4 = constructorAnalyzer(4);
  analyzerListAppend(list,a1);
  analyzerListAppend(list,a2);
  analyzerListAppend(list,a3);
  //int x = analyzerListGetElementByPid(0);
  //analyzerListRemoveElementByPid(list,1);
  printAnalyzerList(list);
  analyzerListRemoveFirst(list);
  analyzerListRemoveLast(list);
  printAnalyzerList(list);
  //
  printAnalyzer(analyzerListGetAnalyzerByIndex(list,0));
  analyzerListRemoveElementByPid(list,2);
  return 0;
}
*/
