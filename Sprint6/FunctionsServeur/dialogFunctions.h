#ifndef DIALOGFUNCTIONS_H_
#define DIALOGFUNCTIONS_H_

/* ----- FICHIER RECENSANT LES FONCTIONS DE DIALOGUE COTE SERVEUR ----- */

#include "library.h"
#include "managementMessage.h"
#include "managementClient.h"
#include "threadFunctions.h"

/**
 * Receptionne un message d'une socket et test bon déroulement
 * Paramètres : int dS : la socket
 *              char * msg : message à recevoir
 *              ssize_t size : taille maximum du message à recevoir
 * */
void receiving(int dS, char * rep, ssize_t size);

/**
 * Reception d'un entier d'une socket et test bon déroulement
 * Paramètres : int dS : la socket
 * Retour : l'entier reçu
 * */
int receivingInt(long dS);

/**
 * Envoi un message à une socket et test bon déroulement
 * Paramètres : int dS : la socket
 *              char * msg : message à envoyer
 * */
void sending(int dS, char * msg);

/**
 * Envoi un entier à une socket et test bon déroulement
 * Paramètres : int dS : la socket
 *              int number : entier à envoyer
 * */
void sendingInt(int dS, int number);

/**
 * Envoi un message à toutes les sockets présentes dans le tableau des clients
 * et teste que tout se passe bien
 * Paramètres : int numClient : expéditeur du message
 *              char * msg : message à envoyer
 * */
void sendingAll(int numClient, char * msg);

/**
 * Envoi un message à toutes les sockets des clients présents dans le salon du client
 * et teste que tout se passe bien
 * Paramètres : int numClient : expéditeur du message
 *              char * msg : message à envoyer
 * */
void sendingRoom(int numClient, char * msg);

/**
 * Envoi un message à un seul client
 * et teste que tout se passe bien
 * Paramètres : int numClient : expéditeur du message
 *              char * msg : message à envoyer contenant un @ suivi d'un pseudo
 * */
void sendingPrivate(int numClient, char * msg);

/**
 * Création de la socket du serveur (socket, bind, listen) et test bon déroulement
 * Paramètres : int port : numéro de port sur lequel le serveur écoute
 * Retour : entier représentant le numéro de la socket créée
 * */
int createSocketServeur(int port);

/**
 * Accepte une connexion sur la socket passée en paramètre et test bon déroulement
 * Paramètres : int dS : numéro de la socket
 * Retour : entier représentant le numéro de la socket client créée
 * */
int acceptConnection(int dS);

/**
 * Envoi fichier à une socket et test bon déroulement
 * Paramètres : int dS : numéro de la socket
 *              FILE * fp : descripteur du fichier à envoyer
 * Retour : pas de retour
 * */
void sendFile(int dS, FILE * fp);

/**
 * Mise en place de l'envoi d'un fichier du client vers le serveur :
 *      - reception du nom du fichier 
 *      - création du thread 
 * Paramètre : int dS : socket du client 
 * */
void uploadFile(int dS);

/**
 * Mise en place de l'envoi d'un fichier du serveur vers le client : 
 *      - informe le thread de réception du client 
 *      - envoi des fichiers pouvant être téléchargés 
 *      - reception du nom du fichier à envoyer 
 *      - vérification de la validité du nom de fichier 
 *          ° si valide : retourne le nom du fichier au client
 *                   et création du thread d'envoi de fichier 
 *          ° sinon : envoi d'un message d'erreur pour informer le client
 * Paramètre : int dS : socket du client 
 *             char * msgReceived : message reçu
 * */
void downloadFile(int dS,char * msgReceived);

#endif
