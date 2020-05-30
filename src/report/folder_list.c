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
// searches a folder by it's name. It is not recursive. It will search only in the list
folder *folderListGetElementByName(folderList *l, char * name);
// searches for a file, recursevely on all folders and subfolders. Return NULL if not found
fileWithStats *folderListGetElementByID(folderList *l, uint id);
// searches for a file recursevely and removes it. Return true if removed from the list. Does nothing if element is not found. Delete if the element must be deleted
bool folderListRemoveElementByID(folderList *l, uint id,bool delete);
// adds the new file to the correct folder in the folder list. It does so considering the subtree
void folderListAddFile(folderList * l, fileWithStats * fs, char * path);
// function that counts the number of files nested in the folderList
int folderListCountFiles(folderList*l);
// prints reccurisively
void folderListPrint(folderList *l);

// FUNZIONI PER LE CARTELLA

folder *constructorFolder(char *name);
void destructorFolder(folder *cartella);
// searches the file with given id recusively. return NULL if none is found
fileWithStats* folderGetElementByID(folder * root,uint id);
// removes the file with given id recusively. true if it is removed from the folder. Delete to true if the elemente must be deleted
bool folderRemoveElementByID(folder * root,uint id,bool delete);
// estracts the first part of a name
char *firstFolder(char *path);
// function that adds a file to the current folder, considering it's path to make the tree
void folderAddFile(folder *cartella, fileWithStats *newFile,char *path);
// function that counts the total number of nested files
int folderCountFiles(folder *root); 
// function that counts the number or // in a path
int countFolders(char *path);
void folderPrint(folder *f);


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

folder *folderListGetElementByName(folderList *l, char * name) {
  folder *current = l->firstNode;
  while (current != NULL) {
    if (streq(current->name,name)) {
      return current;
    }
    current = current->nextNode;
  }
  return NULL;
}

fileWithStats *folderListGetElementByID(folderList *l, uint id){
  fileWithStats * searched = NULL;
  folder *current = l->firstNode;
  while (current != NULL && searched==NULL) {
    searched = folderGetElementByID(current,id);
    current = current->nextNode;
  }
  return searched;
}

bool folderListRemoveElementByID(folderList *l, uint id,bool delete){
  bool removed = false;
  folder *current = l->firstNode;
  while (current != NULL && !removed) {
    removed = folderRemoveElementByID(current,id,delete);
    current = current->nextNode;
  }
  return removed;
}

void folderListAddFile(folderList * l, fileWithStats * fs, char * path){
  char * firstName = firstFolder(path);
  folder* match  = folderListGetElementByName(l,firstName);
  if(match==NULL){
    match = constructorFolder(firstName);
    folderListAppend(l,match);
  }
  int count = strlen(firstName);
  free(firstName);
  folderAddFile(match,fs,path + count + 1);
}

int folderListCountFiles(folderList*l){
  int result = 0;
  folder * current = l->firstNode;
  while(current!=NULL){
    result += folderCountFiles(current);
  }
  return result;
}

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

// changes the oldRoot. Appends to the newRoot the oldRoot. returns the newRoot
folder* folderChageRoot(folder * oldRoot, folder * newRoot){

}

void folderAddFile(folder *cartella, fileWithStats *fs, char *path) {
  if (countFolders(path) < 1) {
    fwsListAppend(cartella->fileList, fs);
  }
  // altrimenti controllo se appartiene ad una delle sottocartelle
  else {
    folderListAddFile(cartella->subfolders,fs,path);
  }
}

int folderCountFiles(folder *root) {
  int result = root->fileList->count;
  folderList *subFolders = root->subfolders;
  if (subFolders != NULL) { //controllo non necessario in quanto una cartella ha sempre una lista di sottocartelle, possibilmente vuota
    result += folderListCountFiles(subFolders);
  }
  return result;
}

fileWithStats* folderGetElementByID(folder * root,uint id){
  fileWithStats * searched = fwsListGetElementByID(root->fileList,id);
  if(searched==NULL){
    folderList *subFolders = root->subfolders;
    if (subFolders != NULL) { //controllo non necessario in quanto una cartella ha sempre una lista di sottocartelle, possibilmente vuota
      searched = folderListGetElementByID(subFolders,id);
    }
  }
  return searched;
}

