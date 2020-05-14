#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1


typedef struct{
  pid_t pid;
  int fileId;
  int pipeToQ[2];
  int currM;
} miniQinfo;


typedef struct Node {
    miniQinfo *data;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct {
    struct Node *first;
    struct Node *last;
    int counter;
} miniQlist;


/**
 * Returns a pointer to a new instance of miniQinfo
 */
miniQinfo *constructorMiniQinfo(pid_t thisPid, int fileIdAssigned, int pipe[2], int currentM){
    miniQinfo *miniQ = (miniQinfo *)malloc(sizeof(miniQinfo));
    miniQ->pid = thisPid;
    miniQ->fileId = fileIdAssigned;
    miniQ->pipeToQ[0] = pipe[0];
    miniQ->pipeToQ[1] = pipe[1];
    miniQ->currM = currentM;

    return miniQ;
}

void printMiniQinfo(miniQinfo *miniQ){
    printf("miniQ has pid: %3d, fileId: %5d,\n", miniQ->pid, miniQ->fileId);
    printf("current M: %3d, pipe[0]: %3d, pipe[1]: %3d\n", miniQ->currM, miniQ->pipeToQ[0], miniQ->pipeToQ[1]);
    printf("====================\n");
}

void deleteMiniQinfo(miniQinfo *miniQ){
    free(miniQ);
}

/**
 * Returns a pointer to a Node with the miniQ inside, it has next and prev fields
 * initialized to NULL;
 */
Node *constructorNode(miniQinfo *miniQ){
    Node *node = (Node *)malloc(sizeof(miniQinfo));
    node->data = miniQ;
    node->next = node->prev = NULL;
}

void printNode(Node *node){
    // printf("Node: %p\n", node);
    printMiniQinfo(node->data);
}

void deleteNode(Node *node){
    deleteMiniQinfo(node->data);
    free(node);
}

/**
 * Returns a pointer to an empty list of miniQinfo elements
 */
miniQlist *constructorMiniQlist(){
    miniQlist *list = (miniQlist *)malloc(sizeof(miniQlist));
    list->first = NULL;
    list->last = NULL;
    list->counter = 0;

    return list;
}

void printMiniQlist(miniQlist *list){
    if(list->counter == 0){
        printf("miniQlist is empty!\n");
    } else {
        struct Node *element = list->first; 
        int i;
        for(i=0; i<list->counter; i++){
            printf("Node: %3d \n", i);
            printNode(element);
            element = element->next;
        }
    }
}

void deleteMiniQlist(miniQlist *list){
    if(list->counter > 0){
        struct Node *element = list->first; 
        int i;
        for(i=0; i<list->counter; i++){
            deleteNode(element);
            element = element->next;
        }
    }
    free(list);
}

/**
 * Append a Node to the list 
 */
void append(miniQlist *list, struct Node *newNode){
    newNode->prev = list->last;
    newNode->next = NULL;
    if(list->counter > 0){
        list->last->next = newNode;
    } else {
        list->first = newNode;
    }
    
    list->last = newNode;
    list->counter++;
}

/**
 * Returns the Node given the pid of miniQ
 * returns NULL if there's no miniQinfo with that pid
 */
Node *getNodeByPid(miniQlist *list, pid_t pid){ 
    struct Node *node = list->first;
    int i;
    
    for(i=0; i<list->counter; i++) {
        if(node->data->pid == pid){
            return node;
        }
        node = node->next;
    }

    return NULL;
}

/**
 * Returns the Node given the id of the file assigned
 * returns NULL if there's no miniQinfo with that fileId
 */
Node *getNodeByFileId(miniQlist *list, int fileId){
    struct Node *node = list->first;
    int i;
    
    for(i=0; i<list->counter; i++) {
        if(node->data->fileId == fileId){
            return node;
        }
        node = node->next;
    }

    return NULL;
}

/**
 * Removes a Node given the pid of the miniQ it contains
 */
void removeByPid(miniQlist *list, pid_t pid){
    Node *toRemove = getNodeByPid(list, pid);
    
    if(toRemove == NULL){
        printf("No node found with pid %3d\n", pid);
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
        list->counter--;
    }
}

/**
 * Removes a Node given the fileId of the miniQinfo it countains
 * it returns the pid of the miniQinfo deleted or -1 if there
 * was not a miniQinfo eith that fileId assigned
 */
pid_t removeByFileId(miniQlist *list, int fileId){
    Node *toRemove = getNodeByFileId(list, fileId);
    
    if(toRemove == NULL){
        printf("No node found with fileId %3d\n", fileId);
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
        pid_t deleted_pid = toRemove->data->pid;
        deleteNode(toRemove);
        list->counter--;
        return deleted_pid;
    }
    return -1;
}

/**
int main(){
    int pipe[2] = {0,1};
    
    miniQinfo *miniQ = constructorMiniQinfo(11, 2, pipe, 7);
    // printMiniQinfo(miniQ);
    miniQinfo *miniQ1 = constructorMiniQinfo(12, 3, pipe, 8);

    Node *nodo = constructorNode(miniQ);
    Node *nodo1 = constructorNode(miniQ1);
    // printNode(nodo);
    // printNode(nodo1);

    miniQlist *list = constructorMiniQlist();
    append(list, nodo);
    append(list, nodo1);

    // removeByPid(list, 12);
    // removeByPid(list, 11);
    // removeByPid(list, 11);
    // removeByFileId(list, 3);
    // removeByFileId(list, 2);
    // removeByFileId(list, 3);
    

    printMiniQlist(list);

    return 0;
}
*/