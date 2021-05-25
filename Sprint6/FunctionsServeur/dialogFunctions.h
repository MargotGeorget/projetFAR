#ifndef DIALOGFUNCTIONS_H_
#define DIALOGFUNCTIONS_H_

/* ----- FICHIER RECENSANT LES FONCTIONS DE DIALOGUE COTE SERVEUR ----- */

#include "library.h"
#include "managementMessage.h"
#include "managementClient.h"
#include "threadFunctions.h"

/**
 * @brief Fonction pour gérer le signal Ctrl C 
 * @param sign le signal 
 * */
void Ctrl_C_Handler(int sign);

/**
 * @brief Fonction pour déconnecter le serveur
 * */
void closeServeur();

/**
 * @brief Receptionne un message d'une socket et test le bon déroulement
 * @param dS la socket
 * @param msg la variable où stocker le message reçu
 * @param size taille maximum du message à recevoir
 * */
void receiving(int dS, char * rep, ssize_t size);

/**
 * @brief Reception d'un entier d'une socket et test le bon déroulement
 * @param dS la socket
 * @return l'entier reçu
 * */
int receivingInt(long dS);

/**
 * @brief Envoi un message à une socket et test le bon déroulement
 * @param dS la socket
 * @param msg le message à envoyer
 * */
void sending(int dS, char * msg);

/**
 * @brief Envoi un entier à une socket et test le bon déroulement
 * @param dS la socket
 * @param number l'entier à envoyer
 * */
void sendingInt(int dS, int number);

/**
 * @brief Envoi un message à toutes les sockets présentes dans le tableau des clients
 * et teste que tout se passe bien
 * @param numClient expéditeur du message
 * @param msg message à envoyer
 * */
void sendingAll(int numClient, char * msg);

/**
 * @brief Envoi un message à toutes les sockets des clients présents dans le salon du client
 * et teste que tout se passe bien
 * @param numClient expéditeur du message
 * @param msg message à envoyer
 * */
void sendingRoom(int numClient, char * msg);

/**
 * @brief Envoi un message à un seul client et teste que tout se passe bien
 * @param numClient expéditeur du message
 * @param msg message à envoyer contenant un @ suivi d'un pseudo
 * */
void sendingPrivate(int numClient, char * msg);

/**
 * @brief Création de la socket du serveur (socket, bind, listen) et test le bon déroulement
 * @param port numéro de port sur lequel le serveur écoute
 * @return entier représentant le numéro de la socket créée
 * */
int createSocketServeur(int port);

/**
 * @brief Accepte une connexion sur la socket passée en paramètre et test le bon déroulement
 * @param dS numéro de la socket
 * @return entier représentant le numéro de la socket client créée
 * */
int acceptConnection(int dS);

/**
 * @brief Envoi fichier à une socket et test bon déroulement
 * @param dS numéro de la socket
 * @param fp descripteur du fichier à envoyer
 * */
void sendFile(int dS, FILE * fp);

/**
 * @brief Mise en place de l'envoi d'un fichier du client vers le serveur :
 *      - reception du nom du fichier 
 *      - création du thread 
 * @param dS socket du client 
 * */
void uploadFile(int dS);

/**
 * @brief Mise en place de l'envoi d'un fichier du serveur vers le client : 
 *      - informe le thread de réception du client 
 *      - envoi des fichiers pouvant être téléchargés 
 *      - reception du nom du fichier à envoyer 
 *      - vérification de la validité du nom de fichier 
 *          ° si valide : retourne le nom du fichier au client
 *                   et création du thread d'envoi de fichier 
 *          ° sinon : envoi d'un message d'erreur pour informer le client
 * @param dS socket du client 
 * @param msgReceived message reçu (pour le retourner au client et informer le thread 
 *                      de reception du client qu'un ficheir va être envoyé)
 * */
void downloadFile(int dS,char * msgReceived);

/**
 * @brief Autorise le client à arrêter le serveur 
 * @param numClient le numéro du client qui souhaite arêter le serveur 
 * @return un booleen 1(true) si le client est administrateur et peut arrêter le serveur, 0(false) sinon 
 * */
int canShutdown(int numClient);

#endif
