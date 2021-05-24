#ifndef THREADFUNCTIONS_H_
#define THREADFUNCTIONS_H_

/* ----- FICHIER DES FONCTIONS RELATIVES AUX THREADS SERVEUR ----- */

#include "library.h"
#include "managementMessage.h"
#include "dialogFunctions.h"
#include "managementRoom.h"
#include "managementClient.h"

/**
 * @brief Préparation et réception d'un fichier envoyé par le client
 * @param fileNameParam nom du fichier reçu
 * */
void * uploadFile_th(void * fileNameParam);

/**
 * @brief Préparation et envoi d'un fichier au client
 * @param fp : descripteur du fichier à envoyer
 * */
void * downloadFile_th(void * fp);

/**
 * @brief Start routine de pthread_create()
 * Diffusion du message reçu à tous les autres clients connectés
 * */
void * broadcast(void * clientParam);

#endif