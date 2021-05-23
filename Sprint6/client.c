#include "./FunctionsClient/library.h"
#include "./FunctionsClient/threadFunctions.h"
#include "./FunctionsClient/managementMessage.h"
#include "./FunctionsClient/dialogFunctions.h"

/*Compiler gcc -pthsread -Wall -ansi -o client client.c*/
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
	dS = createSocketClient(port, ip);

    signal(SIGINT, Ctrl_C_Handler);

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
    pthread_join(thread_receiving, NULL);

    pthread_cancel(thread_sending);
    
    /*Fin de la communication**/
    return 0;
}
