#include "threadFunctions.h"

void * receivingFile_th(void * fileNameParam){

    long dSCFile;
    /*Accepter une connexion*/
    dSCFile = acceptConnection(dSFile);

    char * fileName = (char *)fileNameParam;

    /*Création d'un fichier au même nom et reception du contenu du fichier*/

    /*Création du chemin pour enregister le fichier*/ 
    char * pathToFile = (char *) malloc(sizeof(char)*130);
    strcpy(pathToFile,"FileServeur/");
    strcat(pathToFile,fileName);

    printf("Je reçois le fichier %s du client le socket %ld\n",fileName,dSFile);

    /*Création du fichier et du buffer pour recevoir les données*/
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
        fprintf(fp,"%s",buffer);
        bzero(buffer, 1024);
    }
    fclose(fp);
    close(dSCFile);
    return NULL;

}

void * sendingFile_th(void * fpParam){

    /*Accpeter une connexion*/
	long dSCFile = acceptConnection(dSFile);

    FILE * fp = (FILE *)fpParam;

    printf("J'envoi le fichier au client avec le socket %ld\n",dSCFile);
    
    /*Création du buffer et d'un booleen pour controler la fin de l'envoi du fichier*/
    char data[1024] = "";
    int isEndSendFile = 0;

    while(fgets(data, 1024, (FILE *)fp) != NULL) {
        sendingInt(dSCFile, isEndSendFile);
        if (send(dSCFile, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, 1024);
    }
    isEndSendFile = 1;
    sendingInt(dSCFile, isEndSendFile);  

    fclose(fp);
    close(dSCFile);
    return NULL;
}

void * broadcast(void * clientParam){
    int isEnd = 0;
    int numClient = (long) clientParam;
    pthread_mutex_lock(&lock);
    int dSC = tabClient[numClient].dSC;
    pthread_mutex_unlock(&lock);

    while(!isEnd){
        /*Réception du message*/
        char * msgReceived = (char *) malloc(sizeof(char)*100);
        receiving(dSC, msgReceived, sizeof(char)*100);
        printf("\nMessage recu: %s \n", msgReceived);

        /*On verifie si le client veut terminer la communication*/
        isEnd = endOfCommunication(msgReceived);

        char first = msgReceived[0];
        if(strcmp(&first,"@")==0){
            sendingPrivate(numClient, msgReceived);
        }else if(isReceivingFile(msgReceived)){

            /*Reception du nom du fichier à recevoir*/
            char * fileName = (char *) malloc(sizeof(char)*30);
            receiving(dSC, fileName, sizeof(char)*30);
            printf("\nNom du fichier à recevoir: %s \n", fileName);

            fileName = strtok(fileName, "\n");

            pthread_t threadFile;
            int thread = pthread_create(&threadFile, NULL, receivingFile_th, (void *)fileName);
            if(thread==-1){
                perror("error thread");
            }

        }else if(isSendingFile(msgReceived)){

            /*Envoi du message au thread de reception du client*/

            sending(dSC,msgReceived);

            /*Envoi des fichiers pouvant être téléchargés*/
            int cr = system("ls ./FileServeur > listeFichier.txt");
            /*Vérification*/
            if(cr == -1){
                printf("commande echouée");
            }
            FILE *fp;
            fp = fopen("listeFichier.txt", "r");
            /*ToDo : vérifier l'ouverture du fichier : message d'erreur */

            /*Création du buffer et d'un booleen pour controler la fin de l'envoi du fichier*/
            char data[1024] = "";
            int isEndSendFile = 0;

            while(fgets(data, 1024, (FILE *)fp) != NULL) {
                sendingInt(dSC, isEndSendFile);
                if (send(dSC, data, sizeof(data), 0) == -1) {
                    perror("[-]Error in sending file.");
                    exit(1);
                }
                bzero(data, 1024);
            }
            isEndSendFile = 1;
            sendingInt(dSC, isEndSendFile); 
            fclose(fp);

            /*Reception du nom du fichier à envoyer*/
            char * fileName = (char *) malloc(sizeof(char)*30);
            receiving(dSC, fileName, sizeof(char)*30);
            printf("\nNom du fichier à envoyer: %s \n", fileName);

            fileName = strtok(fileName, "\n");

            /*Création du chemin pour trouver le fichier*/
            char * pathToFile = (char *) malloc(sizeof(char)*130);
            strcpy(pathToFile,"FileServeur/");
            strcat(pathToFile,fileName);

            /*Ouverture et envoi du fichier*/
            fp = fopen(pathToFile,"r");
            if (fp== NULL) {   
                char * error = "error";
                printf("Erreur! Fichier inconnu\n"); 
                sending(dSC,error);
            }else {
                sending(dSC,fileName);

                pthread_t threadFile;
                int thread = pthread_create(&threadFile, NULL, sendingFile_th, (void *)fp);
                if(thread==-1){
                    perror("error thread");
                }
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
