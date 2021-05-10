#ifndef MANAGEMENTMESSAGE_H_
#define MANAGEMENTMESSAGE_H_

/* ----- FICHIER DE GESTION DES MESSAGES ----- */

#include "library.h"
#include "dialogFunctions.h"

/*
 * Mise en forme du message à envoyer avec le pseudo de l'expéditeur
 * Paramètres : char * : message à envoyer
 *              char * : pseudo à ajouter
 */
void addPseudoToMsg(char * msg, char * pseudoSender);

void displayMan(int numClient);

/*
 * Vérifie si un client souhaite quitter la communication
 * Paramètres : char * msg : message du client à vérifier
 * Retour : 1 (vrai) si le client veut quitter, 0 (faux) sinon
*/
int endOfCommunication(char * msg);

int numCommande(char * msg);

/*
 * Vérifie si un client souhaite envoyer un fichier
 * Paramètres : char * msg : message du client à vérifier
 * Retour : 1 (vrai) si le client veut envoyer un fichier, 0 (faux) sinon
*/
int isReceivingFile(char * msg);

/*
 * Vérifie si un client souhaite télécharger un fichier
 * Paramètres : char * msg : message du client à vérifier
 * Retour : 1 (vrai) si le client veut recevoir un fichier, 0 (faux) sinon
*/
int isSendingFile(char * msg);

int isPresentationRoom(char * msg);

int isJoinRoom(char * msg);

int isCreateRoom(char * msg);

int isRemoveRoom(char * msg);

int isUpdateNameRoom(char * msg);

int isUpdateDescrRoom(char * msg);

#endif