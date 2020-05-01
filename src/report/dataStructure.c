#include "../utils.c" //nostro
#include "string.h" //strlen e strcpy
#include <stdio.h> //print etc
#include <stdlib.h> //contiene malloc e free

//struct per i singoli elementi
struct fileData{
  //dobbiamo fare una malloc ? YEP 
  char *path;
  int totalCharacters;
  int occorrenze[256];
  bool fromFolder;
  
  struct fileData *nextFile;
  struct fileData *previousFile;
};
typedef struct fileData listItem;

// funzione che mi inizializza il listItem TESTATO
void constructorListItem(listItem *fs,char *path,  int totalCharacters,  int occorrenze[256],  bool fromFolder){
  //dobbiamo fare una malloc ? sì perché altrimenti da degli errori e poi non ho la memoria dunque mi serve proprio! RICORDA LA FREE
  fs->path=malloc(strlen(path));
  strcpy(fs->path,path);
  fs->totalCharacters = totalCharacters;
  int i;
  for(i=0;i<256;i++){
    fs->occorrenze[i] = occorrenze[i];
  }
  fs->fromFolder = fromFolder;
  fs->previousFile = 0;
  fs->nextFile = 0;
}
// funzione che mi inizializza il listItem TESTATO
void constructorListItemComplete(listItem *fs,char *path,  int totalCharacters,  int occorrenze[256],  bool fromFolder, listItem * prev, listItem * next){
  //dobbiamo fare una malloc ? sì perché altrimenti da degli errori e poi non ho la memoria dunque mi serve proprio! RICORDA LA FREE
  fs->path=malloc(strlen(path));
  strcpy(fs->path,path);
  fs->totalCharacters = totalCharacters;
  int i;
  for(i=0;i<256;i++){
    fs->occorrenze[i] = occorrenze[i];
  }
  fs->fromFolder = fromFolder;
  fs->previousFile = prev;
  fs->nextFile = next;
}
//eliminazione, ancora da gestire tutta la parte dei potenziali errori per malloc e free SEMBRA FUNZIONARE, NECESSARI TEST ESTESI SULLA GESTIONE DELLA MEMORIA
void deleteListItem(listItem *fs){
    free(fs->path);
    free(fs);
}
//funzione di stampa, giusto per controllare i dati TESTATO
void printListItem(listItem *fs){
  printf("%s\n",fs->path);
  //lo tolgo solo per debug più veloce
  /*
  printf("%d\n",fs->totalCharacters);
    int i;
    for(i=0;i<256;i++){
      printf("%d\n",fs->occorrenze[i]);
    }
    */
  printf("%d\n",fs->fromFolder);
}

typedef struct{
  listItem *dataFile;
  int count;
}list;

//costruttore per coda vuota?
void constructorListEmpty(list *list){
  list->count=0;
}
//costruttore con 1 file ebbasta, forse serve TESTATO
void constructorListOne(list *list,listItem *fs){
  list->count=1;
  list->dataFile=fs;
}
//lista vuota come lista di 1 elemento senza dati
bool isEmpty(list *list){
  //soluzione facile che funziona a meno di imbranati che vanno a modificare che che non dovrebbero
  if(list->count==0)
    return true;
  else
    return false;
}
//aggiunge alla fine della lista. C'è sempre da controllare che non si tolga a liste vuote e altre robe del cacchio, ma spero che lo utilizziate in maniera intelligente TESTATO
void append(list *list, listItem * fs){
  //caso di coda vuota
  if(isEmpty(list)){
    list->dataFile=fs;
    //altrimenti
  }else{
    listItem * cursor = list->dataFile;
    while(cursor->nextFile!=0){
      cursor = cursor->nextFile;
    }
    cursor->nextFile = fs;
  }
  list->count++;
}
// remove ?  rimuovo l'ultimo elemento e basta, non lo ritorno sembra funzionare
void removeLast(list *list){
  if(!isEmpty(list)){
    listItem * cursor = list->dataFile;
    //caso con solo 1 elemetno
    if(list->count==1){
      deleteListItem(cursor);
      list->dataFile=0;
    }else {
      //raggiungo il penultimo elemento
      int i;
      for(i=0;i<list->count-2;i++){
        cursor = cursor->nextFile;
      }
      deleteListItem(cursor->nextFile);
      cursor->nextFile=0;
    }
    list->count--;
  }
}
//ritorna il primo elemento della coda/lista. Se la coda è vuota ritorno un ptr a null. TESTATO SEMBRA FUNZIONARE
listItem* getFirst(list *list){
  if(!isEmpty(list)){
    return list->dataFile;
  }
}
//rimozione del primo elemento
void removeFirst(list *list){
  if(!isEmpty(list)){
    listItem * next=0;
    //if per il caso di rimozione da una lista con 1 solo elemento
    if(list->dataFile!=0)
      next=list->dataFile->nextFile;
    deleteListItem(list->dataFile);
    list->dataFile=next;
    list->count--;
  }
}
// TODO TOGO
//stampa della lista completa TESTATO
void printList(list *list) {
  listItem * cursor = list->dataFile;
  if(!isEmpty(list)){
    while(cursor!=0){
      printListItem(cursor);
      cursor=cursor->nextFile;
    }
  }
}
//main di prova per testare
int main(int c, char *argv[]) {
  int retCode = 0;
  char* prova = "GATTO\0";
  listItem *fs = malloc(sizeof(listItem));
  int oc[256] = {1,2,3,4,5,6,7,8,9,0}; //non li metto tutti, sei matto
  constructorListItem(fs,prova,20,oc,true);
  printListItem(fs);
  prova = "deleted\0";
  oc[0]=-1;
  printListItem(fs);
 
  listItem *f1 = malloc(sizeof(listItem));
  char * testo1 = "EL1";
  constructorListItem(f1,testo1,20,oc,true);
  printListItem(f1);

  listItem *f2 = malloc(sizeof(listItem));
  char * testo2 = "EL2";
  constructorListItem(f2,testo2,20,oc,true);
  printListItem(f2);
  list * lista = malloc(sizeof(list));
  
  //constructorListOne(lista,f1);
  constructorListEmpty(lista);
  append(lista,f1);
  append(lista,f2);
  
  printList(lista);
  printf("%d\n",lista->count);
  //listItem *f3 = getFirst(lista);
  //printListItem(f3); 
  //removeLast(lista);
  //removeLast(lista);
  //removeLast(lista);
  removeFirst(lista);
  removeFirst(lista);
  removeFirst(lista);
  printList(lista);
  printf("elementi della lista %d\n",lista->count);
  return retCode;
}
