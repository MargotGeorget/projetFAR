#include "threadFunctions.h"
#include <fcntl.h>

/* -- Fonction pour le thread d'envoi de fichier -- */
void * sendingFile_th(void * fpParam){

    /*Création de la socket*/
	long dSFile = createSocketClient(port+1, ip);

    FILE * fp = (FILE *)fpParam;

    /*printf("J'envoi le fichier au serveur avec le socket %ld\n",dSFile);*/
    
    /*Création du buffer et d'un booleen pour controler la fin de l'envoi du fichier*/
    char data[1024] = "";
    int isEndSendFile = 0;
    int nbOctets = -1;

    /*descripteur de fichier à partir du FILE * */
    int fd = fileno(fp);
    printf("fd : %d\n", fd);

    /*tant qu'on n'a pas atteint la fin du fichier
     * faire un read (retourne 0 si on est en fin de fichier)
     * envoyer le bloc lu */
    while(nbOctets != 0){
        nbOctets = read(fd, data, 1023);
        data[1023]='\0';
        printf("retour read : %d\n",nbOctets);
        printf("contenu envoyé %s\n",data);
        sendingInt(dSFile, nbOctets);
        if(nbOctets != 0){
            if (send(dSFile, data, sizeof(data), 0) == -1) {
                perror("[-]Error in sending file.");
                exit(1);
            }
        }

        printf("après send \n");
        bzero(data, nbOctets);
    } 
    printf("après le while\n");

    isEndSendFile = 1;
    /*sendingInt(dSFile, isEndSendFile);*/
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

    int fd = open(pathToFile, O_RDONLY);
    off_t size = lseek(fd, 0, SEEK_END);
    printf("size: %ld\n",size);

    /*Création du fichier et du buffer pour recevoir les données*/
    char buffer[1024];
    FILE * fp;
    fp = fopen(pathToFile,"w");

    /*Booleen pour controler la fin de la reception du fichier*/
    int isEndRecvFile = receivingInt(dSFile);

    /*Reception*/
    while(!isEndRecvFile){
        recv(dSFile, buffer, 1024, 0);
        isEndRecvFile = receivingInt(dSFile);
        fprintf(fp,"%s",buffer);
        bzero(buffer, 1024);
    }
    fclose(fp);
    close(dSFile);
    return NULL;

}


/* -- Fonction pour le thread d'envoi -- */
void * sending_th(void * dSparam){
    int dS = (long)dSparam;
    while (!isEnd){

        /*Saisie du message au clavier*/
        char * m = (char *) malloc(sizeof(char)*100);
        printf(">");
        fgets(m, 100, stdin);

        /*On vérifie si le client veut quitter la communication*/
        isEnd = endOfCommunication(m);
        
        /*Envoi*/
        /*printf("J'envoi le message au serveur avec le socket %d\n",dS);*/
        sending(dS, m);

        if (isSendingFile(m)){
            sendingFile(dS);
        }else if(isReceivingFile(m)){
            receivingFileSending_th(dS);
        }

        free(m);
    }
    close(dS);
    return NULL;
}

/* -- Fonction pour le thread de reception -- */
void * receiving_th(void * dSparam){
    int dS = (long)dSparam;
    while(!isEnd){

        char * r = (char *) malloc(sizeof(char)*100);
        receiving(dS, r, sizeof(char)*100);
        printf(">%s",r);

        if(isReceivingFile(r)){
            receivingFileReceiving_th(dS);
        }

        free(r);
    }
    close(dS);
    return NULL;
}

