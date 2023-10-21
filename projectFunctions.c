//BINOMES :Oukkal Yacine N° étudiant: 12213245 || Monachon Vivian N° étudiant: 11907866

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "table.h"
#include "projetSys.h"
#define READ_END 0
#define WRITE_END 1

//fonction qui envoie simultanément à tous les processus fils data de type DataPipes
//pour leur dire de mourir et ensuite tue le processus appelant ( père )
// on passe tous les pipe en paramètre 
// on passe le tube du père aussi juste pour le fun
void exitProgramme(DataPipes data,int (*pipes)[2],int* mainPipe,int processNumber){
    //TODO
    printf("attention on va lancer exit \n");
    int operationStatus = 0;
    for(int i = 0 ; i < processNumber ; i++ ){
        write(pipes[i][WRITE_END],&data,sizeof(DataPipes));
        read(mainPipe[READ_END],&operationStatus, sizeof(int));
        if(operationStatus == 0 ){
            printf("le fils %d n'est pas mort \n", i);
        }else{
            printf("le fils %d est passée à la guillotine \n",i);
        }
        wait(NULL);
        
    }
        printf("bye bye \n");
        // on ferme le tube du père coté lecture 
        close(mainPipe[READ_END]);

}


//fonction qui envoie au node 0 data de type DataPipes 
// le processus fils s'occupera de gérer si c'est à lui d'ajouter le mot (data.val) dans 
// sa liste chaînée
// et met le père en mode lecture pour attendre un message succes du fils 
// si operationStatus == 0 erreur 
// else succes 
void setData(DataPipes data ,int* pipe , int* mainPipe ){
    int operationStatus = 0 ;
    write(pipe[WRITE_END],&data,sizeof(DataPipes));
    read(mainPipe[READ_END],&operationStatus,sizeof(int));
    if(operationStatus == 0){
        printf("erreur dans le set \n");
    }else{
        printf("l'ajout à bien été réalisé \n");
    }
}



//fonction qui envoie au node 0 data de type DataPipes 
// le processus fils s'occupe de vérifier si c'est à lui de retourner la valeur ou non 
// puis affiche cette valeur dans le terminal
void lookupData(DataPipes data,int* pipe , int* mainPipe ){
    //TODO
    char value[128] = "";
    write(pipe[WRITE_END],&data,sizeof(DataPipes));
    read(mainPipe[READ_END],&value,sizeof(char)*128);
    printf("la valeur de la clé est : %s \n",value);
}

// fonction dump permet de visualiser l'état actuelle du dictionnaire fragmenté entre les
// processus fils 
void dumpDictionnaire(DataPipes data,int (*pipes)[2],int* mainPipe,int processNumber){
    //TODO
    int operationStatus = 0 ;
    for(int i = 0 ; i < processNumber ; i++ ){
        write(pipes[i][WRITE_END],&data,sizeof(DataPipes));
        read(mainPipe[READ_END],&operationStatus, sizeof(int));
        if(operationStatus == 1 ){
            printf("le fils %d à affiché son dictionnaire \n", i);
        }else{
            printf("le fils %d n'a pas réussi à afficher son dictionnaire \n",i);
        }
    }
}


void closeUnusedPipes(int (*pipes)[2],int processNumber,int currentNode){
     for(int j = 0 ; j < processNumber ; j++){
                    // on est dans le dernier process ?
                    if(currentNode == processNumber-1){
                            // pour éviter de fermer la même chose 10000fois 
                            // mais normalement il fonctionne même sans le if  
                            if(j!=currentNode){
                                close(pipes[j][READ_END]);
                                close(pipes[j+1][WRITE_END]);
                            }else{
                                close(pipes[0][WRITE_END]);
                            }
                    }else{
                        // on est pas dans les tubes en relation avec le processus actuelle
                        if(j!=currentNode){
                            close(pipes[j][READ_END]);
                            close(pipes[j+1][WRITE_END]);
                        }
                        
                    }
     }
}

void controller(DataPipes data,int (*pipes)[2],int* mainPipe,int processNumber){
    //le père commence sa partie --------------------------------------------------------------------
    do{
        //preparation du struct ---------------------------------------------------------------------
        // lire les données saisie par l'utilisateurs et utilisation de do while pour controler l'input
        do{
            int buffer;
            printf("Saisir commande (0 = exit, 1 = set, 2 = lookup, 3 = dump):");

            if (scanf("%d", &buffer) == 1) {
                data.operationToExecute = buffer;
            }
            else {
                data.operationToExecute = 5;
                while (getchar() != '\n');
            }
            //scanf("%d",&data.operationToExecute);
            
        }while(data.operationToExecute <0 && data.operationToExecute > 3);
        
        /*switch case qui va bien -------------------------------------------------------------*/
        //si (0 appelle la fonction exit 
        //si (1) appelle la fonction set
        //si (2) appelle la fonction lookup
        //si (3) appelle la fonction dump
        switch(data.operationToExecute){
            case 0 :
                exitProgramme(data,pipes,mainPipe,processNumber); 
                break ;
            case 1 :
                do{
                    int buffer;
                    printf("Saisir la cle (decimal number):");
                    if (scanf("%d", &buffer) == 1) {
                        data.key = buffer;
                    }
                    else {
                        data.key = -1;
                        while (getchar() != '\n');
                    }
                    
                    //scanf("%d",&data.key);
                }while(data.key<0);
                do{
                    printf("Saisir la valeur (chaine de caracteres, max 128 chars):");
                    // scanf n'arrive pas à empecher l'utilisateur de ne pas dépasser 128 char donc on utilise fgets
                    scanf("%s",data.val);    
                }while(strlen(data.val) > 127);
                setData(data,pipes[0],mainPipe); 
                break;
            case 2 :
                do{
                    printf("Saisir la cle (decimal number):");
                    scanf("%d",&data.key);
                }while(data.key<0);
                lookupData(data,pipes[0],mainPipe); 
                break;
            case 3 :
                dumpDictionnaire(data,pipes,mainPipe,processNumber);
                break;
            default:
                printf("le chiffre que vous avez saisie n'est pas ( >-1 et <4 )\n");
        }
        //---------------------------------------------------------------------------------------
        
        

    }while(data.operationToExecute != 0);
}


