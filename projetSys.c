#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#define READ_END 0
#define WRITE_END 1

int main()
{
    // envoyer 1 : quelle requete faire , 2 : clé , 3 : valeur
    // chaque noeud vérifie quelle requete il doit faire et ainsi quoi faire

    /*on suppose de travailler avec 2 processus pour voir si ça marche*/
    int processNumber = 3;

    /* valeur on suppose que le père envoie */
    int y = 5;
    /*parties pour n processus*/
    // printf("entrez val n > 1 :\n");
    // scanf("%d",&n);

    int pipes[processNumber][2];

    // déclaration des tubes et leurs initialisation
    for (int i = 0; i < processNumber; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe (parent)");
        }
    }
    
    // création des processus fils
    for (int i = 0; i < processNumber; i++)
    {

        switch (fork())
        {
        case -1:
            perror("forking child");
            exit(-1);
        case 0:

            // lecture du fils dans le pipe qui le précède (pipe[i])
            printf("je suis le fils %d et j'essaie de lire \n",i);
            if(read(pipes[i][READ_END],&y,sizeof(int))==-1){
                printf("erreur read %d\n",i);
                perror("read");
                exit(-1);
            }

            
           
            printf("le fils (%d) a lu %d \n", i , y);
            // incrémentation de la valeur échangée entre les processus
            y++;
            printf("nouvelle val de y = %d \n",y);
            
            // écriture du fils dans le pipe[i+1]
            // si il ,ne s'agit pas du dernier fils nous écrivons la valeur y dans 
            // le pipe suivant (pipe[i+1])
            if(i != processNumber -1){
                if(write(pipes[i+1][WRITE_END],&y,sizeof(int))==-1){
                    perror("write");
                }
            }else{
                
                //si c'est le dernier fils, nous réécrivons la valeur dans le tube[0] 
                //afin que le pere puisse la récupérer
                if(write(pipes[0][WRITE_END],&y,sizeof(int))==-1){
                    perror("write");
                }
            }
            

            
            printf("le fils (%d) a ecrit %d \n", i , y);

            //fermeture de tous les pipes
            for(int j = 0; j < processNumber; j++){
                close(pipes[j][READ_END]);
                close(pipes[j][WRITE_END]);
            }

            exit(EXIT_SUCCESS);
            
        default :
           break;
        }
    }
    int x = 3 ;
    // écriture de x par le père dans pipe[0]    
    printf("le père a ecrit %d \n",x);
    write(pipes[0][WRITE_END],&x,sizeof(int));
    
    //fermeture de tous les pipes sauf la lecture dans le pipe[0]
    printf("le père a fermé tous les pipes\n");
    for(int j = 0; j < processNumber; j++){
        if (j != 0){
            close(pipes[j][READ_END]);
        }
        close(pipes[j][WRITE_END]);
    }

    
    //le pere attends que tous les fils aient executé leurs fonctions
       
    for(int j = 0 ; j < processNumber ; j++){
        wait(NULL);
    }

    // le pere procede a la lecture de la valeur y incrémentée par tous les processus fils
    printf("le pere essaye de lire\n");
    if(read(pipes[0][READ_END],&y,sizeof(int))==-1){
        printf("erreur read %d\n",0);
        perror("read");
        exit(-1);
    }

    printf("le père a lu %d \n",y); 
    
    // le père ferme la derniere extrémité du pipe afin que tous les pipes soient fermés 
    // et que le programme puisse etre tué 
    close(pipes[0][READ_END]);  
    return 0;
    
}
