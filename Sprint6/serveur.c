#include "./FunctionsServeur/library.h"
#include "./FunctionsServeur/dialogFunctions.h"
#include "./FunctionsServeur/threadFunctions.h"
#include "./FunctionsServeur/managementClient.h"
#include "./FunctionsServeur/managementMessage.h"
#include "./FunctionsServeur/managementRoom.h"

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

    /*Récupération des paramètres*/
    arg1 = argv[1];

    /*Initialisation du tableau qui informe des threads à kill à false pour chaque indice*/
    int i;
    for(i=0;i<MAX_CLIENT;i++){
        tabThreadToKill[i]=0;
    }
    nbThreadToKill = 0;

    /*Initialisation des salons et des clients*/
    initRoom();
    initClients();

    /*Création, nommage et passage en mode écoute de la socket pour les envois de fichier*/
    dSFile = createSocketServeur(atoi(arg1)+1);
	
	/*Création, nommage et passage en mode écoute de la socket pour la communication entre client*/
	dS = createSocketServeur(atoi(arg1));

    /*Mise en place de l'écoute du ctrl C*/
    signal(SIGINT, Ctrl_C_Handler);

    while(1){
        int dSC;

        /*Tant qu'on peut accepter des clients */
        sem_wait(&semNbClient); 

        /*On tue les threads pour lesquels les clients ont quitté la connexion*/
        killThread();

        /*Accepter une connexion*/
        dSC = acceptConnection(dS);

    /*On gère l'arrivée du client avant de créer son thread*/
        printf("\nArrivée d'un client, connexion en cours...\n");

        /*Récupération du nombre de client connectés*/
        int valueSem;
        sem_getvalue(&semNbClient, &valueSem);
        int nbClient = MAX_CLIENT-valueSem;

        /*Connection*/

        char * pseudo = (char *) malloc(sizeof(char)*100);
        int connectionEnd = 0;
        long numClient;

        /*Tant que la connexion du client n'a pas été validée on redemande un pseudo pour se connecter*/
        while(!connectionEnd){
            int availablePseudo; /*Vérification de la validité du pseudo (pas d'espace)*/
            sending(dSC,"\n-- Votre pseudo (maximum 12 caractères): \n");
            receiving(dSC, pseudo, sizeof(char)*12);
            availablePseudo = isAvailableName(pseudo);

            /*Nouvelle saisie du pseudo si pas valide */ 
            while(!availablePseudo){
                printf("Erreur dans la sasie du pseudo\n");
                sending(dSC,"\nUn pseudo ne peut pas contenir d'espace!\nVotre pseudo (maximum 12 caractères): \n");
                receiving(dSC, pseudo, sizeof(char)*12);
                availablePseudo = isAvailableName(pseudo);
            } 

            printf("-- Pseudo: %s\n",pseudo);

            /*On regarde si un client est inscrit avec ce pseudo*/
            numClient = findClient(pseudo); 

            if(numClient==-1){ /*Le client n'a pas de compte*/

                /*Affectation du numéro au client en fonction des emplacements dans le tableau de Clients*/
                numClient = giveNumClient();

                if(numClient==-1){ /*Aucun emplacement disponibles*/ 
                    sending(dSC,"\nImpossible de vous incrire, nombre de compte maximum atteint\n");
                    printf("\nAucun compte client trouvé, nombre de client maximum atteint\n");
                    sending(dSC,"/end"); 
                    connectionEnd = 1;

                }else { /*Emplacement disponible : Le client peut s'inscrire*/
                
                    printf("\nAucun compte client trouvé, inscription...\n");
                    connectionEnd = createAccount(dSC, pseudo, numClient);
                    printf("Compte client créé : Numéro - %ld, pseudo - %s\n", numClient,pseudo);
                }

            }else{ /*Le client à un compte*/
                if(tabClient[numClient].connected){ /*Le compte est déjà connecté sur un autre appareil*/
                    sending(dSC,"Ce compte est déjà connecté au serveur sur un autre appareil\n");
                    printf("\nCompte client déjà connecté sur une autre machine.\n");
                    sending(dSC,"/end"); 
                    numClient = -1;
                    connectionEnd = 1;

                }else { /*Le client peut se connecter sur ce compte*/
                    printf("\nCompte client trouvé, connexion...\n");
                    connectionEnd = connection(dSC, numClient);
                    printf("\nClient %ld connecté\n", numClient);
                }
            }
        }
        free(pseudo);
        
        if(numClient!=-1){ /*Le client est connecté*/

            /*_____________________ Communication _____________________*/
            int threadReturn = pthread_create(&tabThread[numClient],NULL,broadcast,(void *)numClient);
            if(threadReturn == -1){
                perror("erreur thread create");
            }
        
            printf("\nNombre de clients connectés : %d.\n", nbClient);
            
        }else{ /*Le client n'a pas pu se connecter*/

            close(dSC);
            sem_post(&semNbClient);
        }
    }
    
    sem_destroy(&semNbClient);
	close(dS);
    return 0;
}
