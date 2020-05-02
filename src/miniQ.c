#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.c"

// Attention! I am making the dangerous assumption that id of miniQ will be from 1 to m (from 1 not 0!)

int getFileLength(string fileName){
    // TODO: is an int enough for file length?
    FILE *fp = fopen(fileName, "r");
    fseek(fp, 0L, SEEK_END);
    int length = ftell(fp);
    return length;
}

int getStartPosition(int fileLength, int nrOfPortions, int thisPIndex){
    if(thisPIndex==1){
        return 0;
    }
    int start;
    int lengthOfPortion = (int) ceil((double)(fileLength)/(double)(nrOfPortions));
    printf("lengthOfPortion=%d\n", lengthOfPortion);
    start = (lengthOfPortion*(thisPIndex-1))+1;
    printf("start=%d\n", start);
    return start;
}

int getEndPosition(int fileLength, int nrOfPortions, int thisPIndex){
    if(thisPIndex==nrOfPortions){
        return fileLength;
    }
    int end;
    int lengthOfPortion = (int) ceil((double)(fileLength)/(double)(nrOfPortions));
    printf("lengthOfPortion=%d\n", lengthOfPortion);
    end = (lengthOfPortion*(thisPIndex));
    printf("start=%d\n", end);
    return end;
}

void miniQ(){
    // Fra, to try P out create the file filediprova.txt with some random text in it
    string fileName="./fileDiProva.txt";
    // m -> nrOfPortions
    int nrOfPortions = 4;
    // index of the portion of the file to analyze
    int thisPIndex = 2; 

    int fileLength = getFileLength(fileName);
    int start = getStartPosition(fileLength, nrOfPortions, thisPIndex);
    int end = getEndPosition(fileLength, nrOfPortions, thisPIndex);

    // Now I have the start position and the end position, let's read the file
}


int main(){
    miniQ();
    return 0;
}