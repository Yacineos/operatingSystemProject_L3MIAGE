//BINOMES :Oukkal Yacine N° étudiant: 12213245 || Monachon Vivian N° étudiant: 11907866

#include <stdlib.h>
#include <stdio.h>
#include "table.h"
#include "table.c"


int main(){
    char temp[100];
    // init la tête
    PTable_entry tete = NULL;
    // insertion de qlq elts
    store(&tete , 5 , "Bonjour");
    store(&tete , 3 , "je");
    store(&tete , 9 , "im2ag");
    
    // recherche elt existant 
    printf("%s\n",lookup(tete,5));
    
    //affichage de toute la liste 
    display(tete);
    strcpy(temp,lookup(tete,5));
    if(temp!=NULL)
    //recherche elt non existant 
    printf("%s\n",temp);
    //fait crach le programme
    //affichage de toute la liste 
    display(tete);

    return 0 ;
}