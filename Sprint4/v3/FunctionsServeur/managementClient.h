#ifndef MANAGEMENTCLIENT_H_
#define MANAGEMENTCLIENT_H_

#include "library.h"

/*
 * Fonction pour gérer les indices du tableaux de clients 
 * Retour : un entier, indice du premier emplacement disponible
 *          -1 si tout les emplecements sont occupés. 
 */
int giveNumClient();

/*
 * Fonctions pour trouver un client dans le tableau à partir de son pseudo
 * Paramètre : char * pseudo = le pseudo du client que l'on cherche 
 * Retour : un entier, correspondant à la socket du client trouvé
 *          -1 si le client n'existe pas 
 */
int findClient(char * pseudo);

/* 
 * Fonction qui vérifie si le pseudo saisi n'est pas déjà utilisé 
 * Retour: 1 si le pseudo n'est pas encore utilisé, 0 sinon 
 */
int isAvailablePseudo(char * pseudo);

/*
 * Fonction qui tue les threads pour lesquels les clients se sont déconnectés
 */
void killThread();

#endif