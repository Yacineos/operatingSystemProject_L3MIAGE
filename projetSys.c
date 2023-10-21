//BINOMES :Oukkal Yacine N° étudiant: 12213245 || Monachon Vivian N° étudiant: 11907866

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "projetSys.h"
#include "table.h"
#include <string.h>
#define READ_END 0
#define WRITE_END 1




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
    strcpy(data.val,"debug");
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
             * sauf la où on va lire et écrire
             * -------------------------------------------------------------*/
                closeUnusedPipes(pipes,processNumber,currentNode);
            
            
                //TODO 1.3
               
            
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

                // affichage de ce qu'on a lu pour le debug
                //printf("je suis le fils %d et j'ai réussi à lire :",currentNode); 
                //affichageData(data);

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
                        // est ce que la clé existe déjà ?
                        if(lookup(tete,data.key) != NULL){
                            printf("node%d la clé existe déjà \n",currentNode);
                            operationStatus = 0 ;
                            write(mainPipe[WRITE_END],&operationStatus,sizeof(int));
                        }else{
                            //oui , on store le mot dans notre dictionnaire
                            store(&tete,data.key,data.val);
                            operationStatus = 1 ;
                            write(mainPipe[WRITE_END],&operationStatus,sizeof(int));
                        }
                        

                    }else{
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
                        if(value == NULL){
                            printf("node%d je n'ai pas trouvé la valeur \n",currentNode);
                            value = "je n'ai pas trouvé la valeur";
                        }    
                        // on envoie la valeur dans le tube
                        write(mainPipe[WRITE_END],value,sizeof(char)*128);
                    }else{
                        //non , du coup on envoie la structure au prochain fils   
                        write(pipes[currentNode+1][WRITE_END],&data,sizeof(DataPipes));
                    }

                    break;
                
                //-----------------------------------------------------------------


                // si (data -> operationToExecute == 3 )
                //      l'operation 3 correspond à un dump donc :
                //          on envoie le data du père et on utilise la fonction display
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
    controller(data,pipes,mainPipe,processNumber);
    return 0;
    }
}
 