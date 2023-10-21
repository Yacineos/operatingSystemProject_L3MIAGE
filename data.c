#include <stdlib.h>
#include <stdio.h>
#include "data.h"

// cette fonciton à pour but d'afficher dans le terminal 
// les valuers de notre structure 
void affichageData(DataPipes data){
    printf("( %d , %d , %s )\n",data.operationToExecute,data.key,data.val);
}