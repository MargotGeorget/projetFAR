#ifndef MANAGEMENTCLIENT_H_
#define MANAGEMENTCLIENT_H_

/* ----- FICHIER DE GESTION DES CLIENTS ----- */

#include "library.h"

/*
 * Gestion des indices du tableaux de clients
 * Retour : un entier, indice du premier emplacement disponible dans le tableau des clients
 *          -1 si tout les emplacements sont occupés.
 */
int giveNumClient();

/*
 * Recherche d'un client dans le tableau à partir de son pseudo
 * Paramètre : char * pseudo : pseudo du client que l'on cherche
 * Retour : un entier, correspondant à la socket du client trouvé
 *          -1 si le client n'existe pas 
 */
int findClient(char * pseudo);

/* 
 * Vérification du pseudo saisi
 * Retour: un entier, 1 si le pseudo n'est pas encore utilisé,
 *         0 sinon
 */
int isAvailablePseudo(char * pseudo);

/*
 * Terminaison des threads pour lesquels les clients se sont déconnectés
 */
void killThread();

#endif