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
    int fp = open(pathToFile, O_WRONLY |  O_CREAT, 0666);
    if(fp == -1){
        printf("erreur au open");
        exit(1);
    }
    int nbBytes = receivingInt(dSCFile);

    /*Reception*/
    while(nbBytes>0){
        recv(dSCFile, buffer, 1024, 0);
        write(fp, buffer,nbBytes);
        nbBytes = receivingInt(dSCFile);
        bzero(buffer, 1024);
    }
    printf("\n**Fichier reçu**\n");
    close(fp);
    close(dSCFile);
    return NULL;

}

void * sendingFile_th(void * fpParam){

    /*Accepter une connexion*/
	long dSCFile = acceptConnection(dSFile);

    FILE * fp = (FILE *)fpParam;

    printf("J'envoie le fichier au client avec le socket %ld\n",dSCFile);
    
    /*Appel de la fonction envoyant le fichier*/
    sendFile(dSCFile,fp);

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
        if(strcmp(&first,"@")==0){ /*On regarde si le client souhaite envoyer un message privé*/
            sendingPrivate(numClient, msgReceived);
        }else if(isReceivingFile(msgReceived)){ /*On regarde si le client souhaite envoyer un fichier au serveur*/

            /*Reception du nom du fichier à recevoir*/
            char * fileName = (char *) malloc(sizeof(char)*30);
            receiving(dSC, fileName, sizeof(char)*30);
            printf("\nNom du fichier à recevoir: %s \n", fileName);
            if(strcmp(fileName,"error")==0){
                printf("Nom de fichier incorrect\n");
            }else{

                fileName = strtok(fileName, "\n");

                /*Création du thread pour gérer la reception du fichier*/
                pthread_t threadFile;
                int thread = pthread_create(&threadFile, NULL, receivingFile_th, (void *)fileName);
                if(thread==-1){
                    perror("error thread");
                }
            }
        }else if(isSendingFile(msgReceived)){ /*On regarde si le client souhaite télécharger un fichier du serveur*/

            /*Envoi du message au thread de reception du client*/
            sending(dSC,msgReceived);

            /*Envoi des fichiers pouvant être téléchargés*/
            int cr = system("ls ./FileServeur > listeFichier.txt");
            if(cr == -1){
                printf("commande echouée");
            }
            
            FILE * fp = fopen("listeFichier.txt", "r");
            sendFile(dSC,fp);

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
                /*On informe le client de l'erreur*/
                sending(dSC,error);
            }else {
                /*Le fichier va être envoyé, on informe le client et on transmet au thread de reception le nom du fichier*/
                sending(dSC,fileName);
                /*Création du thread d'envoi de fichier*/
                pthread_t threadFile;
                int thread = pthread_create(&threadFile, NULL, sendingFile_th, (void *)fp);
                if(thread==-1){
                    perror("error thread");
                }
            }
        }else if(isPresentationRoom(msgReceived)){
            presentationRoom(dSC);
        }else if(isJoinRoom(msgReceived)){
            joinRoom(numClient, msgReceived);
        }else if(isCreateRoom(msgReceived)){
            createRoom(numClient, msgReceived);
        }else if(isRemoveRoom(msgReceived)){
            removeRoom(numClient,msgReceived);
        }else if(isUpdateNameRoom(msgReceived)){
            updateNameRoom(numClient,msgReceived);
        }else if(isUpdateDescrRoom(msgReceived)){
            updateDescrRoom(numClient,msgReceived);
        }else if(!isEnd){
            /*Envoi du message aux autres clients*/
            printf("Envoi du message aux autres clients. \n");
            sendingRoom(numClient, msgReceived);
        }
    }
    /*Fermeture du socket client*/
    deleteMember(numClient,tabClient[numClient].idRoom);
    pthread_mutex_lock(&lock);
    tabClient[numClient].occupied=0;
    tabThreadToKill[nbThreadToKill]=tabThread[numClient];
    nbThreadToKill+=1;
    tabThread[numClient] = ((void *)0);
    close(tabClient[numClient].dSC);
    pthread_mutex_unlock(&lock);
    sem_post(&semNbClient);
    return NULL;
}
