#include "../utils.c"
//#include "./folder.h"
#include "./file_with_stats_data_structure.h"
#include "./file_with_stats_list.h"
#include "./int_list.h"
#include <stdio.h> //print etc
#ifndef FOLDER_LIST_DATA_STRUCTURE_H
#define FOLDER_LIST_DATA_STRUCTURE_H
/*  File where we define the structure of the folderList with all the folder
 *  nodes
*/

// folderList of the folder nodes.
// It handles de-allocating nodes when they are removed.
//
// Methods:
//- constructorfolderListEmpty        -> Creates empty folderList
//- constructorfolderListOne          -> Creates folderList with one node
//(passed in
//                                    params)
//- destructorfolderList              -> Destroys the folderList (also
//de-allocates
//                                    nodes)
//- isEmpty                     -> If folderList is empty or not
//- append                      -> appends new node to the end of the folderList
//- getNodeByIndex              -> returns reference to node with given index
//- getNodeWithPath             -> returns reference to node with given path
//- removeElementByPath         -> Remove (and de-allocates) node with given
//                                 filePath
//- removeFirst                 -> Remove (and de-allocates) first node
//- removeLast                  -> Remove (and de-allocates) last node
//- updateFileData {}             -> Adds new stats to the relative folder
//                                 (if not present appends a new node)
//- removeLast                  -> Remove (and de-allocates) last node
//- printfolderList                   -> prints the folderList

struct folder_t;

typedef struct {
  // pointer to the first node
  struct folder_t *firstNode;
  // how many nodes are currently in the fwsfolderList
  int count;
} folderList;


typedef struct folder_t {
  fwsList *listaFile;
  folderList *sottocartelle;
  char *name;

  struct folder_t *nextNode;
  struct folder_t *previousNode;
} folder;

//FUNZIONI PER LA LISTA

// Returns reference to new empty folderList
folderList *constructorFolderListEmpty();
//  Returns reference to new folderList w/ just one item - TESTED✔️
folderList *constructorFolderListOne(folder *fs);
// dels l and everything within, de-allocating what's needed
void destructorFolderList(folderList *l);
// Returns wether the folderList is empty or not
bool folderListIsEmpty(folderList *l);
// Appends new node to end of folderList - TESTED✔️
void folderListAppend(folderList *l, folder *fs);
// Returns node in the given position (starting from 0) Returns NULL if not
// found
folder *folderListGetElementByIndex(folderList *l, int index);
// Returns reference to file  w/ given path Returns NULL if not found
folder *folderListGetElementByName(folderList *l, char *path);
// returns a reference to folder if the folder name matches the first part of the path
// folder *folderListGetElementByPartialPath(folderList *l, char *path) ;
// // return reference to file w/ give id Returns NULL if not found
// folder *folderListGetElementByID(folderList *folderList, uint id);
// Removes first element from the folderfolderList.
void folderListRemoveFirst(folderList *l);
// Removes the last element from the folderList.
void folderListRemoveLast(folderList *folderList);
// Adds the stats from newData to the right file in this folderList. If file is
// not
// present it is ignored.
// void folderListUpdateFileData(folderList *l, uint id, uint charTot,
//                               uint charsToAdd, uint *occourrrences);
// // Adds new chars to file path. IF file with this id is not present, it is
// // ignored
// void folderListUpdateFilePath(folderList *folderList, uint id, char *path);
// // remove element with given id from folderList, del true if deletion of said
// // node id necessary. Does nothing if element is not found
void folderListRemoveElementByID(folderList *folderList, uint id, bool del);
// remove element with given PATH from folderList, del true if deletion of
// said
// node id necessary. Does nothing if element is not found
void folderListRemoveElementByName(folderList *folderList, char *path,
                                   bool del);

void folderListPrint(folderList *l);





// FUNZIONI PER LE CARTELLA

folder *constructorFolder(char *name);
void destructorFolder(folder *cartella);
// bool isInthisFolder(folder *cartella, char *path);
// void cartellaAddFile(folder *cartella, fileWithStats *newFile,char *path);
void folderPrint(folder * f);
// funzione che controlla che i due path siano indentici fino al primo /, ovvero appartengano alla stessa cartella
bool equalFirstFolder(char * path1, char* path2);
//IMPLEMENTAZIONI LISTA

