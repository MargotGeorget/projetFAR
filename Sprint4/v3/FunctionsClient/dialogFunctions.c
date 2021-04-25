#include "dialogFunctions.h"

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

void receiving(int dS, char * rep, ssize_t size){
    int recvR = recv(dS, rep, size, 0);
    if (recvR == -1){ /*vérification de la valeur de retour*/
        printf("** fin de la communication **\n");
        exit(-1);
    }
}

int receivingInt(long dS){
    int number;
    if(recv(dS, &number, sizeof(int), 0) == -1){ /*vérification de la valeur de retour*/
        perror("erreur au recv d'un int");
        exit(-1);
    } 
    return number;
}

int createSocketCLient(int port, char * ip){
    /*Création de la socket*/
	long dS = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in aS;
	aS.sin_family = AF_INET;
	inet_pton(AF_INET, (char *)port, &(aS.sin_addr));
	aS.sin_port = htons(atoi(ip));

	/*Demander une connexion*/
	socklen_t lgA = sizeof(struct sockaddr_in);
	int connectR = connect(dS, (struct sockaddr *) &aS, lgA);
	if (connectR == -1){
		perror("erreur au connect");
		exit(-1);
	}

    return dS;
}

void sendingFile(int dS){
    printf("envoi de fichier\n");

    /*Affichage des fichiers disponibles à l'envoi*/
    int cr = system("ls ./FileToSend");
    if(cr == -1){
        printf("commande echouée");
    }

    /*Saisie du nom du fichier au clavier*/
    char * fileName = (char *) malloc(sizeof(char)*100);
    printf("\nSaisissez le nom d'un fichier à envoyer : \n");
    fgets(fileName, 100, stdin);

    /*Envoi du nom du fichier au serveur*/
    sending(dS,fileName);
    fileName = strtok(fileName, "\n");
    
    /*Création du chemin pour trouver le fichier*/
    char * pathToFile = (char *) malloc(sizeof(char)*130);
    strcpy(pathToFile,"FileToSend/");
    strcat(pathToFile,fileName);

    /*Ouverture et envoi du fichier*/
    FILE * fp = NULL;
    fp = fopen(pathToFile,"r");
    if (fp== NULL) {   
        printf("Erreur! Fichier inconnu\n"); 
    }else {
        /*Création du thread d'envoi de fichier*/
        pthread_t threadFile;
        int thread = pthread_create(&threadFile, NULL, sendingFile_th, (void *)fp);
        if(thread==-1){
            perror("error thread");
        }
    }
}
