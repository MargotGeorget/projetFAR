#include "./FunctionsServeur/library.h"
#include "./FunctionsServeur/dialogFunctions.h"
#include "./FunctionsServeur/threadFunctions.h"
#include "./FunctionsServeur/managementClient.h"
#include "./FunctionsServeur/managementMessage.h"
#include "./FunctionsServeur/managementRoom.h"

/*Compiler gcc -pthread -Wall -ansi -o serveur serveur.c*/
/*Lancer avec ./serveur votre_port */

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/*
 * _____________________ MAIN _____________________
 */
int main(int argc, char *argv[]) {
    
    sem_init(&semNbClient, 0, MAX_CLIENT);

    /*Verification des paramètres*/
    if(argc<2){
        printf("Erreur : Lancez avec ./serveur <votre_port> ");
    }

    arg1 = argv[1];

    /*Initialisation du tableau qui informe des threads à kill à false pour chaque indice*/
    int i;
    for(i=0;i<MAX_CLIENT;i++){
        tabThreadToKill[i]=0;
    }
    printf("init room\n");
    initRoom();
    printf("init room fin\n");

    /*Création, nommage et passage en mode écoute de la socket pour les envois de fichier*/
    dSFile = createSocketServeur(atoi(arg1)+1);
	
	/*Création, nommage et passage en mode écoute de la socket pour la communication entre client*/
	int dS = createSocketServeur(atoi(arg1));

    while(1){
        int dSC;

        /*Tant qu'on peut accepter des clients */
        sem_wait(&semNbClient); 

        /*On tue les threads pour lesquels les clients ont quitté la connexion*/
        killThread();   

        /*Accepter une connexion*/
        dSC = acceptConnection(dS);

    /*On gère l'arrivée du client avant de créer son thread*/

        /*Récupération du nombre de client connectés*/
        int valueSem;
        sem_getvalue(&semNbClient, &valueSem);
        int nbClient = MAX_CLIENT-valueSem;

        /*Affectation du numéro au client en fonction des emplacements dans le tableau de Clients*/
        long numClient = giveNumClient();

        /*Envoi du nombre de client présent au nouveau client*/
        sendingInt(dSC, nbClient);
                
        printf("Client %ld connecté\n", numClient);

        /*Réception du pseudo*/

        char * pseudo = (char *) malloc(sizeof(char)*100);
        int availablePseudo = 0; /*false*/

        do {
            sendingInt(dSC,availablePseudo);
            receiving(dSC, pseudo, sizeof(char)*12);
            pseudo = strtok(pseudo, "\n");
            availablePseudo = isAvailablePseudo(pseudo);

        } while(!availablePseudo);

        /*Pseudo valide*/
        sendingInt(dSC,availablePseudo);

        /*On enregistre le pseudo du client*/
        
        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        tabClient[numClient].pseudo = (char *) malloc(sizeof(char)*12);
        strcpy(tabClient[numClient].pseudo,pseudo);

        /*On enregistre la socket du client*/
        tabClient[numClient].occupied = 1;
        tabClient[numClient].dSC = dSC;

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

        /*On envoi un message pour avertir les autres clients de l'arrivée du nouveau client*/
        strcpy(pseudo,"** a rejoint la communication **\n");
        sendingAll(numClient, pseudo);    

        printf("%s\n",pseudo);

        free(pseudo);

        presentationRoom(dSC);

        /*_____________________ Communication _____________________*/
        int threadReturn = pthread_create(&tabThread[numClient],NULL,broadcast,(void *)numClient);
        if(threadReturn == -1){
            perror("erreur thread create");
        }
    
        printf("Clients connectés : %d\n", nbClient);
        
    }
    sem_destroy(&semNbClient);
	close(dS);
    return 0;
}
