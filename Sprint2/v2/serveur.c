#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>


/*Compiler gcc -Wall -ansi -o serveur serveur.c*/
/*Lancer avec ./serveur votre_port */


typedef struct Client Client;
struct Client{
    char * pseudo;
    long dSC;
};

/* - MAX_CLIENT = nombre maximum de client accepté sur le serveur
 * - tabClient = tableau répertoriant les sockets des clients connectés
 * - tabThread = tableau des threads associés au traitement de chaque client
 * - nbClient = nombre de clients actuellement connectés*/
#define MAX_CLIENT 5
Client tabClient[MAX_CLIENT];
pthread_t tabThread[MAX_CLIENT];
long nbClient = 0;



/*
 * Envoi un message à toutes les sockets présentent dans le tableau des clients
 * et teste que tout se passe bien
 * Paramètres : int dS : expéditeur du message
 *              char * msg : message à envoyer
 * Retour : pas de retour
*/
void sending(int dS, char * msg){
    int i;
    for (i = 0; i<nbClient ; i++) {
        /*On n'envoie pas au client qui a écrit le message*/
        if(dS != tabClient[i].dSC){
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
 * Retour : 0 (faux) si le client veut quitter, 1 (vrai) sinon
*/
int endOfCommunication(char ** msg){
    if (strcmp(*msg, "fin\n")==0){
        *msg = "** a quitté la communication **\n";
        printf("%s",*msg);
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
    printf("pseudo: %s",tabClient[numClient].pseudo);

    while(!isEnd){
        /*Réception du message*/
        char * msgReceived = (char *) malloc(sizeof(char)*100);
        receiving(tabClient[numClient].dSC, msgReceived, sizeof(char)*100);
        printf("\nMessage recu: %s \n", msgReceived);

        /*On verifie si le client veut terminer la communication*/
        isEnd = endOfCommunication(&msgReceived);

        char * msgToSend = (char *) malloc(sizeof(char)*112);
        strcat(msgToSend, pseudoSender);
        strcat(msgToSend, msgReceived);
        printf("%s",msgToSend);
        printf("pseudo de la personne : %s",pseudoSender);

        printf("Envoi du message aux %ld clients. \n", nbClient);
        sending(tabClient[numClient].dSC, msgToSend);
        
    }

    return NULL;
}



/*
 * _____________________ MAIN _____________________
 * */
int main(int argc, char *argv[]) {
    /*Verification des paramètres*/
    if(argc<2){
        printf("Erreur : Lancez avec ./serveur <votre_port> ");
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
	int listenR = listen(dS, 7);
	if (listenR == -1){
		perror("erreur au listen");
		exit(-1);
	}


    while(1){
        int dSC;
        /*Tant qu'on peut accepter des clients */
        if(nbClient < MAX_CLIENT){
            /*Accepter une connexion*/
            struct sockaddr_in aC;
            socklen_t lg = sizeof(struct sockaddr_in);
            dSC = accept(dS, (struct sockaddr*) &aC,&lg);
            if (dSC == -1){
                perror("erreur au accept");
                exit(-1);
            }

            /*Affectation du numéro au client en fonction de l'ordre de connexion*/
            int numClient = nbClient + 1;
            if (send(dSC, &numClient, sizeof(int), 0) == -1){
                perror("erreur au send du numClient");
                exit(-1);
            }

            printf("Client %d connecté\n", numClient);
            /*On enregistre la socket du client*/
            tabClient[nbClient].dSC = dSC;

            /*Réception du pseudo*/
            char * pseudo = (char *) malloc(sizeof(char)*100);
            receiving(dSC, pseudo, sizeof(char)*100);

            /*On enregistre le pseudo du client*/
            pseudo = strtok(pseudo, "\n");
            tabClient[nbClient].pseudo = pseudo;

            /*On envoi un message pour avertir les autres clients de l'arriver du nouveau client*/
            printf("\nPseudo recu: %s \n", pseudo);
            strcat(pseudo," à rejoint la communication\n");
            printf("%s",pseudo);
            printf("Envoi du message aux %ld autres clients. \n", nbClient);
            sending(dSC, pseudo);

            free(pseudo);

            /*_____________________ Communication _____________________*/
            int threadReturn = pthread_create(&tabThread[nbClient],NULL,broadcast,(void *)nbClient);
            if(threadReturn == -1){
                perror("erreur thread create");
            }

            /*On a un client en plus sur le serveur, on incrémente*/
            nbClient += 1;
            
            printf("Clients connectés : %ld\n", nbClient);
            

        }

    }
	close(dS);
    return 0;
}
