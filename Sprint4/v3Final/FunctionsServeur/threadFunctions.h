#ifndef THREADFUNCTIONS_H_
#define THREADFUNCTIONS_H_

/* ----- FICHIER DES FONCTIONS RELATIVES AUX THREADS SERVEUR ----- */

#include "library.h"
#include "managementMessage.h"
#include "dialogFunctions.h"

/*
 * Préparation et réception d'un fichier envoyé par le client
 * Paramètres : void * fileNameParam : nom du fichier reçu
 * */
void * receivingFile_th(void * fileNameParam);

/*
 * Préparation et envoi d'un fichier au client
 * Paramètres : void * fp : descripteur du fichier à envoyer
 * */
void * sendingFile_th(void * fp);

/*
 * Start routine de pthread_create()
 * Diffusion du message reçu à tous les autres clients connectés
 * */
void * broadcast(void * clientParam);

#endif