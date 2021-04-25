#ifndef MANAGEMENTMESSAGE_H_
#define MANAGEMENTMESSAGE_H_

#include "library.h"

/*
 * Fonction pour mettre en forme le message à envoyer avec le pseudo de l'expéditeur 
 * Paramètres : char * : le message à envoyer 
 *              char * : le pseudo à ajouter 
 * Pas de retour
 */
void addPseudoToMsg(char * msg, char * pseudoSender);

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
int isReceivingFile(char * msg);

/*
 * Vérifie si un client souhaite télécharger un fichier
 * Paramètres : char ** msg : message du client à vérifier
 * Retour : 1 (vrai) si le client veut recevoir un fichier, 0 (faux) sinon
*/
int isSendingFile(char * msg);

#endif