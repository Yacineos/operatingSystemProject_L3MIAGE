#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "data.c"
#include "table.c"
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
            printf("le fils %d n'est pas mort hbibi \n", i);
        }else{
            printf("le fils %d est passée à la guillotine \n",i);
        }
        wait(NULL);
    }
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
        printf("erreur ya 3zrinek\n");
    }else{
        printf("bravo fiston bien reçu \n");
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


int main(int argc,char* argv[]){
    int processNumber = 0;

    if(argc != 2){
        printf("nombre de node n'est pas correct\n");
        return 1;
    }else{
    int processNumber = atoi(argv[1]);
    
    //-----------------------------------------------------------------------------
    
   
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------


    /* objet que le père envoie à ses fils pour chacune des inscturctions 
    //* on va le déclarer et l'initialiser à null / 0 ;
    -----------------------------------------------------------------------------*/
    DataPipes data ;
    data.operationToExecute = 0 ;
    data.key = 0 ;
    strcpy(data.val,"wesh frero t'es keblo haha ");
    //-----------------------------------------------------------------------------
    
    
    // déclaration des tubes qui vont servir de moyen de communication entre les fils
    int pipes[processNumber][2];
    //-----------------------------------------------------------------------------
    
    // déclaration et initialisation du main pipe qui va servir à lire les données 
    // envoyé par les fils 
    int mainPipe[2];
    if (pipe(mainPipe) == -1){
        perror("main pipe");
    }

    // initialisation des tubes 
    for (int i = 0; i < processNumber; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe (parent)");
        }
    }
    
    // création des processus fils
    for (int currentNode = 0; currentNode < processNumber; currentNode++)
    {

        switch (fork())
        {
        case -1:
            perror("forking child");
            exit(-1);
        
        case 0:

            // initialisation de la partie du dictionnaire qui est une liste
            // chaînée de bloc de type PTable_entry   
            //-------------------------------------------------------------
                //TODO 1.1
                // on initialise la tête à NULL
                printf("");
                PTable_entry tete = NULL;

            //--------------------------------------------------------------
            
            
            // fermeture mainPipe[Read]-------------------------------------
                //TODO 1.2
                close(mainPipe[READ_END]);
            //--------------------------------------------------------------
            
            
            
            /* fermeture de tous les pipes entre les childs 
            // sauf la où on va lire et écrire
            ---------------------------------------------------------------*/
                //TODO 1.3
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
            //---------------------------------------------------------------
            while(1){
                
                // lecture du fils dans le pipe qui le précède (pipe[currentNode][READ_END])
                // ici on va lire une structure de type DataPipes 
                //printf("je suis le fils %d et j'essaie de lire \n",currentNode);
                if(read(pipes[currentNode][READ_END],&data,sizeof(DataPipes))==-1){
                    printf("erreur read %d\n",currentNode);
                    perror("read");
                    exit(-1);
                }

                // affichage de ce qu'on a lu
                printf("je suis le fils %d et j'ai réussi à lire :",currentNode); 
                affichageData(data);

                switch (data.operationToExecute)
                {
                // on lit constamment et on vérifie la valeur 
                // de inputData que le père nous a envoyé et faire le traitement  
                // si (data.operationToExecute == 0) 
                //      l'opération 0 correspond au exit donc : 
                //          - fermer tous les tubes 
                //          - exit(-1)
                //------------------------------------------------------------

                        //fermer tous les tubes qui n'ont pas était encore fermé
                        //exit(-1); 
                    
                case 0 :
                        printf("");
                        int operationStatus = 0 ;
                        if(currentNode != processNumber-1){
                            close(pipes[currentNode][READ_END]);
                            close(pipes[currentNode+1][WRITE_END]);
                        }else{
                            close(pipes[currentNode][READ_END]);
                        }
                        operationStatus = 1 ;
                        write(mainPipe[WRITE_END],&operationStatus,sizeof(int));
                        close(mainPipe[WRITE_END]);
                        exit(EXIT_SUCCESS);

                    break;
                //--------------------------------------------------------------

                // si(data -> operationToexecute == 1)
                //      l'opération 1 correspond à un set donc :
                //          - test ( est ce que je suis le bon fils ? on peut içi tester avec : 
                //                                                   (( data -> key ) % nbProcessus) == currentNode ? 
                // 
                //              oui : on appelle la fonction store pour ajouter en tête de la liste chaînée 
                //              non : on envoie le data qu'on a reçu au prochain pipe 
                // ----------------------------------------------------------------
                    //TODO 1.4
                case 1 :
                    // suis je le bon fils ?
                    if(data.key % processNumber == currentNode ){
                        int operationStatus = 0;
                        //oui , on store le mot dans notre dictionnaire
                        store(&tete,data.key,data.val);
                        operationStatus = 1 ;
                        write(mainPipe[WRITE_END],&operationStatus,sizeof(int));
                        printf("node%d I got u bro , c'est bien enregistré \n",currentNode);

                    }else{
                        printf("node%d wesh frero c'est pas à moi de le faire \n",currentNode);
                        //non , du coup on envoie la structure au prochain fils   
                        write(pipes[currentNode+1][WRITE_END],&data,sizeof(DataPipes));
                    }
                    break;
                
                //-----------------------------------------------------------------

                // si (data -> operationToExecute == 2 )
                //      l'operation 2 correspond à un lookup du coup :
                //          - test ( est ce que je suis le bon fils ? on peut içi tester avec : 
                //                                                   (( data -> key ) % nbProcessus) == currentNode ?
                //              oui : on appelle la fonction char* lookup(key)
                //                    on recupère notre char* et on l'envoie dans le tube 
                //              non : on envoie le data qu'on a reçu au prochain node / pipe
                // ----------------------------------------------------------------
                    //TODO 1.5
                case 2 : 
                    // suis je le bon fils ?
                    if(data.key % processNumber == currentNode ){
                        //oui , on cherche la valeur dans notre dictionnaire
                        char* value = lookup(tete,data.key);
                        // on envoie la valeur dans le tube
                        write(mainPipe[WRITE_END],value,sizeof(char)*128);
                        printf("node%d I got u bro , c'est bien enregistré \n",currentNode);
                    }else{
                        printf("node%d wesh frero c'est pas à moi de le faire \n",currentNode);
                        //non , du coup on envoie la structure au prochain fils   
                        write(pipes[currentNode+1][WRITE_END],&data,sizeof(DataPipes));
                    }

                    break;
                
                //-----------------------------------------------------------------


                // si (data -> operationToExecute == 3 )
                //      l'operation 3 correspond à un dump donc :
                //           - en cours de reflexion 
                //-----------------------------------------------------------------
                    //TODO 
                case 3 :
                        
                        operationStatus = 0 ;
                        printf("fils %d\n", getpid());
                        display(tete);
                        operationStatus = 1 ;
                        write(mainPipe[WRITE_END],&operationStatus,sizeof(int));

                    break;
                //-----------------------------------------------------------------
                default:
                    break;
                }

            
            }
            exit(0);
        default :
           break;
        }
    }
    //le père commence sa partie --------------------------------------------------------------------
    do{
        //preparation du struct ---------------------------------------------------------------------
        // lire les données saisie par l'utilisateurs et utilisation de do while pour controler l'input
        do{
            printf("Saisir commande (0 = exit, 1 = set, 2 = lookup, 3 = dump):");
            scanf("%d",&data.operationToExecute);

        }while(data.operationToExecute <0 && data.operationToExecute > 3);
        /*switch case qui va bien -------------------------------------------------------------*/
        //si (0 appelle la fonction exit 
        //si (1) appelle la fonction set
        //si (2) appelle la fonction lookup
        //si (3) appelle la fonction dump
        switch(data.operationToExecute){
            case 0 :
                printf("on va exit le programme \n");
                exitProgramme(data,pipes,mainPipe,processNumber); 
                break ;
            case 1 :
                do{
                    printf("Saisir la cle (decimal number):");
                    scanf("%d",&data.key);
                }while(data.key<0);
                do{
                    printf("Saisir la valeur (chaine de caracteres, max 128 chars):");
                    // scanf n'arrive pas à empecher l'utilisateur de ne pas dépasser 128 char donc on utilise fgets
                    scanf("%s",data.val);    
                }while(strlen(data.val) > 127);
                printf("attention je vais set \n");
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
    return 0;
    }
}
 