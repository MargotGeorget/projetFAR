#include "dialogFunctions.h"

void sending(int dS, char * msg){
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
    int nbClient;
    if(recv(dS, &nbClient, sizeof(int), 0) == -1){ /*vérification de la valeur de retour*/
        perror("erreur au recv d'un int");
        exit(-1);
    } 
    return nbClient;
}

int createSocketCLient(char * port, char * ip){
    /*Création de la socket*/
	long dS = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in aS;
	aS.sin_family = AF_INET;
	inet_pton(AF_INET, port, &(aS.sin_addr));
	aS.sin_port = htons(atoi(ip));

	/*Demander une connexion*/
	socklen_t lgA = sizeof(struct sockaddr_in);
	int connectR = connect(dS, (struct sockaddr *) &aS, lgA);
	if (connectR == -1){
		perror("erreur au connect");
		exit(-1);
	}

    return dS;
}
