#ifndef MANAGEMENTROOM_H_
#define MANAGEMENTROOM_H_

/* ----- FICHIER DE GESTION DES ROOMS ----- */

#include "library.h"
#include "dialogFunctions.h"

/**
 * @brief Initialisation des salons de discussion à partir d'un fichier de sauvegarde.
 * */
void initRoom();

/**
 * @brief Diffusion d'un message de bienvenue à un client lors de sa connexion.
 * @param dS socket du client à laquelle envoyer le message.
 * */
void welcomeMsg(int dS);

/**
 * @brief Affichage des salons de discussion acessibles (nom, description, adminitrateurs et membres) 
 * @param dS socket du client à laquelle envoyer le message.
 * */
void presentationRoom(int dS);

/**
 * @brief Crée (rend visible) un salon de discussion avec le nom passé en paramètre.
 * @param numClient numéro du client permettant d'accéder à sa socket, pour lui envoyer un message.
 * @param msg message reçu du client contenant la commande /create suivi du nom du salon à créer 
 * */
void createRoom(int numClient, char * msg);

/**
 * @brief Renvoie le numéro du salon de discussion associé au nom passé en paramètre, renvoie -1 s'il ne le trouve pas.
 * @param nameRoom nom du salon à chercher.
 * */
int getRoomByName(char * nameRoom);

/**
 * @brief Ajoute le client au salon, passés en paramètre, et informe les autres clients de ce salon.
 * @param numClient numéro du client.
 * @param idRoom identifiant du salon à rejoindre.
 * */
void addMember(int numClient, int idRoom);

/**
 * @brief Supprime le client du salon passé en paramètre, et informe les autres clients de ce salon.
 * @param numClient numéro du client.
 * @param idRoom identifiant du salon à quitter.
 * */
void deleteMember(int numClient, int idRoom);

/**
 * @brief Retire le client de son salon actuel et l'ajoute au nouveau salon, dont le nom est passé en paramètre.
 * @param numClient numéro du client.
 * @param msg message reçu du client contenant la commande /join suivi du nom du salon à rejoindre.
 * */
void joinRoom(int numClient, char * msg);

/**
 * @brief Déplace le client demandé dans le salon donné, uniquement si le client qui envoie la commande est un administrateur du serveur
 * @param numClient numéro du client qui demande le déplacement 
 * @param msg message reçu du client contenant la commande /move suivi du nom du client à déplacer puis le nom salon. 
 * */
void moveClient(int numClient, char * msg);

/**
 * @brief Retire le client de son salon actuel et le replace dans le général. 
 * Uniquement si le client qui envoie la commande est un administrateur du serveur ou du salon concerné
 * @param numClient numéro du client qui envoie la commande
 * @param msg message reçu du client contenant la commande /kick suivi du nom du client à déplacer.
 * */
void kickClient(int numClient, char * msg);

/**
 * @brief Retire le client du salon (s'il est présent dans le salon concerné) et le replace dans le général. 
 * Le client ne pourra plus retourner dans ce salon par la suite.
 * Uniquement si le client qui envoie la commande est un administrateur du serveur ou du salon concerné
 * @param numClient numéro du client qui envoie la commande
 * @param msg message reçu du client contenant la commande /ban suivi du nom du client à bannir et du salon concerné.
 * */
void banClient(int numClient, char * msg);

/**
 * @brief Redonne les droits au client de rejoindre le salon concerné.
 * Uniquement si le client qui envoie la commande est un administrateur du serveur ou du salon concerné
 * @param numClient numéro du client qui envoie la commande
 * @param msg message reçu du client contenant la commande /unban suivi du nom du client à débannir et du salon concerné.
 * */
void unbanClient(int numClient, char * msg);

/**
 * @brief Donne les droits de modifications et de suppression d'une room à un client.
 * Uniquement si le client qui envoie la commande est un administrateur du serveur ou du salon concerné
 * @param numClient numéro du client qui envoie la commande
 * @param msg message reçu du client contenant la commande /rightRoom suivi du nom du client et du salon concernés.
 * */
void giveRightRoom(int numClient, char * msg);

/**
 * @brief Mise à jour des fichiers qui sauvegardent les informations des salons. 
 * @param room booleen à 1(true) s'il faut mettre à jour le fichier room.txt
 * @param admin booleen à 1(true) s'il faut mettre à jour le fichier adminRoom.txt
 * @param ban booleen à 1(true) s'il faut mettre à jour le fichier ban.txt
 * */
void updateRoom(int room, int admin, int ban);

/**
 * @brief Renvoie un booléen signalant s'il y a des clients dans le salon de discussion dont le numéro est passé en paramètre. 
 * @param idRoom numéro du salon.
 * @return booleen à 1(true) si des clients sont présent dans le salon, 0 sinon 
 * */
int isOccupiedRoom(int idRoom);

/**
 * @brief Supprime (rend invisible) le salon dont le nom est passé en paramètre.
 * Uniquement si le client qui envoie la commande est un administrateur du serveur ou du salon concerné
 * @param numClient numéro du client qui envoie la commande
 * @param msg message reçu du client contenant la commande /remove suivi du nom du salon concerné
 * */
void removeRoom(int numClient, char * msg);

/**
 * @brief Renvoie l'identifiant du premier salon de discussion non créé (invisible à l'utilisateur).
 * @return identifiant du premier salon disponible, 
 *         si aucun salon disponible retourne NB_ROOMS (identifiant qui n'existe pas)
 * */
int getNonCreatedRoom();

/**
 * @brief Modifie le nom du salon avec le nouveau nom passé en paramètre.
 * Uniquement si le client qui envoie la commande est un administrateur du serveur ou du salon concerné
 * @param numClient numéro du client qui envoie la commande 
 * @param msg message reçu du client contenant la commande /name suivi du nom du salon concerné et du nom à donner au salon.
 * */
void updateNameRoom(int numClient, char * msg);

/**
 * @brief Modifie la description du salon avec la nouvelle description passée en paramètre.
 * Uniquement si le client qui envoie la commande est un administrateur du serveur ou du salon concerné
 * @param numClient numéro du client qui envoie la commande 
 * @param msg message reçu du client contenant la commande /descrRoom suivi du nom du salon concerné et de la nouvelle description à donner au salon.
 * */
void updateDescrRoom(int numClient, char * msg);

#endif