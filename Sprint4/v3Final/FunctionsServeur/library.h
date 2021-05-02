#ifndef LIBRARY_H_
#define LIBRARY_H_

/* ----- FICHIER DE DECLARATION DES VARIABLES GLOBALES ET D'IMPORTATION DES LIBRARIES ----- */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

/*
 * Définition d'une structure Client pour regrouper toutes les informations du client
 * */
typedef struct Client Client;
struct Client{
    int occupied;
    char * pseudo;
    long dSC;
};

/*   Déclaration des variables :
 * - MAX_CLIENT = nombre maximum de client accepté sur le serveur
 * - tabClient = tableau répertoriant les clients connectés
 * - tabThread = tableau des threads associés au traitement de chaque client
 * - semNbClient = sémaphore qui gère le nombre de clients actuellement connectés
 * - tabThreadToKill = tableau recensant les threads à tuer (thread abandonnés lors de la déconnexion d'un client)
 * - lock = variable mutex permettant la gestion des sections critiques
 * - dSFile = socket recevant les connexions lors d'une demande d'envoi/de téléchargement de fichier
 * - arg1 = récupère le premier argument de la ligne de commande
 * */
#define MAX_CLIENT 5
Client tabClient[MAX_CLIENT];
pthread_t tabThread[MAX_CLIENT];
sem_t semNbClient;
int tabThreadToKill[MAX_CLIENT];

extern pthread_mutex_t lock;

long dSFile;
char * arg1;

#endif