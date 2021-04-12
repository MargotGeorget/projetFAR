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

/*
* Fonctions pour gérer les indices du tableaux de clients 
* Retour : un entier, indice du premier emplacement disponible
*          -1 si tout les emplecements sont occupés. 
*/
int giveNumClient(){
    int i = 0;
    while (i<MAX_CLIENT){
        if(!tabClient[i].occupied){
            return i;
        }
        i+=1;
    }
    
    return -1;
}

/*
 * Envoi un message à toutes les sockets présentent dans le tableau des clients
 * et teste que tout se passe bien
 * Paramètres : int dS : expéditeur du message
 *              char * msg : message à envoyer
 * Retour : pas de retour
*/
void sending(int dS, char * msg){
    int i;
    for (i = 0; i<MAX_CLIENT ; i++) {
        /*On n'envoie pas au client qui a écrit le message*/
        if(tabClient[i].occupied && dS != tabClient[i].dSC){
            int sendR = send(tabClient[i].dSC, msg, strlen(msg)+1, 0);
            if (sendR == -1){ /*vérification de la valeur de retour*/
                perror("erreur au send");
                exit(-1);
            }
        }
    }
}

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

/*
 * Start routine de pthread_create()
 * */
void * broadcast(void * clientParam){
    int isEnd = 0;
    int numClient = (long) clientParam;
    char * pseudoSender = tabClient[numClient].pseudo;

    while(!isEnd){
        /*Réception du message*/
        char * msgReceived = (char *) malloc(sizeof(char)*100);
        receiving(tabClient[numClient].dSC, msgReceived, sizeof(char)*100);
        printf("\nMessage recu: %s \n", msgReceived);

        /*On verifie si le client veut terminer la communication*/
        isEnd = endOfCommunication(msgReceived);

        /*Ajout du pseudo de l'expéditeur devant le message à envoyer*/
        char * msgToSend = (char *) malloc(sizeof(char)*115);
        strcat(msgToSend, pseudoSender);
        strcat(msgToSend, " : ");
        strcat(msgToSend, msgReceived);

        /*Envoi du message aux autres clients*/
        printf("Envoi du message aux autres clients. \n");
        sending(tabClient[numClient].dSC, msgToSend);
        
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
 * Fonction qui vérifie si le pseudo saisie n'est pas déjà utilisé 
 * Retour: 1 si le pseudo n'est pas encore utilisé, 0 sinon 
 */
int avalablePseudo(char * pseudo){
    int i;
    for (i=0; i<MAX_CLIENT; i++){
        printf("%d\n",tabClient[i].occupied);
        if(tabClient[i].occupied){
            printf("%s",tabClient[i].pseudo);
            if(strcmp(tabClient[i].pseudo,pseudo)==0){
                return 0;
            }
        }
    }
    return 1;
}

/*
 * _____________________ MAIN _____________________
 * */
int main(int argc, char *argv[]) {

    killThread();
    sem_init(&semNbClient, 0, 5);

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

        /*Accepter une connexion*/
        struct sockaddr_in aC;
        socklen_t lg = sizeof(struct sockaddr_in);
        dSC = accept(dS, (struct sockaddr*) &aC,&lg);
        if (dSC == -1){
            perror("erreur au accept");
            exit(-1);
        }

    /*On gère l'arrivée du client avant de créer son thread*/

        /*Affectation du numéro au client en fonction des emplacements dans le tableau de Clients*/
        long numClient = giveNumClient();
        int valueSem;
        sem_getvalue(&semNbClient, &valueSem);
        int nbClient = MAX_CLIENT-valueSem;
        /*Envoi du nombre de client présent au nouveau client*/
        if (send(dSC, &nbClient, sizeof(int), 0) == -1){
            perror("erreur au send du nbClient");
            exit(-1);
        }
        
        printf("Client %ld connecté\n", numClient);

        /*Réception du pseudo*/

        char * pseudo = (char *) malloc(sizeof(char)*100);
        int avalableP = 0;
        do {
            if(send(dSC,&avalableP, sizeof(int),0)==-1){
                perror("erreur au send du nbClient");
                exit(-1);
            }
            receiving(dSC, pseudo, sizeof(char)*12);
            pseudo = strtok(pseudo, "\n");
            avalableP=avalablePseudo(pseudo);
        } while(!avalableP);
        send(dSC,&avalableP, sizeof(int),0);


        /*On enregistre le pseudo du client*/
        
        pthread_mutex_lock(&lock);
        tabClient[numClient].pseudo = (char *) malloc(sizeof(char)*12);
        printf("%s",pseudo);
        strcpy(tabClient[numClient].pseudo,pseudo);
        printf("%s",tabClient[numClient].pseudo);
        /*On enregistre la socket du client*/
        tabClient[numClient].occupied = 1;
        tabClient[numClient].dSC = dSC;
        pthread_mutex_unlock(&lock);
        printf("%s",tabClient[numClient].pseudo);

        /*On envoi un message pour avertir les autres clients de l'arriver du nouveau client*/
        strcat(pseudo," à rejoint la communication\n");
        sending(dSC, pseudo);

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
