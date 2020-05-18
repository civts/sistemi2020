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
  int index;
} miniQinfo;

typedef struct NodeMiniQ {
    miniQinfo *data;
    struct NodeMiniQ *next;
    struct NodeMiniQ *prev;
} NodeMiniQ;

typedef struct {
    struct NodeMiniQ *first;
    struct NodeMiniQ *last;
    int counter;
} miniQlist;


/**
 * Returns a pointer to a new instance of miniQinfo
 */
miniQinfo* constructorMiniQinfo(pid_t thisPid, int fileIdAssigned, int pipe[2], int currentM, int index){
    miniQinfo *miniQ = (miniQinfo*) malloc(sizeof(miniQinfo));
    miniQ->pid = thisPid;
    miniQ->fileId = fileIdAssigned;
    miniQ->pipeToQ[0] = pipe[0];
    miniQ->pipeToQ[1] = pipe[1];
    miniQ->currM = currentM;
    miniQ->index = index;

    return miniQ;
}

void printMiniQinfo(miniQinfo *miniQ){
    printf("miniQ has pid: %3d, fileId: %5d,\n", miniQ->pid, miniQ->fileId);
    printf("current M: %3d, pipe[0,1]: %3d,%3d, index=%3d\n", miniQ->currM, miniQ->pipeToQ[0], miniQ->pipeToQ[1], miniQ->index);
    printf("====================\n");
}

void deleteMiniQinfo(miniQinfo *miniQ){
    // TODO - dobbiamo chiudere la pipe?
    free(miniQ);
}

/**
 * Returns a pointer to a Node with the miniQ inside, it has next and prev fields
 * initialized to NULL;
 */
NodeMiniQ *constructorNodeMiniQ(miniQinfo *miniQ){
    NodeMiniQ *node = (NodeMiniQ*) malloc(sizeof(NodeMiniQ));
    node->data = miniQ;
    node->next = node->prev = NULL;
}

void printNodeMiniQ(NodeMiniQ *node){
    // printf("Node: %p\n", node);
    printMiniQinfo(node->data);
}

void deleteNodeMiniQ(NodeMiniQ *node){
    deleteMiniQinfo(node->data);
    free(node);
}

/**
 * Returns a pointer to an empty list of miniQinfo elements
 */
miniQlist* constructorMiniQlist(){
    miniQlist *list = (miniQlist*) malloc(sizeof(miniQlist));
    list->first = NULL;
    list->last = NULL;
    list->counter = 0;

    return list;
}

void printMiniQlist(miniQlist *list){
    if(list->counter == 0){
        printf("miniQlist is empty!\n");
    } else {
        struct NodeMiniQ *element = list->first; 
        int i;
        for(i=0; i<list->counter; i++){
            printf("Node: %3d \n", i);
            printNodeMiniQ(element);
            element = element->next;
        }
    }
}

void deleteMiniQlist(miniQlist *list){
    if (list->counter > 0){
        struct NodeMiniQ *element = list->first;
        struct NodeMiniQ *nextElement;
        int i;
        for(i=0; i<list->counter; i++){
            nextElement = element->next;
            deleteNodeMiniQ(element);
            element = nextElement;
        }
    }
    free(list);
}

/**
 * Append a Node to the list 
 */
void appendMiniQ(miniQlist *list, struct NodeMiniQ *newNode){
    newNode->prev = list->last;
    newNode->next = NULL;
    if (list->counter > 0){
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
NodeMiniQ* getNodeMiniQByPid(miniQlist *list, pid_t pid){ 
    struct NodeMiniQ *node = list->first;
    int i;
    
    for(i = 0; i < list->counter; i++) {
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
NodeMiniQ* getNodeMiniQByFileId(miniQlist *list, int fileId){
    struct NodeMiniQ *node = list->first;
    int i;
    
    for(i = 0; i < list->counter; i++) {
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
void removeMiniQByPid(miniQlist *list, pid_t pid){
    NodeMiniQ *toRemove = getNodeMiniQByPid(list, pid);
    
    if (toRemove == NULL){
        printf("No node found with pid %3d\n", pid);
    } else {
        if (list->first == toRemove){
            list->first = toRemove->next;
            if (list->first != NULL){
                list->first->prev = NULL;
            }
        } else if (list->last == toRemove) {
            list->last = toRemove->prev;
            if (list->last != NULL){
                list->last->next = NULL;
            }
        } else {
            toRemove->prev->next = toRemove->next;
            toRemove->next->prev = toRemove->prev;
            
        }
        deleteNodeMiniQ(toRemove);
        list->counter--;
    }
}

/**
 * Removes a Node given the fileId of the miniQinfo it countains
 * it returns the pid of the miniQinfo deleted or -1 if there
 * was not a miniQinfo eith that fileId assigned
 */
pid_t removeMiniQByFileId(miniQlist *list, int fileId){
    pid_t result = -1;
    NodeMiniQ *toRemove = getNodeMiniQByFileId(list, fileId);
    
    if(toRemove == NULL){
        printf("No node found with fileId %3d\n", fileId);
    } else {
        if (list->first == toRemove){
            list->first = toRemove->next;
            if (list->first != NULL){
                list->first->prev = NULL;
            }
        } else if (list->last == toRemove) {
            list->last = toRemove->prev;
            if (list->last != NULL){
                list->last->next = NULL;
            }
        } else {
            toRemove->prev->next = toRemove->next;
            toRemove->next->prev = toRemove->prev;
        }

        pid_t deleted_pid = toRemove->data->pid;
        deleteNodeMiniQ(toRemove);
        list->counter--;
        result = deleted_pid;
    }
    return result;
}

/**
int main(){
    int pipe[2] = {0,1};
    
    miniQinfo *miniQ = constructorMiniQinfo(11, 2, pipe, 7);
    // printMiniQinfo(miniQ);
    miniQinfo *miniQ1 = constructorMiniQinfo(12, 3, pipe, 8);

    Node *nodo = constructorNodeMiniQ(miniQ);
    Node *nodo1 = constructorNodeMiniQ(miniQ1);
    // printNode(nodo);
    // printNode(nodo1);

    miniQlist *list = constructorMiniQlist();
    appendMiniQ(list, nodo);
    appendMiniQ(list, nodo1);

    // removeMiniQByPid(list, 12);
    // removeMiniQByPid(list, 11);
    // removeMiniQByPid(list, 11);
    // removeMiniQByFileId(list, 3);
    // removeMiniQByFileId(list, 2);
    // removeMiniQByFileId(list, 3);
    

    printMiniQlist(list);

    return 0;
}
*/