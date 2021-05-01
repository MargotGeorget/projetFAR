#include "managementClient.h"

int giveNumClient(){
    int i = 0;
    int indice = -1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<MAX_CLIENT && indice==-1){
        if(!tabClient[i].occupied){
            indice = i;
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return indice;
}

int findClient(char * pseudo){
    int i = 0;
    int client = -1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<MAX_CLIENT && client==-1){
        if (tabClient[i].occupied){
            if (strcmp(pseudo, tabClient[i].pseudo)==0){
                client = tabClient[i].dSC;
            }
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return client;
}

int isAvailablePseudo(char * pseudo){
    int i= 0;
    int available = 1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/
    
    while (i<MAX_CLIENT && available){
        
        if(tabClient[i].occupied){
            if(strcmp(tabClient[i].pseudo,pseudo)==0){
                available = 0;
            }
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
    
    return available;
}

void killThread(){
    pthread_mutex_lock(&lock);
    int i;
    for (i=0;i<MAX_CLIENT;i++){
        if(tabThreadToKill[i]){
            pthread_cancel(tabThread[i]);
            tabThreadToKill[i]=0;
        }
    }
    pthread_mutex_unlock(&lock);
}