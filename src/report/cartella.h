#include "../utils.c"
#include "./file_with_stats_data_structure.h"
#include "./file_with_stats_list.h"
#include "./folder_list.h"
#include "./int_list.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef DOUBLE_LIST_H
#define DOUBLE_LIST_H

typedef struct {
  fwsList *listaFile;
  folder_list *sottocartelle;
  // listacartelle * sottocartelle ?
  char *name;
} folder;

folder *constructorCartella(char *name) {
  folder *res = (folder *)malloc(sizeof(folder));
  res->listaFile = constructorFwsListEmpty();
  res->sottocartelle = NULL;
  res->name = name;
}

void destructorCartella(folder *cartella) {
  destructorFwsList(cartella->listaFile);
  if (cartella->sottocartelle != NULL) {
    destructorCartella(cartella->sottocartelle);
  }
  free(cartella);
}

string isInthisFolder(folder *cartella, char *path, int ricorsione) {
  intList *posizioniBarre = constructorIntListEmpty();
  int i = 0;
  while (path[i] != '\0') {
    if (path[i] == '/') {
      intListAppend(posizioniBarre, i);
    }
    i++;
  }
  int penultimaBarra, ultimaBarra;
  intNode *cur = posizioniBarre->firstNode;
  for (i = 0; i < ricorsione; i++) {
    cur = cur->nextNode;
  }
  // dir/ciao/a.txt
  penultimaBarra = cur->value;
  ultimaBarra = cur->nextNode->value;
  int lunghNomeUltimaCartella = ultimaBarra - penultimaBarra;
  char ultimaCartella[lunghNomeUltimaCartella + 1];
  for (i = 0; i < lunghNomeUltimaCartella; i++) {
    ultimaCartella[i] = path[ultimaBarra + 1 + i];
  }
  ultimaCartella[lunghNomeUltimaCartella] = '\0';
  destructorIntList(posizioniBarre);
  if (streq(ultimaCartella, cartella->nome)) {
    return '\0';
  } else
    return ultimaCartella;
}

void cartellaAddFile(folder *cartella, fileWithStats *newFile,
                     int livelloRicorsione) {
  string s = isInthisFolder(cartella, newFile->path, livelloRicorsione);
  if (s == '\0') {
    // aggiungi a questi files
    fwsListAppend(cartella->listaFile, newFile);
  } else {
    // Cerca se è in una sottocartella di questa
    folder *alreadyHere =
        folderListGetElementByName(folderList * l, char *path);
    if (alreadyHere != NULL) {
      // we already have that subfolder
      cartellaAddFile(alreadyHere, newFile, livelloRicorsione + 1);
    } else {
      // create subfolder & add file
      folder *newF = constructorCartella(s);
      folderListAppend(cartella->sottocartelle, newFile);
    }
  }
}

#endif
