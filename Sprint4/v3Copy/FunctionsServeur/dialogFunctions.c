#include "dialogFunctions.h"

void receiving(int dS, char * rep, ssize_t size){
    int recvR = recv(dS, rep, size, 0);
    if (recvR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au recv");
        exit(-1);
    }
}

void sending(int dS, char * msg){
    int sendR = send(dS, msg, strlen(msg)+1, 0);
    if (sendR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au send");
        exit(-1);
    }
}

void sendingInt(int dS, int number){
    int sendR = send(dS, &number, sizeof(int), 0);
    if (sendR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au send");
        exit(-1);
    }
}

void sendingAll(int numClient, char * msg){

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    int dS = tabClient[numClient].dSC;

    printf("Je reçois le message du client avec le socket %d\n",dS);

    addPseudoToMsg(msg, tabClient[numClient].pseudo);

    int i;
    for (i = 0; i<MAX_CLIENT ; i++) {

        /*On n'envoie pas au client qui a écrit le message*/
        if(tabClient[i].occupied && dS != tabClient[i].dSC){
            sending(tabClient[i].dSC, msg);
        }
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

}

void sendingPrivate(int numClient, char * msg){

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    int mydSC = tabClient[numClient].dSC;

    /*Récupération du pseudo présent au début du message*/
    char * copyMsg = (char *) malloc(sizeof(char)*100);
    strcpy(copyMsg, msg);
    char * pseudo = (char *) malloc(sizeof(char)*13);
    pseudo = strtok(copyMsg," ");
    strcpy(pseudo,pseudo+1);

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    int dSC = findClient(pseudo);

    if (dSC==-1){ /*Aucun client n'a été trouvé*/

        char * error = (char *) malloc(sizeof(char)*100);
        error = "Le pseudo saisi n'existe pas!\n";
        sending(mydSC, error);

        printf("%s",error);

        free(error);

    }else { /*Le client à été trouvé et nous avons récupéré sa socket*/

        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        /*Ajout du pseudo de l'expéditeur devant le message à envoyer*/
        addPseudoToMsg(msg, tabClient[numClient].pseudo);
    
        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

        sending(dSC, msg);
        printf("Envoi du message à %s\n", pseudo);       
    }
    free(pseudo);

}

int createSocketServeur(int port){
    /*Création de la socket */
	int dS = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in ad;
	ad.sin_family = AF_INET;
	ad.sin_addr.s_addr = INADDR_ANY;
	ad.sin_port = htons(port);

	/*Nommage de la socket*/
	int bindRFile = bind(dS, (struct sockaddr*)&ad, sizeof(ad));
	if (bindRFile == -1){
		perror("erreur au bind");
		exit(-1);
	}

    /*Passer la socket en mode écoute*/
	int listenRFile = listen(dS, MAX_CLIENT);
	if (listenRFile == -1){
		perror("erreur au listen");
		exit(-1);
	}  

    return dS;
}

int acceptConnection(int dS){
    int dSC;
    struct sockaddr_in aC;
    socklen_t lg = sizeof(struct sockaddr_in);
    dSC = accept(dS, (struct sockaddr*) &aC,&lg);
    if (dSC == -1){
        perror("erreur au accept");
        exit(-1);
    }
    return dSC;
}

void sendFile(int dS, FILE * fp){
    char * data[1024];
    int isEndSendFile = 0;
    int nbOctets = 0;

    /*descripteur de fichier à partir du FILE * */
    int fd = fileno(fp);
    printf("fd : %d\n", fd);

    /*récupérer la taille du fichier*/
    int size = lseek(fd, 0, SEEK_END);
    printf("taille du fichier : %d\n", size);

    /*tant qu'on n'a pas atteint la fin du fichier
     * faire un read (retourne 0 si on est en fin de fichier)
     * envoyer le bloc lu */
    do {
        nbOctets = read(fd, data, 1023);
        printf("retour read : %d\n",nbOctets);
        sendingInt(dS, isEndSendFile);

        if (send(dS, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }

        printf("après send \n");
        bzero(data, 1024);
    } while(nbOctets != 0);

    isEndSendFile = 1;
    sendingInt(dS, isEndSendFile);
    fclose(fp);
}