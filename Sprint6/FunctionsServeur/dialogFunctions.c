#include "dialogFunctions.h"

void Ctrl_C_Handler(int sign) {
    /*Sauvegarde des fichiers avant d'éteindre le serveur*/
    saveClients();
    updateRoom(1,1,1);
    /*Envoi de message aux clients pour les prévenir de l'arrêt du serveur*/
    int i; 
    for(i = 0; i<MAX_CLIENT;i++){
        if(tabClient[i].connected){
            sending(tabClient[i].dSC,"/shutdown");
            close(tabClient[i].dSC);
        }
    }
    shutdown(dS, 2);
    shutdown(dSFile, 2); /*On ferme aussi la connexion de la socket utilisée pour les fichiers*/

    printf("\n CTRL+C détecté : Arrêt du serveur \n");
    exit(EXIT_SUCCESS);
}

void receiving(int dS, char * rep, ssize_t size){
    /*Réception d'un message de taille 'size', de la socket 'dS' et stockage du message dans 'rep'*/
    int recvR = recv(dS, rep, size, 0);
    if (recvR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au recv");
        exit(-1);
    }
}

int receivingInt(long dS){
    /*Réception d'un entier de la socket 'dS' et retour de l'entier*/
    int number;
    int recvR = recv(dS, &number, sizeof(int), 0);
    if( recvR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au recv d'un int");
        exit(-1);
    }else if( recvR == 0){
        closingClient(dS);
    }
    return number;
}

void sending(int dS, char * msg){
    /*Envoi de message 'msg' à la socket 'dS'*/
    int sendR = send(dS, msg, strlen(msg)+1, 0);
    if (sendR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au send");
        exit(-1);
    }
}

void sendingInt(int dS, int number){
    /*Envoi de l'entier 'number' à la socket 'dS'*/
    int sendR = send(dS, &number, sizeof(int), 0);
    if (sendR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au send");
        exit(-1);
    }
}

void sendingAll(int numClient, char * msg){

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    int dS = tabClient[numClient].dSC;

    addPseudoToMsg(msg, tabClient[numClient].pseudo);
    int i;
    for (i = 0; i<MAX_CLIENT ; i++) {
        /*On envoie le message à tout les clients qui sont connectés (connected==1) 
        mais on ne l'envoie pas au client qui a écrit le message*/
        if(tabClient[i].connected && dS != tabClient[i].dSC){
            sending(tabClient[i].dSC, msg);
        }
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

}

void sendingRoom(int numClient, char * msg){

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    int dS = tabClient[numClient].dSC;

    addPseudoToMsg(msg, tabClient[numClient].pseudo);

    int i;
    int idRoom = tabClient[numClient].idRoom;
    for (i = 0; i<MAX_CLIENT ; i++) {

        /*On envoie le message à tout les clients présent dans le salon  
        mais on ne l'envoie pas au client qui a écrit le message*/
        if(rooms[idRoom].members[i] && dS != tabClient[i].dSC){
            sending(tabClient[i].dSC, msg);
        }
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

}

void sendingPrivate(int numClient, char * msg){

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    int mydSC = tabClient[numClient].dSC;

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    /*Récupération du pseudo présent au début du message*/
    char * pseudo = (char *) malloc(SIZE_MSG);
    strcpy(pseudo, msg);
    pseudo = strtok(pseudo," ");
    strcpy(pseudo,pseudo+1); /*Pour enlever le @*/

    int client = findClient(pseudo);

    if (client==-1){ /*Aucun client n'a été trouvé*/

        sending(mydSC, "Le pseudo saisi n'existe pas!\n");

    }else { /*Le client à été trouvé*/

        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        /*Ajout du pseudo de l'expéditeur devant le message à envoyer*/
        /*addPseudoToMsg(msg, tabClient[numClient].pseudo);*/
    
        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

        sending(tabClient[client].dSC, msg);
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
    char data[1024];
    int nbBytes = -1;

    /*descripteur de fichier à partir du FILE * */
    int fd = fileno(fp);

    /*tant qu'on n'a pas atteint la fin du fichier
     * faire un read (retourne 0 si on est en fin de fichier)
     * envoyer le bloc lu */
    while(nbBytes != 0){
        nbBytes = read(fd, data, 1023);
        data[1023]='\0';
        sendingInt(dS, nbBytes);
        if(nbBytes != 0){
            if (send(dS, data, sizeof(data), 0) == -1) {
                perror("[-]Error in sending file.");
                exit(1);
            }
        }
        bzero(data, nbBytes);
    } 
    printf("\n** Fichier envoyé **\n");
    fclose(fp);
}

void uploadFile(int dS){
    /*Reception du nom du fichier à recevoir*/
    char * fileName = (char *) malloc(SIZE_MSG);
    receiving(dS, fileName, SIZE_MSG);

    printf("\nNom du fichier à recevoir: %s \n", fileName);

    if(strcmp(fileName,"error")==0){ /*Le nom du fichier saisie par le client est incorrect*/
        printf("Nom de fichier incorrect\n");

    }else{ /*Le nom du fichier saisie par le client est correct, on peut recevoir le contenu*/

        /*Création du thread pour gérer la reception du fichier*/
        pthread_t threadFile;
        int thread = pthread_create(&threadFile, NULL, uploadFile_th, (void *)fileName);
        if(thread==-1){
            perror("error thread");
        }   
    }
}

void downloadFile(int dS,char * msgReceived){

    /*Envoi du message au thread de reception du client*/
    sending(dS,msgReceived);

    /*Envoi des fichiers pouvant être téléchargés*/
    int cr = system("ls ./FileServeur > listeFichier.txt");
    if(cr == -1){
        printf("commande echouée");
    }
    
    FILE * fp = fopen("listeFichier.txt", "r");
    sendFile(dS,fp);

    /*Reception du nom du fichier à envoyer*/
    char * fileName = (char *) malloc(SIZE_MSG);
    receiving(dS, fileName, SIZE_MSG);
    printf("\nNom du fichier à envoyer: %s \n", fileName);


    /*Création du chemin pour trouver le fichier*/
    char * pathToFile = (char *) malloc(sizeof(char)*130);
    strcpy(pathToFile,"FileServeur/");
    strcat(pathToFile,fileName);

    /*Ouverture et envoi du fichier*/
    fp = fopen(pathToFile,"r");
    if (fp== NULL) { /*Le fichier n'a pas été trouvé*/
        char * error = "error";
        printf("Erreur! Fichier inconnu\n"); 
        /*On informe le client de l'erreur*/
        sending(dS,error);
    }else {
        /*Le fichier va être envoyé, on informe le client et on transmet au thread de reception le nom du fichier*/
        sending(dS,fileName);

        /*Création du thread d'envoi de fichier*/
        pthread_t threadFile;
        int thread = pthread_create(&threadFile, NULL, downloadFile_th, (void *)fp);
        if(thread==-1){
            perror("error thread");
        }
    }
    free(pathToFile);
}