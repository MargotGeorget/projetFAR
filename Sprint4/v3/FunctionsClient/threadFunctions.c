#include "threadFunctions.h"

void * sendingFile_th(void * fileNameParam){

    /*Création de la socket*/
	long dSFile = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in aS;
	aS.sin_family = AF_INET;
	inet_pton(AF_INET, arg1, &(aS.sin_addr));
	aS.sin_port = htons(atoi(arg2)+1);

    /*Demander une connexion*/
	socklen_t lgA = sizeof(struct sockaddr_in);
	int connectR = connect(dSFile, (struct sockaddr *) &aS, lgA);
	if (connectR == -1){
		perror("erreur au connect");
		exit(-1);
	}

    char * fileName = (char *)fileNameParam;

    printf("J'envoi le fichier %s au serveur avec le socket %ld\n",fileName,dSFile);
    
    /*Création du chemin pour trouver le fichier*/
    char * pathToFile = (char *) malloc(sizeof(char)*130);
    strcpy(pathToFile,"FileToSend/");
    strcat(pathToFile,fileName);

    /*Ouverture et envoi du fichier*/
    FILE * fp = NULL;
    fp = fopen(pathToFile,"r");
    if (fp== NULL) {   
        printf("Error! Could not open file\n"); 
        exit(-1); 
    }
    char data[1024] = "";
    /*Booleen pour controler la fin de l'envoi du fichier*/
    int isEndSendFile = 0;

    while(fgets(data, 1024, (FILE *)fp) != NULL) {
        if (send(dSFile, &isEndSendFile, sizeof(int), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        if (send(dSFile, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        sleep(2);
        bzero(data, 1024);
    }
    isEndSendFile = 1;
    if (send(dSFile, &isEndSendFile, sizeof(int), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        
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
            printf("envoi de fichier\n");
            int cr = system("ls ./FileToSend");
            if(cr == -1){
                printf("commande echouée");
            }

            /*Saisie du nom du fichier au clavier*/
            char * fileName = (char *) malloc(sizeof(char)*100);

            printf("\nSaisissez le nom d'un fichier à envoyer : \n");
            fgets(fileName, 100, stdin);
            sending(dS,fileName);
            fileName = strtok(fileName, "\n");

            pthread_t threadFile;
            int thread = pthread_create(&threadFile, NULL, sendingFile_th, (void *)fileName);
            if(thread==-1){
                perror("error thread");
            }
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