bool folderRemoveElementByID(folder * root,uint id,bool delete){
  bool removed = fwsListRemoveElementByID(root->fileList,id,delete);
  if(!removed){
    folderList *subFolders = root->subfolders;
    if (subFolders != NULL) { //controllo non necessario in quanto una cartella ha sempre una lista di sottocartelle, possibilmente vuota
      removed = folderListRemoveElementByID(root->subfolders,id,delete);
    }
  }
  return removed;
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

void folderPrint(folder *f) {
  char *name;
  printf("folder name: %s\n", f->name);
  fwsListPrint(f->fileList);
  folderListPrint(f->subfolders);
}

// Returns the stats for this folder and its subfolders
charGroupStats statsForFolder(folder *f) {
  charGroupStats result;
  result.az = result.AZ = result.digits = result.punctuation = result.spaces =
      result.otherChars = result.totalChars = result.totalCharsRead = 0;
  fileWithStats *fileCursor = f->fileList->firstNode;
  // foreach subfolder get the stats and add to result
  while (fileCursor != NULL) {
    charGroupStats fileStats = statsForFile(fileCursor);

    result.az += fileStats.az;
    result.AZ += fileStats.AZ;
    result.digits += fileStats.digits;
    result.punctuation += fileStats.punctuation;
    result.spaces += fileStats.spaces;
    result.otherChars += fileStats.otherChars;
    result.totalCharsRead += fileStats.totalCharsRead;
    result.totalChars += fileStats.totalChars;
    
    fileCursor = fileCursor->nextNode;
  }
  folder *subfolder = f->subfolders->firstNode;
  // foreach subfolder get the stats and add to result
  while (subfolder != NULL) {
    charGroupStats subfStats = statsForFolder(subfolder);

    result.az += subfStats.az;
    result.AZ += subfStats.AZ;
    result.digits += subfStats.digits;
    result.punctuation += subfStats.punctuation;
    result.spaces += subfStats.spaces;
    result.otherChars += subfStats.otherChars;
    result.totalCharsRead += subfStats.totalCharsRead;
    result.totalChars += subfStats.totalChars;

    subfolder = subfolder->nextNode;
  }
  return result;
}

#endif

// // main di prova per testarefolder
// int main(int c, char *argv[]) {
//   // folderList * folderLista =  constructorFolderListEmpty();
//   // folderList * folderListaAnnidata =  constructorFolderListEmpty();
//   // folderListAppend(folderListaAnnidata,constructorFolder("cibo"));
//   // folderListAppend(folderLista, constructorFolder("ciao"));
//   // folderListAppend(folderLista,constructorFolder("patate"));
//   // folderLista->firstNode->nextNode->sottocartelle = folderListaAnnidata;

//   folder *cartellaTest = constructorFolder("");
//   fileWithStats *a = constructorFWS("/root/a.txt", 1, 0, NULL, true);
//   fileWithStats *f = constructorFWS("/root/f.txt", 1, 0, NULL, true);
//   fileWithStats *d = constructorFWS("/root/d.txt", 1, 0, NULL, true);
//   fileWithStats *b = constructorFWS("/root/patate/b.txt", 4, 0, NULL, true);
//   fileWithStats *e = constructorFWS("/root/patate/c.txt", 1, 0, NULL, true);
  
//   folderAddFile(cartellaTest, a, a->path);
//   folderAddFile(cartellaTest, f, f->path);
//   folderAddFile(cartellaTest, b, b->path);
//   folderAddFile(cartellaTest, e, e->path);
//   folderAddFile(cartellaTest, d, d->path);
//   folderPrint(cartellaTest);
//   int tot = countFilesInFolder(cartellaTest);
//   printf("%d",tot);
//   printf("\n\n");
//   fileWithStats *patate = folderGetElementByID(cartellaTest,4);
//   if(patate!=NULL){
//     fwsPrint(patate);
//   }
//   // printf("\n\n");
//   // printf("\n\n");
//   folderRemoveElementByID(cartellaTest,4,true);

//   patate = folderGetElementByID(cartellaTest,4);
//   if(patate!=NULL){
//     fwsPrint(patate);
//   }
//   folderPrint(cartellaTest);
  
//   // printf("count %d",countFolders("/b.txt"));
//   // char * txt = "/root/patate/b.txt";
//   // printf("count %d\n",countLettersFirstFolder(txt));
//   // printf("count %s\n",txt+countLettersFirstFolder(txt));
//   // char * p = "banana/casa/a.txt";
//   // char * a = ithFolder(p,1);
//   // printf("cartella : %s\n",a);
//   // printf("cartella : %s\n",lastFolder(p));
//   // char * b = "/a/a.txt";
//   // bool res = equalFirstFolder(p,b);
//   // printf("i due sono uguali ? %s",res==1?"true":"false");
//   // printf("count %d",countFolders(b));
//   // char * first = firstFolder(p);
//   // printf("fisrtFOleder %s\n",first);

//   return 0;
// }
