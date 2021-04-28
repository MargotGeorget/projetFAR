#ifndef THREADFUNCTIONS_H_
#define THREADFUNCTIONS_H_

/* ----- FICHIER DES FONCTIONS RELATIVES AUX THREADS CLIENT ----- */

#include "library.h"
#include "managementMessage.h"
#include "dialogFunctions.h"

/* Envoi du fichier dans le thread associé
 * Paramètres : void * fileNameParam : nom du fichier à envoyer
 * */
void * sendingFile_th(void * fileNameParam);

/* Réception du fichier dans le thread associé
 * Paramètres : void * fileNameParam : nom du fichier à receptionner
 * */
void * receivingFile_th(void * fileNameParam);

/* Gestion du thread d'envoi
 * Paramètres : void * dSparam : numéro de la socket sur laquelle envoyer
 * */
void * sending_th(void * dSparam);

/* Gestion du thread de réception
 * Paramètres : void * dSparam : numéro de la socket sur laquelle recevoir
 * */
void * receiving_th(void * dSparam);

#endif