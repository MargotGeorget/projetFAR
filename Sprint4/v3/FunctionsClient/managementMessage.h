#ifndef MANAGEMENTMESSAGE_H_
#define MANAGEMENTMESSAGE_H_

#include "library.h"

/*
 * Vérifie si un client souhaite quitter la communication
 * Paramètres : char ** msg : message du client à vérifier
 * Retour : 1 (vrai) si le client veut quitter, 0 (faux) sinon
*/
int endOfCommunication(char * msg);

/*
 * Vérifie si un client souhaite envoyer un fichier
 * Paramètres : char ** msg : message du client à vérifier
 * Retour : 1 (vrai) si le client veut envoyer un fichier, 0 (faux) sinon
*/
int isSendingFile(char * msg);

#endif