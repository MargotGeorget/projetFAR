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

long tabClient[2];

/*
 * Envoi un message à une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              char * msg : message à envoyer
 * Retour : pas de retour
*/
void sending(int dS, char * msg){
    int sendR = send(dS, msg, strlen(msg)+1, 0);
    if (sendR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au send");
        exit(-1);
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



void * client1ToClient2(void * dSCparam){
    int dSC = (long)dSCparam;
    int isEnd = 0;
    while(!isEnd){
        /*Reception du message du client1*/
        char * msg = (char *) malloc(sizeof(char)*100);
        receiving(dSC, msg, sizeof(char)*100);
        printf("\nMessage recu du client1: %s \n", msg);

        /*On verifie si le client 1 veut terminer la communication*/
        isEnd = endOfCommunication(&msg);

        /*Envoi du message au client2*/
        sending(tabClient[1], msg);
        printf(" -- Message envoye\n");
    }
    close(tabClient[1]);
    kill(getpid(),SIGKILL);
    return EXIT_SUCCESS;
}

void * client2ToClient1(void * dSCparam){
    int dSC = (long)dSCparam;
    int isEnd = 0;
    while(!isEnd){
        /*Reception du message du client2*/
        char * msg = (char *) malloc(sizeof(char)*100);
        receiving(dSC, msg, sizeof(char)*100);
        printf("\nMessage recu du client2: %s \n", msg);

        /*On verifie si le client 2 veut terminer la communication*/
        isEnd = endOfCommunication(&msg);

        /*Envoi du message au client1*/
        sending(tabClient[0], msg);
        printf(" -- Message envoye\n");
    }
    close(tabClient[0]);
    printf("%d", getpid());
    kill(getpid(),SIGKILL);
    return EXIT_SUCCESS;
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
        /*Accepter une premiere connexion*/
        struct sockaddr_in aC;
        socklen_t lg = sizeof(struct sockaddr_in);
        int dSC1 = accept(dS, (struct sockaddr*) &aC,&lg);
        if (dSC1 == -1){
            perror("erreur au accept");
            exit(-1);
        }
        tabClient[0]=dSC1;

        /*Affectation du numéro au client en fonction de l'ordre de connexion*/
        int numClient = 1;
        if (send(dSC1, &numClient, sizeof(int), 0) == -1){
            perror("erreur au send du numClient");
            exit(-1);
        }

        printf("Client 1 connecté\n");

        /*Accepter une deuxieme connexion*/
        int dSC2 = accept(dS, (struct sockaddr*) &aC,&lg);
        if (dSC2 == -1){
            perror("erreur au accept");
            exit(-1);
        }
        tabClient[1]=dSC2;

        /*Affectation du numéro au client en fonction de l'ordre de connexion*/
        numClient = 2;
        if (send(dSC2, &numClient, sizeof(int), 0) == -1){
            perror("erreur au send du numClient");
            exit(-1);
        }

        printf("Client 2 connecté\n");

        /*_____________________ Communication _____________________*/
       
        pthread_t thread_client1ToClient2;
        pthread_t thread_client2ToClient1;

        int thread1 = pthread_create(&thread_client1ToClient2, NULL, client1ToClient2, (void *)tabClient[0]);
        if(thread1==-1){
            perror("error thread 1");
            
        }

        int thread2 = pthread_create(&thread_client2ToClient1, NULL, client2ToClient1,  (void *)tabClient[1]);
        if(thread1==-1){
            perror("error thread 1");
            
        }

        /*Attente de la fin des threads*/
        pthread_join(thread_client1ToClient2, NULL);
        pthread_join(thread_client2ToClient1, NULL);

	    close(dSC1); 
        close(dSC2);
    }
	close(dS);
    return 0;
}
