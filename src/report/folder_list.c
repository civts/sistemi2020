#include "../utils.c"
//#include "./folder.h"
#include "./file_with_stats_data_structure.h"
#include "./file_with_stats_list.h"
#include "./int_list.h"
#include <stdio.h>
#ifndef FOLDER_LIST_DATA_STRUCTURE_H
#define FOLDER_LIST_DATA_STRUCTURE_H

// serve per abilitare la doppia ricorsione
struct folder_t;

typedef struct {
  struct folder_t *firstNode;
  // how many nodes are currently in the folderList
  int count;
} folderList;

typedef struct folder_t {
  fwsList *fileList;
  folderList *subfolders;
  char *name;

  struct folder_t *nextNode;
  struct folder_t *previousNode;
} folder;


// FUNZIONI PER LA LISTA

// Returns reference to new empty folderList
folderList *constructorFolderListEmpty();

// dels l and everything within, de-allocating what's needed
void destructorFolderList(folderList *l);
// Returns wether the folderList is empty or not
bool folderListIsEmpty(folderList *l);
// Appends new node to end of folderList - TESTED✔️
void folderListAppend(folderList *l, folder *fs);

void folderListPrint(folderList *l);

// FUNZIONI PER LE CARTELLA

folder *constructorFolder(char *name);
void destructorFolder(folder *cartella);
// function that adds a file to the current folder, considering it's path to make the tree
void folderAddFile(folder *cartella, fileWithStats *newFile,char *path);
void folderPrint(folder *f);
// funzione che controlla che i due path siano indentici fino al primo /, ovvero
// appartengano alla stessa cartella
bool equalFirstFolder(char *path1, char *path2);

// IMPLEMENTAZIONI LISTA

folderList *constructorFolderListEmpty() {
  folderList *l = (folderList *)malloc(sizeof(folderList));
  l->count = 0;
  l->firstNode = NULL;
}


void destructorFolderList(folderList *l) {
  folder *current = l->firstNode;
  // del every node until none is left
  while (current != NULL) {
    folder *nextNode = current->nextNode;
    destructorFolder(current);
    current = nextNode;
  }
  free(l);
}

bool folderListIsEmpty(folderList *l) { return l->count == 0; }

void folderListAppend(folderList *l, folder *fs) {
  if (folderListIsEmpty(l)) {
    l->firstNode = fs;
  } else {
    folder *cursor = l->firstNode;
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    cursor->nextNode = fs;
    fs->previousNode = cursor;
  }
  l->count++;
}

// Prints the folderList debug
void folderListPrint(folderList *l) {
  folder *cursor = l->firstNode;
  printf("folderList count: %d\n", l->count);
  while (cursor != NULL) {
    folderPrint(cursor);
    cursor = cursor->nextNode;
  }
}

// IMPLEMENTAZIONI CARTELLA

folder *constructorFolder(char *name) {
  folder *res = (folder *)malloc(sizeof(folder));
  res->fileList = constructorFwsListEmpty();
  res->subfolders = constructorFolderListEmpty();
  res->name = malloc(sizeof(char) * strlen(name) + 1);
  strcpy(res->name, name);
  res->nextNode = NULL;
  res->previousNode = NULL;
}

void destructorFolder(folder *cartella) {
  destructorFwsList(cartella->fileList);
  if (cartella->subfolders != NULL) {
    destructorFolderList(cartella->subfolders);
  }
  free(cartella);
}

char *ithFolder(char *path, int i) {
  int j = 0;
  int cursor = 1;
  while (j < i) {
    if (path[cursor] == '/')
      j++;
    if (j == i) {
    }
    cursor++;
  }
}
// estrae il primo nome di cartella
char *firstFolder(char *path) {
  int j = 0;
  if (path[j] == '/')
    j++;
  int length = 0;
  while (path[j] != '\0' && path[j] != '/') {
    j++;
    length++;
  }
  char *name = malloc(sizeof(char) * length + 1);
  j = 0;
  int i = 0;
  if (path[j] == '/')
    j++;
  while (i < length) {
    name[i] = path[j];
    i++;
    j++;
  }
  name[i] = '\0';

  return name;
}
int countLettersFirstFolder(char *path) {
  int j = 0;
  if (path[j] == '/')
    j++;
  while (path[j] != '\0' && path[j] != '/') {
    j++;
  }
  return j;
}
int countFolders(char *path) {
  int j = 0;
  if (path[j] == '/')
    j++;
  int count = 0;
  while (path[j] != '\0') {
    if (path[j] == '/')
      count++;
    j++;
  }
  return count;
}
char *lastFolder(char *path) {
  int last = countFolders(path);
  return ithFolder(path, last - 1);
}

bool equalFirstFolder(char *path1, char *path2) {
  int i = 0;
  int j = 0;
  if (path1[i] == '/')
    i++;
  if (path2[j] == '/')
    j++;
  int a = i + 1;
  int b = j + 1;
  while (path1[a] != '/')
    a++;
  while (path2[b] != '/')
    b++;
  bool equals = true;
  while (i < a && j < b) {
    if (path1[i] != path2[j])
      equals = false;
    i++;
    j++;
  }
  return equals;
}

