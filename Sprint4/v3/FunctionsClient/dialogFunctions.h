#ifndef DIALOGFUNCTIONS_H_
#define DIALOGFUNCTIONS_H_

#include "library.h"
#include "threadFunctions.h"

/*
 * Envoi un message à une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              char * msg : message à envoyer
 * Retour : pas de retour
 * */
void sending(int dS, char * msg);

/*
 * Envoi un entier à une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              int number : entier à envoyer
 * Retour : pas de retour
 * */
void sendingInt(int dS, int number);

/*
 * Receptionne un message d'une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              char * msg : message à recevoir
 *              ssize_t size : taille maximum du message à recevoir
 * Retour : pas de retour
 * */
void receiving(int dS, char * rep, ssize_t size);

/*
 * Receptionne un entier d'une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 * Retour : l'entier reçu
 * */
int receivingInt(long dS);

/*
 * Création et demande de connexion d'une socket client à un serveur 
 * Paramètres : int port : port de la socket serveur
 *              char * ip : ip du serveur
 * Retour : la socket créée
 * */
int createSocketCLient(int port, char * ip);

/*
 * Met en place l'envoi de fichier et créer le thread nécessaire 
 * Paramètres : int dS : la socket courante du client 
 * Retour : pas de retour
 * */
void sendingFile(int dS);

#endif