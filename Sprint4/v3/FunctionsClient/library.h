#ifndef LIBRARY_H_
#define LIBRARY_H_

/* ----- FICHIER DE DECLARATION DES VARIABLES GLOBALES ----- */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>

/*   Déclaration des variables :
 * - isEnd = booléen gérant la déconnexion d'un client
 * - port = numéro du port sur lequel on se trouve
 * - ip = adresse IP pour communiquer avec le serveur
 * */
extern int isEnd;
int port;
char * ip;


#endif