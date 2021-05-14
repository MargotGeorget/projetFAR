#include "./FunctionsClient/library.h"
#include "./FunctionsClient/threadFunctions.h"
#include "./FunctionsClient/managementMessage.h"
#include "./FunctionsClient/dialogFunctions.h"

/*Compiler gcc -pthread -Wall -ansi -o client client.c*/
/*Lancer avec ./client votre_ip votre_port*/
int isEnd = 0;

/*
 * _____________________ MAIN _____________________
 * */
int main(int argc, char *argv[]) {

    /*Verification des paramètres*/
    if(argc<3){
        printf("Erreur : Lancez avec ./client <votre_ip> <votre_port> ");
    }

    ip = argv[1]; 
    port = atoi(argv[2]); 

	/*Création de la socket*/
	long dS = createSocketClient(port, ip);

    /*Reception du nombre de client*/
    int nbClient;
    nbClient = receivingInt(dS);
        
    /*Saisie du pseudo du client au clavier*/
    int availablePseudo;
    char * myPseudo = (char *) malloc(sizeof(char)*12);
    availablePseudo = receivingInt(dS); /*Reçois faux en premier*/
   
    printf("Votre pseudo (maximum 12 caractères): ");
    fgets(myPseudo, 12, stdin);
    sending(dS,myPseudo);
    availablePseudo = receivingInt(dS); 
   
    while(!availablePseudo){
        printf("Pseudo déjà utilisé!\nVotre pseudo (maximum 12 caractères): ");
        fgets(myPseudo, 12, stdin);
        sending(dS,myPseudo);
        availablePseudo = receivingInt(dS);
    }

    free(myPseudo);

    /*_____________________ Communication _____________________*/

    /*Echange des messages*/

    /*Création d'un thread d'envoi*/
    pthread_t thread_sending;
    pthread_t thread_receiving;

    int thread1 = pthread_create(&thread_sending, NULL, sending_th, (void *)dS);
    if(thread1==-1){
        perror("error thread 1");
    }

    /*Création d'un thread de reception*/
    int thread2 = pthread_create(&thread_receiving, NULL, receiving_th, (void *)dS);
    if(thread2==-1){
        perror("error thread 2");
    }

    /*Attente de la fin des threads*/
    pthread_join(thread_sending, NULL);
    pthread_join(thread_receiving, NULL);
    
    /*Fin de la communication**/
    return 0;
}
