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
} FileState;

typedef struct NodeFileState {
    FileState *data;
    struct NodeFileState *next;
    struct NodeFileState *prev;
} NodeFileState;

typedef struct FileList {
    struct NodeFileState *first;
    struct NodeFileState *last;
    long long int number_of_nodes;
} FileList;

/**
 * Create a new FileState, returns the pointer
 */
FileState *constructorFileState(char *fileName, int fileId, int portions, int indexOfP){
    FileState *newFileState = (FileState *)malloc(sizeof(FileState));
    newFileState->fileName = (char *)malloc(strlen(fileName)+1);
    strcpy(newFileState->fileName, fileName);
    newFileState->idFile = fileId;
    newFileState->numOfRemainingPortionsToRead = portions;
    newFileState->pIndex = indexOfP;

    return newFileState;
}

void printFileState(FileState *state){
    printf("FileState id: %3d, File name: %s\n", state->idFile, state->fileName);
    printf("Remaining portions: %3d, index of P: %3d\n", state->numOfRemainingPortionsToRead, state->pIndex);
    printf("================\n");
}

void deleteFileState(FileState *state){
    free(state->fileName);
    free(state);
}


/**
 * Create a new fileNode (given a filestate)
 * next and prev will be added at the moment of appending the node in the list
 */
NodeFileState *constructorFileNode(FileState *newFileState){
    NodeFileState *newNode = (NodeFileState *) malloc(sizeof(NodeFileState));
    newNode->data = newFileState;
    newNode->prev = newNode->next = NULL;

    return newNode;
}

void printNode(NodeFileState *node){
    // printf("NodeFileState: %p\n", node);
    printFileState(node->data);
    printf("==================");
}

void deleteNode(NodeFileState *node){
    deleteFileState(node->data);
    free(node);
    // printf("node destroyed\n");
}


/**
 * Create a new empty fileList
 */
FileList *constructorFileNameList(){
    FileList *list = (FileList *) malloc(sizeof(FileList));
    list->first = NULL;
    list->last = NULL;
    list->number_of_nodes = 0;

    return list;
}

void printList(FileList *list) {
    if(list->number_of_nodes==0){
        printf("List is empty!\n");
        return;
    }

    struct NodeFileState *node = list->first;
    int i;
    for(i=0; i<list->number_of_nodes; i++) {
        printf("NodeFileState: %3d\n", i);
        printNode(node);
        node = node->next;
    }
    printf("\n");
}

void deleteList(FileList *list){
    NodeFileState *node = list->first;
    NodeFileState *next;
    int i;
    for (i=0; i<list->number_of_nodes; i++){
        next = node->next;
        deleteNode(node);
        node = next;
    }
    free(list);
}


/**
 * Append a NodeFileState to the list 
 */
void *appendFileState(FileList *list, struct NodeFileState *newNode){
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

/**
 * Returns the first NodeFileState with a file with the argument name
 * returns NULL if there's no file with that name
 */
NodeFileState *getNodeByName(FileList *list, char *nodeName){  
    struct NodeFileState *node = list->first;
    int i;
    
    for (i = 0; i<list->number_of_nodes; i++) {
        
        if(strcmp(nodeName, node->data->fileName) == 0){
            return node;
        }
        node = node->next;
    }

    return NULL;
}

/**
 * Returns the NodeFileState given the id of the FileState
 * returns NULL if there's no FileState with that id
 * returns the only first NodeFileState if more nodes have that id
 */
NodeFileState *getNodeById(FileList *list, int id){
    struct NodeFileState *node = list->first;
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
int removeNode(FileList *list, char *name){
    int idRemovedFile = -1;
    NodeFileState *toRemove = getNodeByName(list, name);
    
    if(toRemove == NULL){
        printf("No node found with name %s\n", name);
    } else {
        idRemovedFile = toRemove->data->idFile;
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

    return idRemovedFile;
}

/**
 * Decrements the value of numOfRemainingPortionsToRead of the first file with id idFile
 * Returns the new value of numOfRemainingPortionsToRead, or -1 in the case that the 
 * file was not found
 */
int decrementRemainingPortionsById(FileList *list, int idFile){
    int newValue=-1;
    NodeFileState *node = getNodeById(list, idFile);
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
bool isAnalisiFinita(FileList *list){
    bool finished = true;
    NodeFileState *element = list->first;
    int i;

    for(i=0; i<list->number_of_nodes; i++){
        if(element->data->numOfRemainingPortionsToRead > 0) finished=false;
        if(!finished) break;
        element = element->next;
    }

    return finished;
}

void assignFileTo(FileList *list, int idFile, int portions){
    // TODO: implementare/capire questa funzione
    // A che cosa mi serve qui il numOfRemainingPortions se l'ho già settato nel FileState?'
}


/**
int main(){
    FileState *file = constructorFileState("prova1", 2, 1, 4);
    FileState *file2 = constructorFileState("prova2", 3, 1, 4);
//    printFileState(file);

    NodeFileState *nodo = constructorFileNode(file);
    NodeFileState *nodo2 = constructorFileNode(file2);
//    printNode(nodo);

    FileList *lista = constructorFileNameList(lista);
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

    // NodeFileState *trovato = getNodeById(lista, 2);
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