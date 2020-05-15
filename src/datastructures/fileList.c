#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "../utils.c"

#define READ 0
#define WRITE 1

typedef struct {
  char *fileName;
  int  idFile;
  int  numOfRemainingPortionsToRead;
  int  pIndex;
} fileState;

typedef struct Node {
    fileState *data;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct FileNameList {
    struct Node *first;
    struct Node *last;
    long long int number_of_nodes;
} FileNameList;

/**
 * Create a new fileState, returns the pointer
 */
fileState *constructorFileState(char *fileName, int fileId, int portions, int indexOfP){
    fileState *newFileState = (fileState *)malloc(sizeof(fileState));
    newFileState->fileName = (char *)malloc(sizeof(fileName));
    strcpy(newFileState->fileName, fileName);
    newFileState->idFile = fileId;
    newFileState->numOfRemainingPortionsToRead = portions;
    newFileState->pIndex = indexOfP;

    return newFileState;
}

void printFileState(fileState *state){
    printf("FileState id: %3d, File name: %s\n", state->idFile, state->fileName);
    printf("Remaining portions: %3d, index of P: %3d\n", state->numOfRemainingPortionsToRead, state->pIndex);
    printf("================\n");
}

void deleteFileState(fileState *state){
    free(state->fileName);
    free(state);
}


/**
 * Create a new fileNode (given a filestate)
 * next and prev will be added at the moment of appending the node in the list
 */
Node *constructorFileNode(fileState *newFileState){
    Node *newNode = (Node *) malloc(sizeof(Node));
    newNode->data = newFileState;
    newNode->prev = newNode->next = NULL;

    return newNode;
}

void printNode(Node *node){
    // printf("Node: %p\n", node);
    printFileState(node->data);
    print("==================");
}

void deleteNode(Node *node){
    deleteFileState(node->data);
    free(node);
    printf("node destroied\n");
}


/**
 * Create a new empty fileList
 */
FileNameList *constructorFileNameList(FileNameList *list){
    list = (FileNameList *)malloc(sizeof(FileNameList));
    list->first = NULL;
    list->last = NULL;
    list->number_of_nodes = 0;

    return list;
}

void printList(FileNameList *list) {
    if(list->number_of_nodes==0){
        printf("List is empty!\n");
        return;
    }

    struct Node *node = list->first;
    int i;
    for(i=0; i<list->number_of_nodes; i++) {
        printf("Node: %3d\n", i);
        printNode(node);
        node = node->next;
    }
}

void deleteList(FileNameList *list){
    Node *node = list->first;
    Node *next;
    int i;
    for(i=0; i<list->number_of_nodes; i++){
        next = node->next;
        deleteNode(node);
        node = next;
    }
    free(list);
}


/**
 * Append a Node to the list 
 */
void *append(FileNameList *list, struct Node *newNode){
    newNode->prev = list->last;
    newNode->next = NULL;
    if(list->number_of_nodes>0){
        list->last->next = newNode;
    } else {
        list->first = newNode;
    }
    
    list->last = newNode;
    list->number_of_nodes++;
}

void appendName(){
    
}

/**
 * Returns the first Node with a file with the argument name
 * returns NULL if there's no file with that name
 */
Node *getNodeByName(FileNameList *list, char *nodeName){  
    struct Node *node = list->first;
    int i;
    
    for(i=0; i<list->number_of_nodes; i++) {
        
        if(strcmp(nodeName, node->data->fileName) == 0){
            return node;
        }
        node = node->next;
    }

    return NULL;
}

/**
 * Returns the Node given the id of the fileState
 * returns NULL if there's no fileState with that id
 * returns the only first Node if more nodes have that id
 */
Node *getNodeById(FileNameList *list, int id){
    struct Node *node = list->first;
    int i;
    
    for(i=0; i<list->number_of_nodes; i++) {
        
        if(node->data->idFile == id){
            return node;
        }
        node = node->next;
    }

    return NULL;
}

/**
 * Removes a node given the name of the file.
 * If there's no node with that name it does noting.
 * If there are more nodes with that name it deletes the first one.
 */
void removeNode(FileNameList *list, char *name){
    Node *toRemove = getNodeByName(list, name);
    
    if(toRemove == NULL){
        printf("No node found with name %s\n", name);
    } else {
        if(list->first == toRemove){
            list->first = toRemove->next;
            if(list->first != NULL)list->first->prev = NULL;
        } else if(list->last == toRemove) {
            list->last = toRemove->prev;
            if(list->last != NULL) list->last->next = NULL;
        } else {
            toRemove->prev->next = toRemove->next;
            toRemove->next->prev = toRemove->prev;
            
        }
        deleteNode(toRemove);
        list->number_of_nodes--;
    }
}

/**
 * Decrements the value of numOfRemainingPortionsToRead of the first file with id idFile
 * returns the new value of numOfRemainingPortionsToRead, or -1 in the case that the 
 * file was not found
 */
int decrementRemainingPortionsById(FileNameList *list, int idFile){
    int newValue=-1;
    Node *node = getNodeById(list, idFile);
    if(node==NULL){
        printf("Cannot decrement, idFile not found\n");
    } else {
        newValue = --node->data->numOfRemainingPortionsToRead;
        if(newValue < 0){
            printf("ATTENZIONE! numOfRemainingPortionsToRead del file: \"%s\"", node->data->fileName);
            printf(" e sceso sotto zero!!!\n");
        } 
    }

    return newValue;
}

/**
 * Returns true if numOfRemainingPortionsToRead is equal to 0 for every file,
 * false otherwise.
 */
bool isAnalisiFinita(FileNameList *list){
    bool finished = true;
    Node *element = list->first;
    int i;

    for(i=0; i<list->number_of_nodes; i++){
        if(element->data->numOfRemainingPortionsToRead > 0) finished=false;
        if(!finished) break;
        element = element->next;
    }

    return finished;
}

void assignFileTo(FileNameList *list, int idFile, int portions){
    // TODO: implementare/capire questa funzione
    // A che cosa mi serve qui il numOfRemainingPortions se l'ho già settato nel fileState?'
}


/**
int main(){
    fileState *file = constructorFileState("prova1", 2, 1, 4);
    fileState *file2 = constructorFileState("prova2", 3, 1, 4);
//    printFileState(file);

    Node *nodo = constructorFileNode(file);
    Node *nodo2 = constructorFileNode(file2);
//    printNode(nodo);

    FileNameList *lista = constructorFileNameList(lista);
    lista = append(lista, nodo);
    lista = append(lista, nodo2);

    // removeNode(lista, "prova2");
    // removeNode(lista, "prova2");
    // decrementRemainingPortionsById(lista, 2);
    // decrementRemainingPortionsById(lista, 2);
    // decrementRemainingPortionsById(lista, 3);
    // if(isAnalisiFinita(lista)){
    //     printf("FINE\n");
    // }

    printList(lista);

    // Node *trovato = getNodeById(lista, 2);
    // if(trovato == NULL){
    //     printf("Nodo non trovato\n");
    // } else {
    //     printf("Id trovato: %d\n", trovato->data->idFile);
    // }
    
    // removeNode(lista, "prova");
    // deleteList(lista);


    // int newValue = decrementRemainingById(lista, 5);
    // printf("NewValue: %d\n", newValue);



    return 0;
}
*/