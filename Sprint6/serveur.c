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
    nbThreadToKill = 0;

    initRoom();
    initClients();

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

        /*Envoi du nombre de client présent au nouveau client*/
        sendingInt(dSC, nbClient);

        /*Réception du pseudo*/

        char * pseudo = (char *) malloc(sizeof(char)*100);
        int availablePseudo = 0;

        do {
            receiving(dSC, pseudo, sizeof(char)*12);
            pseudo = strtok(pseudo, "\n");
            availablePseudo = isAvailableName(pseudo);
            sendingInt(dSC,availablePseudo);

        } while(!availablePseudo);

        int errorMaxClient = -1;
        int errorClientConnected = -2;
        int signUp = 0;
        int signIn = 1; 
        printf("pseudo: %s\n",pseudo);
        long numClient = findClient(pseudo); 
        printf("%ld\n",numClient);
        if(numClient==-1){ /*Le client n'a pas de compte*/
            /*Affectation du numéro au client en fonction des emplacements dans le tableau de Clients*/
            numClient = giveNumClient();
            printf("numéro client : %ld\n",numClient);
            if(numClient==-1){
                sendingInt(dSC,errorMaxClient);
            }else { /*Le client peut s'inscrire*/
                sendingInt(dSC,signUp);
                createAccount(dSC, pseudo, numClient);
                printf("Compte client créé : Numéro - %ld, pseudo - %s\n", numClient,pseudo);
            }
        }else{/*Le client à un compte, il se connecte*/
            if(tabClient[numClient].connected){
                sendingInt(dSC,errorClientConnected);
            }else {
                sendingInt(dSC,signIn);
                connection(dSC, numClient);
                printf("Client %ld connecté\n", numClient);
            }
        }
            free(pseudo);
        
        if(numClient!=-1){

            /*_____________________ Communication _____________________*/
            int threadReturn = pthread_create(&tabThread[numClient],NULL,broadcast,(void *)numClient);
            if(threadReturn == -1){
                perror("erreur thread create");
            }
        
            printf("Clients connectés : %d\n", nbClient);
            
        }else{
            sem_post(&semNbClient);
        }
    }
    sem_destroy(&semNbClient);
	close(dS);
    return 0;
}
