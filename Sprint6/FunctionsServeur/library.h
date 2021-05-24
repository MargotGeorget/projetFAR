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
#define MAX_CLIENT 15 /*Attention si vous diminuez le nombre de client max des clients risquent de ne plus pouvoir se connecter*/
#define NB_ROOMS 4 /*3 salons + 1 salon général*/
#define SIZE_MSG sizeof(char)*500 

/*
 * Définition d'une structure Client pour regrouper toutes les informations du client
 * */
typedef struct Client Client;
struct Client{
    int connected; /*Permet de savoir si le client est connecté ou non*/
    int id;
    char pseudo[15];
    char password[12];
    char descr[300];
    long dSC; /*socket du client*/
    int idRoom; /*id du salon dans lequel se trouve le client*/
    int created; /*Permet de savoir si un client occupe la place dans le tableau de client*/
    int isAdmin; /*Booleen pour savoir si le client est un admin du serveur*/
};

typedef struct Room Room;
struct Room{
    int id;
    char * name;
    char * descr;
    int members[MAX_CLIENT]; /*Booleen (0 ou 1) à l'indice i pour savoir si le client numéro i est présent dans le salon*/ 
    int admin[MAX_CLIENT]; /*Booleen (0 ou 1) à l'indice i pour savoir si le client numéro i est admin du salon*/ 
    int ban[MAX_CLIENT]; /*Booleen (0 ou 1) à l'indice i pour savoir si le client numéro i est ban du salon*/ 
    int created; /*Permet de savoir s'il faut afficher le salon au client, s'il est créé ou non*/
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
int dS;
char * arg1;

#endif