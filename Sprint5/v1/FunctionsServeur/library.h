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
#include <stdarg.h>

/*
 * MAX_CLIENT = nombre maximum de client accepté sur le serveur
 * NB_ROOMS = nombre maximum de salon de discussion 
 * */
#define MAX_CLIENT 5
#define NB_ROOMS 4 /*3 salons + 1 salon général*/

/*
 * Définition d'une structure Client pour regrouper toutes les informations du client
 * */
typedef struct Client Client;
struct Client{
    int occupied;
    char * pseudo;
    long dSC;
    int idRoom;
};

typedef struct Room Room;
struct Room{
    int id;
    char * name;
    char * descr;
    int members[MAX_CLIENT];
    int nbClient;
    int created;
};

/*   Déclaration des variables :
 * - tabClient = tableau répertoriant les clients connectés
 * - tabThread = tableau des threads associés au traitement de chaque client
 * - semNbClient = sémaphore qui gère le nombre de clients actuellement connectés
 * - tabThreadToKill = tableau recensant les threads à tuer (thread abandonnés lors de la déconnexion d'un client)
 * - lock = variable mutex permettant la gestion des sections critiques
 * - dSFile = socket recevant les connexions lors d'une demande d'envoi/de téléchargement de fichier
 * - arg1 = récupère le premier argument de la ligne de commande
 * */

Client tabClient[MAX_CLIENT];
Room rooms[NB_ROOMS];
pthread_t tabThread[MAX_CLIENT];
sem_t semNbClient;
pthread_t tabThreadToKill[MAX_CLIENT*2];
int nbThreadToKill;

extern pthread_mutex_t lock;

long dSFile;
char * arg1;

#endif