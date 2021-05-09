#include "threadFunctions.h"
#include <fcntl.h>

/* -- Fonction pour le thread d'envoi de fichier -- */
void * sendingFile_th(void * fpParam){

    /*Création de la socket*/
	long dSFile = createSocketClient(port+1, ip);

    FILE * fp = (FILE *)fpParam;

    /*printf("J'envoi le fichier au serveur avec le socket %ld\n",dSFile);*/
    
    /*Création du buffer pour l'envoi du fichier*/
    char data[1024] = "";
    int nbBytes = -1;

    /*descripteur de fichier à partir du FILE * */
    int fd = fileno(fp);

    /*tant qu'on n'a pas atteint la fin du fichier
     * faire un read (retourne 0 si on est en fin de fichier)
     * envoyer le bloc lu */
    while(nbBytes != 0){
        nbBytes = read(fd, data, 1023);
        data[1023]='\0';
        sendingInt(dSFile, nbBytes);
        if(nbBytes != 0){
            if (send(dSFile, data, sizeof(data), 0) == -1) {
                perror("[-]Error in sending file.");
                exit(1);
            }
        }
        bzero(data, nbBytes);
    } 
    printf("\n** Fichier envoyé **\n");
    fclose(fp);
    close(dSFile);
    return NULL;
}

/* -- Fonction pour le thread de reception de fichier -- */
void * receivingFile_th(void * fileNameParam){

    /*Créer un socket et demander une connection*/ 
    long dSFile = createSocketClient(port+1, ip);

    char * fileName = (char *)fileNameParam;

    /*Création du chemin pour enregister le fichier*/ 
    char * pathToFile = (char *) malloc(sizeof(char)*130);
    strcpy(pathToFile,"FileReceived/");
    strcat(pathToFile,fileName);

    /*printf("Je reçois le fichier %s du serveur avec le socket %ld\n",fileName,dSFile);*/

    /*Création du fichier et du buffer pour recevoir les données*/
    char buffer[1024];
    /*changement du fopen en open*/
    int fp = open(pathToFile, O_WRONLY |  O_CREAT, 0666);
    if(fp == -1){
        printf("erreur au open");
        exit(1);
    }

    /*Booleen pour controler la fin de la reception du fichier*/
    int nbBytes;
    recv(dSFile, &nbBytes, sizeof(int), 0);

    /*Reception*/
    while(nbBytes>0){
        recv(dSFile, buffer, 1024, 0);
        write(fp, buffer,nbBytes);
        recv(dSFile, &nbBytes, sizeof(int), 0);
        bzero(buffer, 1024);
    }
    printf("\n** Fichier reçu **\n");
    close(fp);

    close(dSFile);
    return NULL;

}


/* -- Fonction pour le thread d'envoi -- */
void * sending_th(void * dSparam){
    printf("sending th lancé \n");
    int dS = (long)dSparam;
    while (!isEnd){
        
        /*Saisie du message au clavier*/
        char * m = (char *) malloc(sizeof(char)*100);
        printf(">");
        fgets(m, 100, stdin);

        /*On vérifie si le client veut quitter la communication*/
        isEnd = endOfCommunication(m);
        printf("isEnd: %d\n",isEnd);
        
        /*Envoi*/
        printf("J'envoi le message au serveur avec le socket %d\n",dS);
        sending(dS, m);

        if (isSendingFile(m)){
            sendingFile(dS);
        }

        free(m);
    }
    close(dS);
    printf("dS close au sending\n");
    return NULL;
}

/* -- Fonction pour le thread de reception -- */
void * receiving_th(void * dSparam){
    int dS = (long)dSparam;
    while(!isEnd){

        char * r = (char *) malloc(sizeof(char)*300);
        receiving(dS, r, sizeof(char)*300);

        if(isReceivingFile(r)){
            receivingFile(dS);
        }else{
            printf(">%s",r);
        }

        free(r);
    }
    close(dS);
    printf("dS close au receiving\n");
    return NULL;
}

