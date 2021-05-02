#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>

/*Compiler gcc -pthread -Wall -ansi -o client client.c*/
/*Lancer avec ./client votre_ip votre_port*/

int isEnd = 0;


/*
 * Vérifie si un client souhaite quitter la communication
 * Paramètres : char ** msg : message du client à vérifier
 * Retour : 1 (vrai) si le client veut quitter, 0 (faux) sinon
*/
int endOfCommunication(char * msg){
    if (strcmp(msg, "fin\n")==0){
        strcpy(msg, "** a quitté la communication **\n");
        return 1;
    }
    return 0;
}

/*
 * Vérifie si un client souhaite envoyer un fichier
 * Paramètres : char ** msg : message du client à vérifier
 * Retour : 1 (vrai) si le client veut envoyer un fichier, 0 (faux) sinon
*/
int isSendingFile(char * msg){
    if (strcmp(msg, "/file\n")==0){
        return 1;
    }
    return 0;
}

/*
 * Envoi un message à une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              char * msg : message à envoyer
 * Retour : pas de retour
 * */
void sending(int dS, char * msg){
    int sendR = send(dS, msg, strlen(msg)+1, 0);
    if (sendR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au send");
        exit(-1);
    }
}

void sendingFile(int dS){

    /*Saisie du nom du fichier au clavier*/
    char * fileName = (char *) malloc(sizeof(char)*100);

    printf("\nSaisissez le nom d'un fichier à envoyer : \n");
    fgets(fileName, 100, stdin);
    sending(dS,fileName);
    fileName = strtok(fileName, "\n");

    /*Création du chemin pour trouver le fichier*/
    char * pathToFile = (char *) malloc(sizeof(char)*130);
    strcpy(pathToFile,"FileToSend/");
    strcat(pathToFile,fileName);
    printf("%s",pathToFile);

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
        printf("%s\n",data);
        if (send(dS, &isEndSendFile, sizeof(int), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        if (send(dS, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        
        bzero(data, 1024);
    }
    isEndSendFile = 1;
    if (send(dS, &isEndSendFile, sizeof(int), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        
    fclose(fp);
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
        sending(dS, m);

        if (isSendingFile(m)){
            printf("envoi de fichier\n");
            int cr = system("ls ./FileToSend");
            if(cr == -1){
                printf("commande echouée");
            }
            sendingFile(dS);
        }

        free(m);
    }
    close(dS);
    return NULL;
}


/*
 * Receptionne un message d'une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              char * msg : message à recevoir
 *              ssize_t size : taille maximum du message à recevoir
 * Retour : pas de retour
 * */
void receiving(int dS, char * rep, ssize_t size){
    int recvR = recv(dS, rep, size, 0);
    if (recvR == -1){ /*vérification de la valeur de retour*/
        printf("** fin de la communication **\n");
        exit(-1);
    }
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


/*
 * _____________________ MAIN _____________________
 * */
int main(int argc, char *argv[]) {

    /*Verification des paramètres*/
    if(argc<3){
        printf("Erreur : Lancez avec ./client <votre_ip> <votre_port> ");
    }

	/*Création de la socket*/
	long dS = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in aS;
	aS.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &(aS.sin_addr));
	aS.sin_port = htons(atoi(argv[2]));

	/*Demander une connexion*/
	socklen_t lgA = sizeof(struct sockaddr_in);
	int connectR = connect(dS, (struct sockaddr *) &aS, lgA);
	if (connectR == -1){
		perror("erreur au connect");
		exit(-1);
	}

    /*Reception du nombre de client*/
    int nbClient;
    if (recv(dS, &nbClient, sizeof(int), 0) == -1){ /*vérification de la valeur de retour*/
        perror("erreur au recv du numClient");
        exit(-1);
    }
        
    /*Saisie du pseudo du client au clavier*/
    int availablePseudo;
    char * myPseudo = (char *) malloc(sizeof(char)*12);
    recv(dS,&availablePseudo,sizeof(int),0);
   
    printf("Votre pseudo (maximum 12 caractères): ");
    fgets(myPseudo, 12, stdin);
    sending(dS,myPseudo);
    recv(dS,&availablePseudo,sizeof(int),0);
   
    while(!availablePseudo){
        printf("Pseudo déjà utilisé!\nVotre pseudo (maximum 12 caractères): ");
        fgets(myPseudo, 12, stdin);
        sending(dS,myPseudo);
        recv(dS,&availablePseudo,sizeof(int),0);
    }
    
    /*En attente d'un autre client*/
    if(nbClient==1){
        printf("En attente d'un autre client\n");

        /*Reception du premier message informant de l'arrivée d'un autre client*/
        char * msg = (char *) malloc(sizeof(char)*100);
        receiving(dS, msg, sizeof(char)*100);
        printf("%s",msg);

        free(msg);
    }

    /*_____________________ Communication _____________________*/

    /*Echange des messages*/

    /*Création d'un thread d'envoi*/
    pthread_t thread_sendind;
    pthread_t thread_receiving;

    int thread1 = pthread_create(&thread_sendind, NULL, sending_th, (void *)dS);
    if(thread1==-1){
        perror("error thread 1");
    }

    /*Création d'un thread de reception*/
    int thread2 = pthread_create(&thread_receiving, NULL, receiving_th, (void *)dS);
    if(thread2==-1){
        perror("error thread 2");
    }

    /*Attente de la fin des threads*/
    pthread_join(thread_sendind, NULL);
    pthread_join(thread_receiving, NULL);
    
    /*Fin de la communication**/
    return 0;
}
