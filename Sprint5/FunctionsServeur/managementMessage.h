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

/**
 * Envoi du fichier contenant le nom des commandes utilisables par le client 
 * et leur descriptif : rôle et paramètres
 * Paramètre : int : numéro du client 
 * */
void displayMan(int numClient);

/**
 * Découpe le message réçu et retourne le numéro de commande associé 
 * Paramètre : char * : message reçu, commençant pas / ou @
 * Retour : int : correspond au numéro de la commande (voir fichier cmd.txt), 
 *                si aucune commande n'a été reconnue retourne 0 
 * */
int numCommande(char * msg);

#endif