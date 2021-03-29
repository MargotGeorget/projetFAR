#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

/*Compiler gcc -Wall -ansi -o client client.c*/
/*Lancer avec ./client 162.38.111.181 8000*/

int main(int argc, char *argv[]) {

	/*Création de la socket*/
	int dS = socket(PF_INET, SOCK_STREAM, 0);
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

	/*Communication*/
	char * m = "Bonjour !";
	int sendR = send(dS, m, strlen(m)+1, 0);
	if (sendR == -1){
		perror("erreur au send");
		exit(-1);
	}

	int r;
	int recvR = recv(dS, &r, sizeof(int), 0);
	if (recvR == -1){
		perror("erreur au recv");
		exit(-1);
	}
	printf("reponse : %d \n", r);
	shutdown(dS,2);

}
