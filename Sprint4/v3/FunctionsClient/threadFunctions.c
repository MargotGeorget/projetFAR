#include "threadFunctions.h"

/* -- Fonction pour le thread d'envoi de fichier -- */
void * sendingFile_th(void * fpParam){

    /*Création de la socket*/
	long dSFile = createSocketCLient(port+1, ip);

    FILE * fp = (FILE *)fpParam;

    printf("J'envoi le fichier au serveur avec le socket %ld\n",dSFile);
    
    /*Création du buffer et d'un booleen pour controler la fin de l'envoi du fichier*/
    char data[1024] = "";
    int isEndSendFile = 0;

    while(fgets(data, 1024, (FILE *)fp) != NULL) {
        sendingInt(dSFile, isEndSendFile);
        if (send(dSFile, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, 1024);
    }
    isEndSendFile = 1;
    sendingInt(dSFile, isEndSendFile);  

    fclose(fp);
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
        printf("J'envoi le message au serveur avec le socket %d\n",dS);
        sending(dS, m);

        if (isSendingFile(m)){
            sendingFile(dS);
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

        free(r);
    }
    close(dS);
    return NULL;
}

