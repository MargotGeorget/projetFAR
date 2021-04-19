#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>


/*Compiler gcc -pthread -Wall -ansi -o serveur serveur.c*/
/*Lancer avec ./serveur votre_port */

/*
 * Définition d'une structure Client pour regrouper toutes les informations du client
 * */
typedef struct Client Client;
struct Client{
    int occupied;
    char * pseudo;
    long dSC;
};

/*   Initialisation des variables :
 * - MAX_CLIENT = nombre maximum de client accepté sur le serveur
 * - tabClient = tableau répertoriant les clients connectés
 * - tabThread = tableau des threads associés au traitement de chaque client
 * - semNbClient = sémaphore qui gère le nombre de clients actuellement connectés
 * - tabThreadToKill = 
 * - lock = 
 * */
#define MAX_CLIENT 5
Client tabClient[MAX_CLIENT];
pthread_t tabThread[MAX_CLIENT];
sem_t semNbClient;
int tabThreadToKill[MAX_CLIENT];

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/*_____Fonctions pour gérer le tableau de Clients_____*/

/*
 * Fonction pour gérer les indices du tableaux de clients 
 * Retour : un entier, indice du premier emplacement disponible
 *          -1 si tout les emplecements sont occupés. 
 */
int giveNumClient(){
    int i = 0;
    int indice = -1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<MAX_CLIENT && indice==-1){
        if(!tabClient[i].occupied){
            indice = i;
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return indice;
}

/*
 * Fonctions pour trouver un client dans le tableau à partir de son pseudo
 * Paramètre : char * pseudo = le pseudo du client que l'on cherche 
 * Retour : un entier, correspondant à la socket du client trouvé
 *          -1 si le client n'existe pas 
 */
int findClient(char * pseudo){
    int i = 0;
    int client = -1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<MAX_CLIENT && client==-1){
        if (tabClient[i].occupied){
            if (strcmp(pseudo, tabClient[i].pseudo)==0){
                client = tabClient[i].dSC;
            }
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return client;
}

/* 
 * Fonction qui vérifie si le pseudo saisi n'est pas déjà utilisé 
 * Retour: 1 si le pseudo n'est pas encore utilisé, 0 sinon 
 */
int isAvailablePseudo(char * pseudo){
    int i= 0;
    int available = 1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/
    
    while (i<MAX_CLIENT && available){
        
        if(tabClient[i].occupied){
            if(strcmp(tabClient[i].pseudo,pseudo)==0){
                available = 0;
            }
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
    
    return available;
}

/*
 * Fonction pour mettre en forme le message à envoyer avec le pseudo de l'expéditeur 
 * Paramètres : char * : le message à envoyer 
 *              char * : le pseudo à ajouter 
 * Pas de retour
 */
void addPseudoToMsg(char * msg, char * pseudoSender){
    char * msgToSend = (char *) malloc(sizeof(char)*150);
    strcpy(msgToSend, pseudoSender);
    strcat(msgToSend, " : ");
    strcat(msgToSend, msg);
    strcpy(msg,msgToSend);
    free(msgToSend);
    return;
}


/*_____Fonction pour gérer les envois et les receptions_____*/

/*
 * Receptionne un message d'une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              char * msg : message à recevoir
 *              ssize_t size : taille maximum du message à recevoir
 * Retour : pas de retour
*/
void receiving(int dS, char * rep, ssize_t size){
    int recvR = recv(dS, rep, size, 0);
    if (recvR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au recv");
        exit(-1);
    }
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

/*
 * Envoi un entier à une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              int number : entier à envoyer
 * Retour : pas de retour
 * */
void sendingInt(int dS, int number){
    int sendR = send(dS, &number, sizeof(int), 0);
    if (sendR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au send");
        exit(-1);
    }
}

/*
 * Envoi un message à toutes les sockets présentent dans le tableau des clients
 * et teste que tout se passe bien
 * Paramètres : int numClient: expéditeur du message
 *              char * msg : message à envoyer
 * Retour : pas de retour
*/
void sendingAll(int numClient, char * msg){

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    int dS = tabClient[numClient].dSC;

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

/*
 * Envoi un message à un seul client
 * et teste que tout se passe bien
 * Paramètres : int numClient : expéditeur du message
 *              char * msg : message à envoyer contenant un @ suivi d'un pseudo
 * Retour : pas de retour
*/
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
        error = "Le pseudo saisit n'existe pas!\n";
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

/*
 * Vérifie si un client souhaite quitter la communication
 * Paramètres : char ** msg : message du client à vérifier
 * Retour : 1 (vrai) si le client veut quitter, 0 (faux) sinon
*/
int endOfCommunication(char * msg){
    if (strcmp(msg, "** a quitté la communication **\n")==0){
        return 1;
    }
    return 0;
}


/*_____Fonction pour gérer les threads_____*/
/*
 * Start routine de pthread_create()
 * */
void * broadcast(void * clientParam){
    int isEnd = 0;
    int numClient = (long) clientParam;

    while(!isEnd){
        /*Réception du message*/
        char * msgReceived = (char *) malloc(sizeof(char)*100);
        receiving(tabClient[numClient].dSC, msgReceived, sizeof(char)*100);
        printf("\nMessage recu: %s \n", msgReceived);

        /*On verifie si le client veut terminer la communication*/
        isEnd = endOfCommunication(msgReceived);

        char first = msgReceived[0];
        if(strcmp(&first,"@")==0){
            sendingPrivate(numClient, msgReceived);
        }else {
            /*Envoi du message aux autres clients*/
            printf("Envoi du message aux autres clients. \n");
            sendingAll(numClient, msgReceived);
        }
    }

    /*Fermeture du socket client*/
    sem_post(&semNbClient);
    pthread_mutex_lock(&lock);
    tabClient[numClient].occupied=0;
    tabThreadToKill[numClient]=1;
    close(tabClient[numClient].dSC);
    pthread_mutex_unlock(&lock);
    return NULL;
}

/*
* Fonction qui tue les threads pour lesquels les clients se sont déconnectés
*/
void killThread(){
    pthread_mutex_lock(&lock);
    int i;
    for (i=0;i<MAX_CLIENT;i++){
        if(tabThreadToKill[i]){
            pthread_cancel(tabThread[i]);
            tabThreadToKill[i]=0;
        }
    }
    pthread_mutex_unlock(&lock);
}

/*
 * _____________________ MAIN _____________________
 * */
int main(int argc, char *argv[]) {

    sem_init(&semNbClient, 0, MAX_CLIENT);

    /*Verification des paramètres*/
    if(argc<2){
        printf("Erreur : Lancez avec ./serveur <votre_port> ");
    }

    /*Initialisation du tableau qui informe des threads à kill à false pour chaque indice*/
    int i;
    for(i=0;i<MAX_CLIENT;i++){
        tabThreadToKill[i]=0;
    }

	/*Création de la socket*/
	int dS = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in ad;
	ad.sin_family = AF_INET;
	ad.sin_addr.s_addr = INADDR_ANY;
	ad.sin_port = htons(atoi(argv[1]));

	/*Nommage de la socket*/
	int bindR = bind(dS, (struct sockaddr*)&ad, sizeof(ad));
	if (bindR == -1){
		perror("erreur au bind");
		exit(-1);
	}

	/*Passer la socket en mode écoute*/
	int listenR = listen(dS, MAX_CLIENT);
	if (listenR == -1){
		perror("erreur au listen");
		exit(-1);
	}


    while(1){
        int dSC;

        /*Tant qu'on peut accepter des clients */
        sem_wait(&semNbClient); 

        /*On tue les threads pour lesquels les clients ont quitté la connexion*/
        killThread();   

        /*Accepter une connexion*/
        struct sockaddr_in aC;
        socklen_t lg = sizeof(struct sockaddr_in);
        dSC = accept(dS, (struct sockaddr*) &aC,&lg);
        if (dSC == -1){
            perror("erreur au accept");
            exit(-1);
        }

    /*On gère l'arrivée du client avant de créer son thread*/

        /*Récupération du nombre de client connectés*/
        int valueSem;
        sem_getvalue(&semNbClient, &valueSem);
        int nbClient = MAX_CLIENT-valueSem;

        /*Affectation du numéro au client en fonction des emplacements dans le tableau de Clients*/
        long numClient = giveNumClient();

        /*Envoi du nombre de client présent au nouveau client*/
        sendingInt(dSC, nbClient);
                
        printf("Client %ld connecté\n", numClient);

        /*Réception du pseudo*/

        char * pseudo = (char *) malloc(sizeof(char)*100);
        int availablePseudo = 0; /*false*/

        do {
            sendingInt(dSC,availablePseudo);
            receiving(dSC, pseudo, sizeof(char)*12);
            pseudo = strtok(pseudo, "\n");
            availablePseudo = isAvailablePseudo(pseudo);

        } while(!availablePseudo);

        /*Pseudo valide*/
        sendingInt(dSC,availablePseudo);

        /*On enregistre le pseudo du client*/
        
        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        tabClient[numClient].pseudo = (char *) malloc(sizeof(char)*12);
        strcpy(tabClient[numClient].pseudo,pseudo);

        /*On enregistre la socket du client*/
        tabClient[numClient].occupied = 1;
        tabClient[numClient].dSC = dSC;

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

        /*On envoi un message pour avertir les autres clients de l'arrivée du nouveau client*/
        strcpy(pseudo,"** a rejoint la communication **\n");
        sendingAll(numClient, pseudo);    

        printf("%s\n",pseudo);

        free(pseudo);

        /*_____________________ Communication _____________________*/
        int threadReturn = pthread_create(&tabThread[numClient],NULL,broadcast,(void *)numClient);
        if(threadReturn == -1){
            perror("erreur thread create");
        }
    
        printf("Clients connectés : %d\n", nbClient);
        
    }
    sem_destroy(&semNbClient);
	close(dS);
    return 0;
}
