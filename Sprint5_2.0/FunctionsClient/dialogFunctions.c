#include "dialogFunctions.h"

void sending(int dS, char * msg){
    /*msg[strcspn(msg, "\n")] = 0;*/
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

int createSocketClient(int port, char * ip){

    /*Création de la socket*/
	long dS = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in aS;
	aS.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(aS.sin_addr));
	aS.sin_port = htons(port);

	/*Demander une connexion*/
	socklen_t lgA = sizeof(struct sockaddr_in);
	int connectR = connect(dS, (struct sockaddr *) &aS, lgA);
	if (connectR == -1){
		perror("erreur au connect");
		exit(-1);
	}

    return dS;
}

void uploadFile(int dS){
    printf("\n ----- Listes de fichiers disponibles à l'envoi ----- \n");

    /*Affichage des fichiers disponibles à l'envoi*/
    int cr = system("ls --format=single-column ./FileToSend");
    if(cr == -1){
        printf("commande echouée");
    }

    /*Saisie du nom du fichier au clavier*/
    char * fileName = (char *) malloc(sizeof(char)*100);
    printf("\n --- Saisissez le nom d'un fichier à envoyer : \n");
    fgets(fileName, 100, stdin);
    strtok(fileName,"\n");
    strcat(fileName,"\0");
    
    /*Création du chemin pour trouver le fichier*/
    char * pathToFile = (char *) malloc(sizeof(char)*130);
    strcpy(pathToFile,"FileToSend/");
    strcat(pathToFile,fileName);

    /*Ouverture et envoi du fichier*/
    FILE * fp = NULL;
    fp = fopen(pathToFile,"r");

    if (fp== NULL) { /*Le nom du fichier saisi est incorrecte, on prévient le serveur*/
        char * error = "error";
        sending(dS, error);  
        printf("Erreur! Fichier inconnu\n"); 

    }else { /*Le fichier s'est ouvert correctement, on peut l'envoyer au serveur*/
        /*Envoi du nom du fichier au serveur*/
        sending(dS,fileName);
        /*Création du thread d'envoi de fichier*/
        pthread_t threadFile;
        int thread = pthread_create(&threadFile, NULL, uploadFile_th, (void *)fp);
        if(thread==-1){
            perror("error thread");
        }
    }
    free(pathToFile);
    free(fileName);
    /*ToDo join pour close fp*/
}

void downloadFile(int dS){
    /*Reception et affichage d'un fichier contenant les noms de tout les fichiers pouvant être téléchargés*/ 
    printf("\n ----- Listes de fichiers disponibles au téléchargement ----- \n");
    /*Booleen pour controler la fin de la reception du fichier*/
    int nbBytes = receivingInt(dS);
    char buffer[1024];
    /*Reception et affichage*/
    while(nbBytes>0){
        recv(dS, buffer, 1024, 0);
        printf("%s\n",buffer);
        /*buffer[nbBytes-2]='\0';*/
        nbBytes = receivingInt(dS);
        bzero(buffer, 1024);
    }
    printf("\n --- Saisissez le nom d'un fichier à télécharger : \n");
    /*La saisie se fait coté sending (thread)*/

    char * fileName = (char *) malloc(sizeof(char)*100);
    receiving(dS,fileName,sizeof(char)*100);
    if(strcmp(fileName,"error")==0){ /*Le nom de fichier saisi est incorrecte*/
        printf("Nom de fichier saisit incorrect!\n");
    }else { /*Le nom de fichier saisi est correcte, on peut recevoir le fichier*/
        /*Création du thread de reception de fichier*/
        pthread_t threadFile;
        int thread = pthread_create(&threadFile, NULL, downloadFile_th, (void *)fileName);
        if(thread==-1){
            perror("error thread");
        } 
    }
    return;
}
