#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/*Compiler gcc -Wall -ansi -o client client.c*/
/*Lancer avec ./client votre_ip votre_port*/

int isEnd = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int endOfCommunication(char * msg, char * msgToCmp){
    if (strcmp(msg, msgToCmp)==0){
        strcpy(msg, "** a quitté la communication **\n");
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

/* -- Fonction pour le thread d'envoi -- */
void * sending_th(void * dSparam){
    int dS = (long)dSparam;
    while (!isEnd){

        /*Saisie du message au clavier*/
        char * m = (char *) malloc(sizeof(char)*100);

        printf(">");
        fgets(m, 100, stdin);
        isEnd = endOfCommunication(m,"fin\n");
        
        /*Envoi*/
        sending(dS, m);

        free(m);
    }
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
        perror("erreur au recv");
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
        isEnd = endOfCommunication(r, "** a quitté la communication **\n");
        free(r);
    }
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

    /*Reception du numéro du client*/
    int numClient = 0;
    if (recv(dS, &numClient, sizeof(int), 0) == -1){ /*vérification de la valeur de retour*/
        perror("erreur au recv du numClient");
        exit(-1);
    }

    printf("Vous êtes le client numéro %d. \n", numClient);

    /*Saisie du pseudo du client au clavier*/
    char * myPseudo = (char *) malloc(sizeof(char)*12);
    printf("Votre pseudo (maximum 12 caractères): ");
    fgets(myPseudo, 12, stdin);

    /*Envoi du message*/
    sending(dS, myPseudo);

    /*En attente d'un autre client*/
    if(numClient==1){
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

    /*Fin de la communication*/
	close(dS);
    return 0;
}
