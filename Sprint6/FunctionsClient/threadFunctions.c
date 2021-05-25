#include "threadFunctions.h"
#include <fcntl.h>

/* -- Fonction pour le thread d'envoi de fichier -- */
void * uploadFile_th(void * fpParam){

    /*Création de la socket*/
	long dSFile = createSocketClient(port+1, ip);

    FILE * fp = (FILE *)fpParam;

    /*Création du buffer pour l'envoi du fichier*/
    char data[1024] = "";
    int nbBytes = -1;

    /*descripteur de fichier à partir du FILE * */
    int fd = fileno(fp);

    /* tant qu'on n'a pas atteint la fin du fichier
     * faire un read (retourne 0 si on est en fin de fichier)
     * envoyer le nombre de bytes lu puis le bloc lu */
    while(nbBytes != 0){
        nbBytes = read(fd, data, 1023);
        data[1023]='\0';
        sleep(0.3);
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
void * downloadFile_th(void * fileNameParam){

    /*Créer un socket et demander une connection*/ 
    long dSFile = createSocketClient(port+1, ip);

    char * fileName = (char *)fileNameParam;

    /*Création du chemin pour enregister le fichier*/ 
    char * pathToFile = (char *) malloc(sizeof(char)*130);
    strcpy(pathToFile,"FileReceived/");
    strcat(pathToFile,fileName);

    /*Création du fichier et du buffer pour recevoir les données*/
    char buffer[1024];
    int fp = open(pathToFile, O_WRONLY |  O_CREAT, 0666);
    if(fp == -1){
        printf("erreur au open");
        exit(1);
    }

    int nbBytes; /*Nombre de bytes à recevoir*/
    nbBytes = receivingInt(dSFile);

    /*Reception*/
    while(nbBytes>0){
        recv(dSFile, buffer, 1024, 0);
        write(fp, buffer,nbBytes);
        nbBytes = receivingInt(dSFile);
        bzero(buffer, 1024);
    }
    printf("\n** Fichier reçu **\n");
    
    close(fp);
    close(dSFile);
    return NULL;

}


/* -- Fonction pour le thread d'envoi -- */
void * sending_th(void * dSparam){
    int dS = (long)dSparam;

    while (!isEnd){

        /*Saisie du message au clavier*/
        char * m = (char *) malloc(sizeof(char)*100);
        fgets(m, 100, stdin);
        strtok(m,"\n");
        strcat(m,"\0");

        /*On vérifie si le client veut quitter la communication*/
        isEnd = endOfCommunication(m);

        /*Envoi*/
        sending(dS, m);

        if (isUploadFile(m)){ /*On vérifie si le client veut envoyer un fichier*/
            uploadFile(dS);
        }else if(isMan(m)){
            displayMan();
        }

        free(m);
    }

    close(dS);
    return NULL;
}

/* -- Fonction pour le thread de reception -- */
void * receiving_th(void * dSparam){
    int dS = (long)dSparam;
    int serveurShutdown;

    while(!isEnd && !serveurShutdown){
        
        /*Reception des messages*/
        char * r = (char *) malloc(sizeof(char)*500);
        receiving(dS, r, sizeof(char)*500);

        /*Vérification des messages reçu*/
        isEnd = endOfCommunication(r);
        serveurShutdown = isServeurShutdown(r);

        if(isDownloadFile(r)){ /*On vérifie si le client va recevoir un fichier*/
            downloadFile(dS);
        }else if(!isEnd && !serveurShutdown){
            printf("%s",r);
        }

        free(r);
    }

    close(dS);
    return NULL;
}

