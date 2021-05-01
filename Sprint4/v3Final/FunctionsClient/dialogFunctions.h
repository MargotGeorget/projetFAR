#ifndef DIALOGFUNCTIONS_H_
#define DIALOGFUNCTIONS_H_

/* ----- FICHIER RECENSANT LES FONCTIONS DE DIALOGUE CÔTE CLIENT ----- */

#include "library.h"
#include "threadFunctions.h"

/*
 * Envoi d'un message à une socket et test bon déroulement
 * Paramètres : int dS : la socket
 *              char * msg : message à envoyer
 * */
void sending(int dS, char * msg);

/*
 * Envoi d'un entier à une socket et test bon déroulement
 * Paramètres : int dS : la socket
 *              int number : entier à envoyer
 * */
void sendingInt(int dS, int number);

/*
 * Reception d'un message d'une socket et test bon déroulement
 * Paramètres : int dS : la socket
 *              char * rep : message à recevoir
 *              ssize_t size : taille maximum du message à recevoir
 * */
void receiving(int dS, char * rep, ssize_t size);

/*
 * Reception d'un entier d'une socket et test bon déroulement
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
int createSocketClient(int port, char * ip);

/*
 * Mise en place de l'envoi d'un fichier et création du thread nécessaire
 * Paramètres : int dS : la socket courante du client
 * */
void sendingFile(int dS);

/*
 * Mise en place de la reception d'un fichier dans le thread de reception
 *          - reception des fichiers pouvant être téléchargés
 *          - affichage des fichiers au client
 *          - réception de la confirmation du serveur de l'existence du fichier
 *          - création du thread de réception
 * Paramètres : int dS : la socket courante du client
 * */
void receivingFileReceiving_th(int dS);

/*
 * Mise en place de la reception de fichier
 *          - saisie du nom du fichier par le client 
 *          - envoi du nom de fichier au serveur
 * Paramètres : int dS : la socket courante du client
 * */
void receivingFileSending_th(int dS);

#endif