void folderAddFile(folder *cartella, fileWithStats *newFile, char *path) {
  // controllo  se appartiene direttamente alla cartella corrente se sì aggiungo
  // qui? no potenzialmente ho anche cartelle da costruire!
  // prendo il nome della prima cartella
  // printf("cartella : %s\n",cartella->name);
  // printf("path rimanente : %s\n",path);
  // se si tratta di un file da aggiungere qui
  if (countFolders(path) < 1) {
    fwsListAppend(cartella->fileList, newFile);
  }
  // altrimenti controllo se appartiene ad una delle sottocartelle
  else {
    char *primaCartella = firstFolder(path);
    folder *nested =
        folderListGetElementByName(cartella->subfolders, primaCartella);
    if (nested == NULL) {
      // aggiungo una cartella alla lista
      nested = constructorFolder(primaCartella);
      folderListAppend(cartella->subfolders, nested);
    }
    // cancello solo il nome, che è già stato copiato
    free(primaCartella);
    folderAddFile(nested, newFile, path + countLettersFirstFolder(path));
  }
  //free(prim
}

int countFilesInFolder(folder *root) {
  int result = root->fileList->count;
  folderList *subFolders = root->subfolders;
  if (subFolders != NULL) { //controllo non necessario in quanto una cartella ha sempre una lista di sottocartelle, possibilmente vuota
    folder* nested = subFolders->firstNode;
    while(nested!=NULL){
      result += countFilesInFolder(nested);
      nested = nested->nextNode;
    }
  }
  return result;
}
// recursion
fileWithStats* folderGetElementByID(folder * root,uint id){
  fileWithStats * searched = fwsListGetElementByID(root->fileList,id);
  if(searched==NULL){
    folderList *subFolders = root->subfolders;
    if (subFolders != NULL) { //controllo non necessario in quanto una cartella ha sempre una lista di sottocartelle, possibilmente vuota
      folder* nested = subFolders->firstNode;
        while(nested!=NULL && searched==NULL){
          searched = folderGetElementByID(nested,id);
          nested = nested->nextNode;
      }
    }
  }
  return searched;
}
// recursion
bool folderRemoveElementByID(folder * root,uint id){
  bool removed = fwsListRemoveElementByID(root->fileList,id,true);
  if(!removed){
    folderList *subFolders = root->subfolders;
    if (subFolders != NULL) { //controllo non necessario in quanto una cartella ha sempre una lista di sottocartelle, possibilmente vuota
      folder* nested = subFolders->firstNode;
        while(nested!=NULL && !removed){
          removed = folderRemoveElementByID(nested,id);
          nested = nested->nextNode;
      }
    }
  }
  return removed;
}
void folderPrint(folder *f) {
  char *name;
  printf("folder name: %s\n", f->name);
  fwsListPrint(f->fileList);
  folderListPrint(f->subfolders);
}
#endif

// main di prova per testarefolder
int main(int c, char *argv[]) {
  // folderList * folderLista =  constructorFolderListEmpty();
  // folderList * folderListaAnnidata =  constructorFolderListEmpty();
  // folderListAppend(folderListaAnnidata,constructorFolder("cibo"));
  // folderListAppend(folderLista, constructorFolder("ciao"));
  // folderListAppend(folderLista,constructorFolder("patate"));
  // folderLista->firstNode->nextNode->sottocartelle = folderListaAnnidata;

  folder *cartellaTest = constructorFolder("");
  fileWithStats *a = constructorFWS("/root/a.txt", 1, 0, NULL, true);
  fileWithStats *f = constructorFWS("/root/f.txt", 1, 0, NULL, true);
  fileWithStats *b = constructorFWS("/root/patate/b.txt", 4, 0, NULL, true);
  fileWithStats *e = constructorFWS("/root/patate/c.txt", 1, 0, NULL, true);
  fileWithStats *d = constructorFWS("/root/d.txt", 1, 0, NULL, true);
  folderAddFile(cartellaTest, a, a->path);
  folderAddFile(cartellaTest, f, f->path);
  folderAddFile(cartellaTest, b, b->path);
  folderAddFile(cartellaTest, e, e->path);
  folderAddFile(cartellaTest, d, d->path);
  folderPrint(cartellaTest);
  int tot = countFilesInFolder(cartellaTest);
  printf("%d",tot);
  printf("\n\n2");
  fwsPrint(folderGetElementByID(cartellaTest,4));
  printf("\n\n");
  printf("\n\n");
  folderRemoveElementByID(cartellaTest,4);
  folderPrint(cartellaTest);
  
  // printf("count %d",countFolders("/b.txt"));
  // char * txt = "/root/patate/b.txt";
  // printf("count %d\n",countLettersFirstFolder(txt));
  // printf("count %s\n",txt+countLettersFirstFolder(txt));
  // char * p = "banana/casa/a.txt";
  // char * a = ithFolder(p,1);
  // printf("cartella : %s\n",a);
  // printf("cartella : %s\n",lastFolder(p));
  // char * b = "/a/a.txt";
  // bool res = equalFirstFolder(p,b);
  // printf("i due sono uguali ? %s",res==1?"true":"false");
  // printf("count %d",countFolders(b));
  // char * first = firstFolder(p);
  // printf("fisrtFOleder %s\n",first);

  return 0;
}
