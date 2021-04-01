#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

/*Compiler gcc -Wall -ansi -o client client.c*/
/*Lancer avec ./client 162.38.111.181 8000*/

/*
Envoi un message à une socket et teste que tout se passe bien
Paramètres : int dS : la socket
             char * msg : message à envoyer   
Retour : pas de retour
*/
void sending(int dS, char * msg){
    int sendR = send(dS, msg, strlen(msg)+1, 0);
    if (sendR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au send");
        exit(-1);
    }
}

/*
Receptionne un message d'une socket et teste que tout se passe bien
Paramètres : int dS : la socket
             char * msg : message à recevoir
             ssize_t size : taille maximum du message à recevoir   
Retour : pas de retour
*/
void receiving(int dS, char * rep, ssize_t size){
    int recvR = recv(dS, rep, size, 0);
    if (recvR == -1){ /*vérification de la valeur de retour*/
        perror("erreur au recv");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {

    /*Verification des paramètres*/
    if(argc<3){
        printf("Erreur : Lancez avec ./client <votre_ip> <votre_port> ");
    }

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

    /*En attente du client 2*/
    if(numClient==1){
        printf("En attente d'un autre client\n");
    }

    /*Reception du pseudo du client avec lequel on communique*/
    char * hisPseudo = (char *) malloc(sizeof(char)*12);
    receiving(dS,hisPseudo,sizeof(char)*12);
    hisPseudo = strtok(hisPseudo, "\n");
    printf("\n--------- Vous communiquez avec %s ---------\n", hisPseudo);
    printf("\nVos messages peuvent faire jusqu'à 100 caractères.\nPour quitter la conversation envoyez 'fin'.\nBonne communication !\n\n");


    /*_____________________ Communication _____________________*/

    /*Envoi du premier message par le client1*/

    if(numClient==1){
        
        /*Saisie du message au clavier*/
        char * m = (char *) malloc(sizeof(char)*100);
        printf("Vous : ");
        fgets(m, 100, stdin); 

        /*Envoi du message*/ 
        sending(dS, m);       

        free(m);
    }

    /*Echange des messages*/

    int communication = 1;

    while(communication){

        /*Reception du message et affichage*/
        char * r = (char *) malloc(sizeof(char)*100);
        receiving(dS, r, sizeof(char)*100);
        printf("%s : %s", hisPseudo, r);

        /*On verifie si l'autre client veut mettre fin à la connexion*/
        if (strcmp(r, "** A quitté la communication **\n")==0){
            communication = 1;
            free(r);
            break;
        }
        
        free(r);
        
        /*Saisie du message au clavier*/
	    char * m = (char *) malloc(sizeof(char)*100);
        printf("Vous : ");
        fgets(m, 100, stdin);

        /*Envoi du message*/
        sending(dS, m);	    

        /*On verifie si le client veut fermer sa connexion*/
        if (strcmp(m, "fin\n")==0){
            communication = 1;
            free(m);
            break;
        }

        free(m);
    }

	close(dS);

}
