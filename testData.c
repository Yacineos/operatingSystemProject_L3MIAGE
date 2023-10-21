#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/wait.h>
#include "data.c"
#define READ_END 0
#define WRITE_END 1
#include <string.h>
//déterminer si le pere envoie un Datapipes ou une référence Pdatapipes vers le fils 
int main(){
    
    
    int processNumber = 1;
    //initialisation d'un dataPipes
    DataPipes data;
    data.operationToExecute = 5;
    data.key= 4 ;
    strcpy(data.val,"58752dsfds");
    
    // création d'un tube
    int pipes[processNumber][2];

    for (int i = 0; i < processNumber; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe (parent)");
        }
    } 

    // création d'un fils
    switch (fork())
        {
        case -1:
            //cas d'erreur
            perror("forking child");
            exit(-1);
        case 0:
            // lecture du fils dans le pipe 
                printf("le fils va lire  %s \n",data.val);
            if(read(pipes[0][READ_END],&data,sizeof(DataPipes))==-1){
                printf("erreur read %d\n",0);
                perror("read");
                exit(-1);
            }
            //affichage de la donnée transmise par le pere
            printf("Data1 : (%d , %d , %s )\n",data.operationToExecute , data.key , data.val );
            
            //fermeture du tube
            close(pipes[0][READ_END]);
            close(pipes[0][WRITE_END]);
            
            exit(EXIT_SUCCESS);

        default :
           break;
        }
    
    
    //saisie des parametres dans le terminal
    printf("Saisir commande (0 = exit, 1 = set, 2 = lookup, 3 = dump): ");
    scanf("%d",&(data.operationToExecute));
    printf("Saisir la cle (decimal number):");
    scanf("%d",&(data.key));
    printf("Saisir la valeur (chaine de caracteres, max 128 chars): ");
    fgets(data.val,sizeof(data.val),stdin);
    affichageData(data);
    //ecriture dans le tube
    printf("le père a ecrit %s \n",data.val);
    write(pipes[0][WRITE_END],&data,sizeof(DataPipes));
    printf("le père a ecrit %s \n",data.val);
    //fermeture du pipe
    close(pipes[0][READ_END]);
    close(pipes[0][WRITE_END]);

    //le pere attend la mort du fils

    wait(NULL);
    



    return 0 ;
}