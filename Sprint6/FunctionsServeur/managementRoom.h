#ifndef MANAGEMENTROOM_H_
#define MANAGEMENTROOM_H_

/* ----- FICHIER DE GESTION DES ROOMS ----- */

#include "library.h"
#include "dialogFunctions.h"

/**
 * Initialisation des salons de discussion à partir d'un fichier de sauvegarde.
 * */
void initRoom();

/**
 * Diffusion d'un message de bienvenue à un client lors de sa connexion.
 * Paramètre : int dS : socket du client à laquelle envoyer le message.
 * */
void welcomeMsg(int dS);

/**
 * Affichage des salons de discussion acessibles (nom et description) 
 * ainsi que les clients connectés à ces derniers.
 * Paramètre : int dS : socket du client à laquelle envoyer le message.
 * */
void presentationRoom(int dS);

/**
 * Crée (rend visible) un salon de discussion avec le nom passé en paramètre.
 * Paramètres : int numClient : numéro du client permettant d'accéder à sa socket, pour lui envoyer un message.
 *              char * msg : message reçu du client, dans lequel se trouve le nom du salon.
 * */
void createRoom(int numClient, char * msg);

/**
 * Renvoie le numéro du salon de discussion associé au nom passé en paramètre, renvoie -1 s'il ne le trouve pas.
 * Paramètres : char * nameRoom : nom du salon à chercher.
 * */
int getRoomByName(char * nameRoom);

/**
 * Ajoute le client au salon, passés en paramètre, et informe les autres clients de ce salon.
 * Paramètres : int numClient : numéro du client.
 *              int idRoom : identifiant du salon à rejoindre.
 * */
void addMember(int numClient, int idRoom);

/**
 * Supprime le client du salon, passés en paramètre, et informe les autres clients de ce salon.
 * Paramètres : int numClient : numéro du client.
 *              int idRoom : identifiant du salon à quitter.
 * */
void deleteMember(int numClient, int idRoom);

/**
 * Retire le client de son salon actuel et l'ajoute au nouveau salon, dont le nom est passé en paramètre.
 * Paramètres : int numClient : numéro du client.
 *              char * msg : message reçu du client, dans lequel se trouve le nom du salon.
 * */
void joinRoom(int numClient, char * msg);

void moveClient(int numClient, char * msg);

void kickClient(int numClient, char * msg);

void banClient(int numClient, char * msg);

void unbanClient(int numClient, char * msg);

void giveRightRoom(int numClient, char * msg);

/**
 * Mise à jour des fichiers qui sauvegardent les informations des salons. 
 * Paramètres : int room : Booleen à 1(true) s'il faut mettre à jour le fichier room.txt
 *              int admin : Booleen à 1(true) s'il faut mettre à jour le fichier adminRoom.txt
 *              int ban : Booleen à 1(true) s'il faut mettre à jour le fichier ban.txt
 * */
void updateRoom(int room, int admin, int ban);

/**
 * Renvoie un booléen signalant s'il y a des clients dans le salon de discussion dont le numéro est passé en paramètre. 
 * Paramètres : int idRoom : numéro du salon.
 * */
int isOccupiedRoom(int idRoom);

/**
 * Supprime (rend invisible) le salon dont le nom est passé en paramètre.
 * Paramètres : int numClient : numéro du client.
 *              char * msg : message reçu du client, dans lequel se trouve le nom du salon.
 * */
void removeRoom(int numClient, char * msg);

/**
 * Renvoir l'identifiant du premier salon de discussion non créé (invisible à l'utilisateur).
 * Retour : int : identifiant du premier salon disponible, 
 *                si aucun salon disponible retourne NB_ROOMS (identifiant qui n'existe pas)
 * */
int getNonCreatedRoom();

/**
 * Modifie le nom du salon avec le nouveau nom passé en paramètre.
 * Paramètres : int numClient : numéro du client.
 *              char * msg : message reçu du client, dans lequel se trouve le nom du salon à modifier 
 *              et le nouveau nom.
 * */
void updateNameRoom(int numClient, char * msg);

/**
 * Modifie la description du salon avec la nouvelle description passée en paramètre.
 * Paramètres : int numClient : numéro du client.
 *              char * msg : message reçu du client, dans lequel se trouve le nom du salon à modifier
 *              et la nouvelle description du salon.
 * */
void updateDescrRoom(int numClient, char * msg);

#endif