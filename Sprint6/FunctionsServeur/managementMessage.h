#ifndef MANAGEMENTMESSAGE_H_
#define MANAGEMENTMESSAGE_H_

/* ----- FICHIER DE GESTION DES MESSAGES ----- */

#include "library.h"
#include "dialogFunctions.h"

/**
 * @brief Mise en forme du message à envoyer avec le pseudo de l'expéditeur
 * @param msg message à envoyer
 * @param pseudoSender pseudo à ajouter
 */
void addPseudoToMsg(char * msg, char * pseudoSender);

/**
 * @brief Envoi du fichier contenant le nom des commandes utilisables par le client 
 * et leur descriptif : rôle et paramètres
 * @param numClient numéro du client 
 * */
void displayMan(int numClient);

/**
 * @brief Découpe le message réçu et retourne le numéro de commande associé 
 * @param msg message reçu, commençant pas / ou @
 * @return l'entier compris entre 0 et 26, correspondant au numéro de la commande, 
 *         si aucune commande n'a été reconnue retourne 0 
 * */
int numCommande(char * msg);

/**
 * @brief Informe de la validité d'un nom si celui ci ne contient pas d'espace 
 * @param name nom à vérifier 
 * @return 0 si le nom n'est pas valide, 1 sinon 
 * */
int isAvailableName(char * name);

#endif