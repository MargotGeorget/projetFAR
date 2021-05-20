#include "threadFunctions.h"

void * uploadFile_th(void * fileNameParam){

    long dSCFile;
    /*Accepter une connexion*/
    dSCFile = acceptConnection(dSFile);

    char * fileName = (char *)fileNameParam;

    /*Création d'un fichier au même nom et reception du contenu du fichier*/

    /*Création du chemin pour enregister le fichier*/ 
    char * pathToFile = (char *) malloc(sizeof(char)*130);
    strcpy(pathToFile,"FileServeur/");
    strcat(pathToFile,fileName);

    printf("Je reçois le fichier %s du client le socket %ld\n",fileName,dSFile);

    /*Création du fichier et du buffer pour recevoir les données*/
    char buffer[1024];
    int fp = open(pathToFile, O_WRONLY |  O_CREAT, 0666);
    if(fp == -1){
        printf("erreur au open");
        exit(1);
    }
    int nbBytes = receivingInt(dSCFile);

    /*Reception*/
    while(nbBytes>0){
        recv(dSCFile, buffer, 1024, 0);
        write(fp, buffer,nbBytes);
        nbBytes = receivingInt(dSCFile);
        bzero(buffer, 1024);
    }
    printf("\n**Fichier reçu**\n");

    close(fp);
    close(dSCFile);

    free(fileName);
    free(pathToFile);

    return NULL;

}

void * downloadFile_th(void * fpParam){

    /*Accepter une connexion*/
	long dSCFile = acceptConnection(dSFile);

    FILE * fp = (FILE *)fpParam;

    printf("J'envoie le fichier au client avec le socket %ld\n",dSCFile);
    
    /*Appel de la fonction envoyant le fichier*/
    sendFile(dSCFile,fp);

    fclose(fp);
    close(dSCFile);
    return NULL;
}

void * broadcast(void * clientParam){
    
    int isEnd = 0;
    int numClient = (long) clientParam;
    /*Récupéraion de la socket du client*/
    pthread_mutex_lock(&lock);
    int dSC = tabClient[numClient].dSC;
    pthread_mutex_unlock(&lock);

    while(!isEnd){
        /*Réception du message*/
        char * msgReceived = (char *) malloc(sizeof(char)*100);
        receiving(dSC, msgReceived, sizeof(char)*100);
        printf("\nMessage recu: %s \n", msgReceived);

        char * msgInfo = (char *) malloc(sizeof(char)*100);;
        char first = msgReceived[0];

        if(strcmp(&first,"@")!=0 && strcmp(&first,"/")!=0){ /*On regarde si une commande est présente dans le message*/
            /*Envoi du message aux autres clients*/
            printf("Envoi du message aux autres clients. \n");
            sendingRoom(numClient, msgReceived);
        }else { /*Analyse de la commande*/

            int numCmd = numCommande(msgReceived);
            printf("Numéro de la commande: %d\n",numCmd);

            switch(numCmd){
            case 0:        
                strcpy(msgInfo,"Aucune commande reconnue\n"),
                sending(dSC,msgInfo);
                break;
            case 1: /* --/man-- Afficher la listes des commandes*/
                /*displayMan(numClient); ToDo : corriger les erreurs*/
                break;
            case 2: /* --/whoishere-- Afficher la liste des clients connectés*/
                displayClient(numClient);
                break;
            case 3:
                updatePseudo(numClient,msgReceived);
                break;
            case 4: /* --/rooms-- Présentation des salons (Nom, Description et clients membres)*/
                presentationRoom(dSC);
                break;
            case 5: /* --/join nameRoom-- Changer de salon*/
                joinRoom(numClient, msgReceived);
                break;
            case 6: /* --/create nameRoom-- Créer un nouveau salon*/
                createRoom(numClient, msgReceived);
                break;
            case 7: /* --/remove nameRoom-- Supprimer un salon*/
                removeRoom(numClient,msgReceived);
                break;
            case 8: /* --/name nameRoom newNameRoom-- Changer le nom d'un salon*/
                updateNameRoom(numClient,msgReceived);
                break;
            case 9: /* --/descrRoom nameRoom newDescrRoom-- Changer la description d'un salon*/
                updateDescrRoom(numClient,msgReceived);
                break;
            case 10: /* --/upload-- Télécharger un fichier vers le serveur*/
                uploadFile(dSC);
                break;
            case 11: /* --/download-- Envoyer un fichier vers le client*/
                downloadFile(dSC,msgReceived);
                break;
            case 12: /* --/end-- Quitter le serveur*/
                isEnd = 1;
                sending(tabClient[numClient].dSC,msgReceived);
                break;
            case 13: /* --@all-- Envoyer le message à tous les clients*/
                sendingAll(numClient,msgReceived);
                break;
            case 14: /*--@pseudo-- Envoyer le message en privée*/
                sendingPrivate(numClient,msgReceived);
                break;
            case 15: /* --/descr newDescr-- Changer sa description de profil*/
                updateDescr(numClient,msgReceived);
                break;
            case 16: /* --/profil pseudo-- Afficher le profil d'un client (pseudo, rôle et description)*/
                displayOneClient(numClient,msgReceived);
                break;
            case 17: /* --/deleteAccount-- Supprimer son compte (déconnecte la personne automatiquement)*/
                isEnd = deleteAccount(numClient); /*Si le compte peut être supprimé, isEnd passe à true, le client est déconnecté*/
                break;
            case 18: /*--/admin-- Afficher les clients admin*/
                displayAdmin(numClient);
                break;
            case 19: /* --/move pseudo nameRoom-- Déplacer le client demandé dans la room donnée*/
                moveClient(numClient,msgReceived);
                break;
            case 20: /* --/ban pseudo-- Renvoie le client dans le salon général et l'empêche de revenir dans le salon dans lequel il était*/
                banClient(numClient,msgReceived);
                break;
            case 21: /*--/kick pseudo-- Renvoie le client dans le salon général*/
                kickClient(numClient,msgReceived);
                break;
            case 22: /*--/unban pseudo nameRoom-- Redonne les autorisations au client pour rejoindre le salon*/
                unbanClient(numClient,msgReceived);
                break;
            case 23: /*--/rightRoom pseudo nameRoom-- Donne le rôle administrateur de la room au client */
                giveRightRoom(numClient,msgReceived);
                break;
            case 24: /*--/rightServer pseudo nameRoom-- Donne le rôle administrateur du serveur au client*/
                giveRightServer(numClient,msgReceived);
                break;
            }
        }
        free(msgReceived);
        free(msgInfo);
    }
    /*Fermeture du socket client*/
    deleteMember(numClient,tabClient[numClient].idRoom);
    pthread_mutex_lock(&lock);
    tabClient[numClient].connected=0;
    tabThreadToKill[nbThreadToKill]=tabThread[numClient];
    nbThreadToKill+=1;
    tabThread[numClient] = ((void *)0);
    close(tabClient[numClient].dSC);
    pthread_mutex_unlock(&lock);
    sem_post(&semNbClient);
    return NULL;
}