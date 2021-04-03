#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

/*Compiler gcc -Wall -ansi -o serveur serveur.c*/
/*Lancer avec ./serveur votre_port */

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
        return 0;
    }
    return 1;
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

        /*Affectation du numéro au client en fonction de l'ordre de connexion*/
        numClient = 2;
        if (send(dSC2, &numClient, sizeof(int), 0) == -1){
            perror("erreur au send du numClient");
            exit(-1);
        }

        printf("Client 2 connecté\n");

        /*_____________________ Communication _____________________*/
        
        int communication = 1;

        while(communication){

            /*Reception du message du client1*/
            char * msg = (char *) malloc(sizeof(char)*100);
            receiving(dSC1, msg, sizeof(char)*100);
            printf("\nMessage recu du client1: %s \n", msg);

            /*On verifie si le client 1 veut terminer la communication*/
            communication = endOfCommunication(&msg);

            /*Envoi du message au client2*/
            sending(dSC2, msg);
            printf(" -- Message envoye\n");

            if(communication){
                /*Reception de la réponse du client2*/
                char * rep = (char *) malloc(sizeof(char)*100);
                receiving(dSC2, rep, sizeof(char)*100);
                printf("\nReponse recu du client2 : %s \n", rep);
                
                /*On verifie si le client 2 veut terminer la communication*/
                communication = endOfCommunication(&rep);

                /*Envoi de la reponse au client1*/
                sending(dSC1, rep);
                printf(" -- Reponse envoye\n");
            }
        }

	    close(dSC1); 
        close(dSC2);
    }
	close(dS);

}