folderList *constructorFolderListEmpty() {
  folderList *l = (folderList *)malloc(sizeof(folderList));
  l->count = 0;
  l->firstNode = NULL;
}

folderList *constructorFolderListOne(folder *fs) {
  folderList *l = constructorFolderListEmpty();
  folderListAppend(l, fs);
  return l;
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

folder *folderListGetElementByIndex(folderList *l, int index) {
  folder *result = NULL;
  if (index >= 0 && index < l->count) {
    result = l->firstNode;
    int i;
    for (i = 0; i < index; i++) {
      result = result->nextNode;
    }
  }
  return result;
}

folder *folderListGetElementByName(folderList *l, char *name) {
  folder *current = l->firstNode;
  while (current != NULL) {
    if (streq(current->name, name)) {
      return current;
    }
    current = current->nextNode;
  }
  // not found
  return NULL;
}
// folder *folderListGetElementByPartialPath(folderList *l, char *path) {
//   folder *current = l->firstNode;
//   while (current != NULL) {
//     if (equalFirstFolder(current->name,path)) {
//       return current;
//     }
//     current = current->nextNode;
//   }
//   // not found
//   return NULL;
// }
/*
folder *folderListGetElementByID(folderList *l, uint id) {
  folder *current = l->firstNode;
  while (current != NULL) {
    if (current->id == id) {
      return current;
    }
    current = current->nextNode;
  }
  // not found
  return NULL;
}
*/
void folderListRemoveFirst(folderList *l) {
  if (!folderListIsEmpty(l)) {
    folder *newFirstNode = l->firstNode->nextNode;
    destructorFolder(l->firstNode);
    l->firstNode = newFirstNode;
    l->count--;
  }
}

void folderListRemoveLast(folderList *l) {
  if (!folderListIsEmpty(l)) {
    folder *cursor = l->firstNode;
    // Get cursor to the last folderList node
    while (cursor->nextNode != NULL) {
      cursor = cursor->nextNode;
    }
    // Detach from the folderList
    folder *penultimate = cursor->previousNode;
    if (penultimate != NULL)
      penultimate->nextNode = NULL;
    // del
    destructorFolder(cursor);
    l->count--;
  }
}

// void folderListUpdateFileData(folderList *l, uint id, uint charTot, uint
// charsRead,
//                            uint *occourrrences) {
//   folder *target = folderListGetElementByID(l, id);
//   if (target != NULL) {
//     // if already exists
//     fwsUpdateFileData(target, charTot, charsRead, occourrrences);
//   }
//   /*
//   else{
//     // if does not exists
//     folder *newNode =
//   constructorFWS(path,id,charTot,occourrrences,fromFolder);
//     folderListAppend(l,newNode);
//   }
//   */
// }

// void folderListUpdateFilePath(folderList *l, uint id, char *path) {
//   folder *target = folderListGetElementByID(l, id);
//   if (target != NULL) {
//     fwsUpdateFilePath(target, path);
//   }
// }

// void folderListRemoveElementByID(folderList *l, uint id, bool del) {
//   folder *target = folderListGetElementByID(l, id);
//   if (target != NULL) {
//     folder *prev = target->previousNode;
//     folder *next = target->nextNode;
//     if (del)
//       destructorFWS(target);

//     // se l'elemento rimosso è in testa devo cambiare anche il puntatore di
//     // lista
//     if (prev != NULL)
//       prev->nextNode = next;
//     else
//       l->firstNode = next;
//     if (next != NULL)
//       next->previousNode = prev;
//     l->count--;
//   }
// }
//

void folderListRemoveElementByName(folderList *l, char *name, bool del) {
  folder *target = folderListGetElementByName(l, name);
  if (target != NULL) {
    folder *prev = target->previousNode;
    folder *next = target->nextNode;
    if (del)
      destructorFolder(target);
    if (prev != NULL)
      prev->nextNode = next;
    else
      l->firstNode = next;
    if (next != NULL)
      next->previousNode = prev;
    l->count--;
  }
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


//IMPLEMENTAZIONI CARTELLA

folder *constructorFolder(char *name) {
  folder *res = (folder *)malloc(sizeof(folder));
  res->listaFile = constructorFwsListEmpty();
  res->sottocartelle = constructorFolderListEmpty();
  res->name = malloc(sizeof(char)*strlen(name)+1); 
  strcpy(res->name,name);
  res->nextNode=NULL;
  res->previousNode=NULL;
}

void destructorFolder(folder *cartella) {
  destructorFwsList(cartella->listaFile);
  if (cartella->sottocartelle != NULL) {
    destructorFolderList(cartella->sottocartelle);
  }
  free(cartella);
}

char * ithFolder(char* path, int i){
  int j=0;
  int cursor=1;
  while(j<i){
    if(path[cursor]=='/') j++;
    if(j==i) {

    }
    cursor++;
  }
}
// estrae il primo nome di cartella
char * firstFolder(char * path){
  int j=0;
  if(path[j]=='/') j++;
  int length = 0;
  while(path[j]!='\0' && path[j]!='/'){
    j++; length++;
  }
  char * name = malloc(sizeof(char)*length+1);
  j=0;
  int i=0;
  if(path[j]=='/') j++;
  while(i<length){
    name[i]=path[j];
    i++;
    j++;
  }
  name[i]='\0';

  return name;
}
int countLettersFirstFolder(char* path){
  int j=0;
  if(path[j]=='/') j++;
  while(path[j]!='\0' && path[j]!='/'){
    j++;
  }
  return j;
}
int countFolders(char * path){
  int j=0;
  if(path[j]=='/') j++;
  int count = 0;
  while(path[j]!='\0'){
    if(path[j]=='/') count++;
    j++;
  }
  return count;
}
char * lastFolder(char * path){
  int last = countFolders(path);
  return ithFolder(path,last-1);
}

bool equalFirstFolder(char * path1, char* path2){
  int i = 0;
  int j = 0;
  if(path1[i]=='/') i++;
  if(path2[j]=='/') j++;
  int a = i+1;
  int b = j+1;
  while(path1[a]!='/') a++;
  while(path2[b]!='/') b++;
  bool equals = true;
  while(i<a && j<b){
    if(path1[i] != path2[j] ) equals = false;
    i++; j++;
  }
  return equals;
}
void cartellaAddFile(folder *cartella, fileWithStats *newFile,char * path) {
  // controllo  se appartiene direttamente alla cartella corrente se sì aggiungo qui? no potenzialmente ho anche cartelle da costruire!
  //prendo il nome della prima cartella
  // printf("cartella : %s\n",cartella->name);
  // printf("path rimanente : %s\n",path);
  char *primaCartella = firstFolder(path);
  // printf("prima cartella %s\n",primaCartella);
  bool isthisFolder = streq(cartella->name,primaCartella);
  // se si tratta di un file da aggiungere qui
  if(countFolders(path)<=1){
    fwsListAppend(cartella->listaFile,newFile);
  }
  //altrimenti controllo se appartiene ad una delle sottocartelle
  else {
    char *secondaCartella = firstFolder(path+countLettersFirstFolder(path));
    folder *nested = folderListGetElementByName(cartella->sottocartelle,secondaCartella);
    if(nested==NULL){
       //aggiungo una cartella alla lista
      nested = constructorFolder(secondaCartella);
      folderListAppend(cartella->sottocartelle,nested);
    }
    free(secondaCartella);
    cartellaAddFile(nested,newFile,path+countLettersFirstFolder(path));
  }
  free(primaCartella);
}
void folderPrint(folder * f){
  char *name;
  printf("folder name: %s\n",f->name);
  fwsListPrint(f->listaFile);
  folderListPrint(f->sottocartelle);
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

  folder * cartellaTest = constructorFolder("root");
  fileWithStats *a = constructorFWS("/root/a.txt",1,0,NULL,true);
  fileWithStats *f = constructorFWS("/root/f.txt",1,0,NULL,true);
  fileWithStats *b = constructorFWS("/root/patate/b.txt",1,0,NULL,true);
  fileWithStats *e = constructorFWS("root/patate/c.txt",1,0,NULL,true);
  fileWithStats *d = constructorFWS("root/d.txt",1,0,NULL,true);
  cartellaAddFile(cartellaTest,a,"/root/a.txt");
  cartellaAddFile(cartellaTest,f,"/root/f.txt");
  cartellaAddFile(cartellaTest,b,"/root/patate/b.txt");
   cartellaAddFile(cartellaTest,e,"root/patate/c.txt");
  cartellaAddFile(cartellaTest,d,"root/d.txt");
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
