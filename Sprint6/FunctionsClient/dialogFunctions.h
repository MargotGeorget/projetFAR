#ifndef DIALOGFUNCTIONS_H_
#define DIALOGFUNCTIONS_H_

/* ----- FICHIER RECENSANT LES FONCTIONS DE DIALOGUE CÔTE CLIENT ----- */

#include "library.h"
#include "threadFunctions.h"

/**
 * @brief Fonction pour gérer le signal Ctrl C 
 * @param sign le signal 
 * */
void Ctrl_C_Handler(int sign);

/**
 * @brief Envoi d'un message à une socket et test le bon déroulement
 * @param dS la socket
 * @param msg message à envoyer
 * */
void sending(int dS, char * msg);

/**
 * @brief Envoi d'un entier à une socket et test bon déroulement
 * @param dS la socket
 * @param number entier à envoyer
 * */
void sendingInt(int dS, int number);

/**
 * @brief Reception d'un message d'une socket et test bon déroulement
 * @param dS la socket
 * @param rep la variable où stocker le message reçu
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
 * @brief Création et demande de connexion d'une socket client à un serveur 
 * @param port port de la socket serveur
 * @param ip ip du serveur
 * @return la socket créée
 * */
int createSocketClient(int port, char * ip);

/**
 * @brief Mise en place de l'envoi d'un fichier et création du thread nécessaire
 * @param dS la socket courante du client
 * */
void uploadFile(int dS);

/**
 * @brief Mise en place de la reception d'un fichier dans le thread de reception
 *          - reception des fichiers pouvant être téléchargés
 *          - affichage des fichiers au client
 *          - réception de la confirmation du serveur de l'existence du fichier
 *          - création du thread de réception
 * @param dS la socket courante du client
 * */
void downloadFile(int dS);

#endif