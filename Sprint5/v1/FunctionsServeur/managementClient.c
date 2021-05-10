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

void displayClient(int numClient){

	char * msg = (char *)malloc(sizeof(char)*12*MAX_CLIENT);
	strcpy(msg, "_____ Liste des clients connectés _____ \n");
    int i;

    for(i = 0; i < MAX_CLIENT; i++){
    	
    	/*Si le client i est connecté*/
    	if(tabClient[i].occupied){
    		strcat(msg, "-- ");
    		strcat(msg, tabClient[i].pseudo);
    		strcat(msg, "\n");
    	}
    }
    strcat(msg, "_______________________________________ \n");
    sending(tabClient[numClient].dSC, msg);
    free(msg);

	return;
}

void killThread(){
    int i;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/
    
    for (i=0;i<MAX_CLIENT*2;i++){
        if(tabThreadToKill[i]!=((void *)0)){
            pthread_join(tabThreadToKill[i],NULL);
        }
    }
    nbThreadToKill=0;

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
}