#include "threadFunctions.h"

void * receivingFile_th(void * fileNameParam){

    long dSCFile;
    /*Accepter une connexion*/
    dSCFile = acceptConnection(dSFile);

    char * fileName = (char *)fileNameParam;

    /*Création d'un fichier au même nom et reception du contenu du fichier*/

    /*Création du chemin pour accéder au fichier*/ 
    char * pathToFile = (char *) malloc(sizeof(char)*130);
    strcpy(pathToFile,"FileServeur/");
    strcat(pathToFile,fileName);

    printf("Je reçois le fichier %s du client le socket %ld\n",fileName,dSFile);

    /*Ouverture du fichier et création du buffer pour recevoir les données*/
    char buffer[1024];
    FILE * fp;
    fp = fopen(pathToFile,"w");

    /*Booleen pour controler la fin de la reception du fichier*/
    int isEndRecvFile;
    recv(dSCFile, &isEndRecvFile, sizeof(int), 0);

    /*Reception*/
    while(!isEndRecvFile){
        recv(dSCFile, buffer, 1024, 0);
        recv(dSCFile, &isEndRecvFile, sizeof(int), 0);
        printf("%s\n",buffer);
        fprintf(fp,"%s",buffer);
        bzero(buffer, 1024);
    }
    fclose(fp);
    return NULL;

}

void * broadcast(void * clientParam){
    int isEnd = 0;
    int numClient = (long) clientParam;

    /*METTRE UN MUTEX !!*/

    while(!isEnd){
        /*Réception du message*/
        char * msgReceived = (char *) malloc(sizeof(char)*100);
        receiving(tabClient[numClient].dSC, msgReceived, sizeof(char)*100);
        printf("\nMessage recu: %s \n", msgReceived);

        /*On verifie si le client veut terminer la communication*/
        isEnd = endOfCommunication(msgReceived);

        char first = msgReceived[0];
        if(strcmp(&first,"@")==0){
            sendingPrivate(numClient, msgReceived);
        }else if(isReceivingFile(msgReceived)){

            /*Reception du nom du fichier à recevoir*/
            char * fileName = (char *) malloc(sizeof(char)*30);
            receiving(tabClient[numClient].dSC, fileName, sizeof(char)*30);
            printf("\nNom du fichier à recevoir: %s \n", fileName);

            fileName = strtok(fileName, "\n");

            pthread_t threadFile;
            int thread = pthread_create(&threadFile, NULL, receivingFile_th, (void *)fileName);
            if(thread==-1){
                perror("error thread");
            }
        }else {
            /*Envoi du message aux autres clients*/
            printf("Envoi du message aux autres clients. \n");
            sendingAll(numClient, msgReceived);
        }
    }

    /*Fermeture du socket client*/
    sem_post(&semNbClient);
    pthread_mutex_lock(&lock);
    tabClient[numClient].occupied=0;
    tabThreadToKill[numClient]=1;
    close(tabClient[numClient].dSC);
    pthread_mutex_unlock(&lock);
    return NULL;
}
