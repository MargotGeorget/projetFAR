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

	/*Accepter une connexion*/
	struct sockaddr_in aC;
	socklen_t lg = sizeof(struct sockaddr_in);
	int dSC = accept(dS, (struct sockaddr*) &aC,&lg);
	if (dSC == -1){
		perror("erreur au accept");
		exit(-1);
	}

	/*Communication*/
	char msg [20];
	int recvR = recv(dSC, msg, sizeof(msg), 0);
	if (recvR == -1){
		perror("erreur au recv");
		exit(-1);
	}

	printf("recu : %s \n", msg);
	int r = 10;
	int sendR = send(dSC, &r, sizeof(int), 0);
	if (sendR == -1){
		perror("erreur au send");
		exit(-1);
	}
	shutdown(dSC, 2); 
	shutdown(dS, 2);

}
