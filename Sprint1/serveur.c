#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

/*Compiler gcc -Wall -ansi -o serveur serveur.c*/
/*Lancer avec ./serveur 8000*/

int main(int argc, char *argv[]) {
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

	/*Accepter une premiere connexion*/
	struct sockaddr_in aC;
	socklen_t lg = sizeof(struct sockaddr_in);
	int dSC1 = accept(dS, (struct sockaddr*) &aC,&lg);
	if (dSC1 == -1){
		perror("erreur au accept");
		exit(-1);
	}

    printf("Client 1 connecté\n");

    /*Accepter une deuxieme connexion*/
	int dSC2 = accept(dS, (struct sockaddr*) &aC,&lg);
	if (dSC2 == -1){
		perror("erreur au accept");
		exit(-1);
	}

    printf("Client 2 connecté\n");

	/*Communication*/
    
    while(1){

        /*Reception du message du client1*/
        char * msg = (char *) malloc(sizeof(char)*32);
        int recvR = recv(dSC1, msg, sizeof(msg), 0);
        if (recvR == -1){
            perror("erreur au recv");
            exit(-1);
        }
        printf("Message recu du client1: %s \n", msg);

        /*Envoi du message au client2*/
        int sendR = send(dSC2, msg, strlen(msg), 0);
        if (sendR == -1){
            perror("erreur au send");
            exit(-1);
        }
        printf("Message envoye\n");

        /*Reception de la réponse du client2*/
        char * rep = (char *) malloc(sizeof(char)*32);
        int recvR2 = recv(dSC2, rep, sizeof(rep), 0);
        if (recvR2 == -1){
            perror("erreur au recv");
            exit(-1);
        }
        printf("Reponse recu du client2 : %s \n", rep);
        
        /*Envoi de la reponse au client1*/
        int sendR2 = send(dSC1, rep, strlen(rep), 0);
        if (sendR2 == -1){
            perror("erreur au send");
            exit(-1);
        }
        printf("Reponse envoye\n");
    }

	shutdown(dSC1, 2); 
    shutdown(dSC2, 2);
	shutdown(dS, 2);

}
