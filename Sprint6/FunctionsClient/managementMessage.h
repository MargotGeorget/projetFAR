#ifndef MANAGEMENTMESSAGE_H_
#define MANAGEMENTMESSAGE_H_

#include "library.h"

/**
 * @brief Vérifie si un client souhaite quitter la communication
 * @param msg message du client à vérifier
 * @return 1 (vrai) si le client veut quitter, 0 (faux) sinon
*/
int endOfCommunication(char * msg);

/**
 * @brief Vérifie si un client souhaite envoyer un fichier (envoie /upload)
 * @param msg message du client à vérifier
 * @return 1 (vrai) si le client veut envoyer un fichier, 0 (faux) sinon
*/
int isUploadFile(char * msg);

/**
 * @brief Vérifie si un client souhaite recevoir un fichier (envoie /download)
 * @param msg message du client à vérifier
 * @return 1 (vrai) si le client veut recevoir un fichier, 0 (faux) sinon
*/
int isDownloadFile(char * msg);

/**
 * @brief Vérifie si le serveur à été fermé
 * @param msg message reçu à vérifier
 * @return 1 (vrai) si le serveur est fermé, 0 (faux) sinon
*/
int isServeurShutdown(char * msg);

/**
 * @brief Vérifie si le client veut voir le manuel
 * @param msg message reçu à vérifier
 * @return 1 (vrai) si la commande est /man, 0 (faux) sinon
*/
int isMan(char * msg);

/**
 * @brief affiche le man.txt au client
 */
void displayMan();

#endif