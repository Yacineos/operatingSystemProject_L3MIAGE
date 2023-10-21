#ifndef PROJETSYS_H
#define PROJETSYS_H

typedef struct _DataPipes {
    int operationToExecute ;
    int key;
    char val[128];
} DataPipes ;

typedef DataPipes *PDataPipes ;

void affichageData(DataPipes data);
// Description: Header file for projetSys.c

void exitProgramme(DataPipes data,int (*pipes)[2],int* mainPipe,int processNumber);
void setData(DataPipes data ,int pipe[2] , int mainPipe[2] );
void lookupData(DataPipes data,int* pipe , int* mainPipe );
void dumpDictionnaire(DataPipes data,int (*pipes)[2],int* mainPipe,int processNumber);
void closeUnusedPipes(int (*pipes)[2],int processNumber,int currentNode);
void controller(DataPipes data,int (*pipes)[2],int* mainPipe,int processNumber);
#endif