#ifndef MANAGEMENTCLIENT_H_
#define MANAGEMENTCLIENT_H_

/* ----- FICHIER DE GESTION DES CLIENTS ----- */

#include "library.h"
#include "dialogFunctions.h"

/**
 * @brief Gestion des indices du tableau de clients pour trouver un emplacement disponible
 * @return un entier, indice du premier emplacement disponible dans le tableau des clients
 *         -1 si tout les emplacements sont occupés.
 */
int giveNumClient();

/**
 * @brief Recherche d'un client dans le tableau à partir de son pseudo
 * @param pseudo pseudo du client que l'on cherche
 * @return un entier, correspondant au numéro du client trouvé
 *         -1 si le client n'existe pas 
 */
int findClient(char * pseudo);

/**
 * @brief Envoie tout les pseudos des clients connectés sur le serveur au client
 * @param numClient numéro du client pour l'envoi
 * */
void displayClient(int numClient);

/**
 * @brief Envoie tout les pseudos des administrateurs du serveur au client
 * @param numClient numéro du client pour l'envoi
 * */
void displayAdmin(int numClient);

/**
 * @brief Envoie le profil d'un autre client au client
 * @param numClient numéro du client pour l'envoi
 * @param msg message reçu du client contenant la commande /profil suivi du 
 * pseudo du client pour afficher son profil 
 * */
void displayOneClient(int numClient, char * msg);

/**
 * @brief Initialisation du tableau des clients à partir du fichier clients.txt qui 
 * sauvegarde les comptes et informations des clients inscrits sur le serveur 
 * */
void initClients();

/**
 * @brief Sauvegarde des comptes et informations des clients inscrits sur le serveur dans le fichier clients.txt
 * */
void saveClients();

/**
 * @brief Modifie le pseudo du client 
 * @param numClient numéro du client 
 * @param msg message réçu contenant la commande /pseudo suivi du nouveau pseudo à mettre au client
 * */
void updatePseudo(int numClient, char * msg);

/**
 * @brief Modifie la description du client
 * @param numClient numéro du client 
 * @param msg message réçu contenant la commande /descr suivi de la nouvelle description à mettre au client
 * */
void updateDescr(int numClient, char * msg);

/**
 * @brief Modifie le mot de passe du client
 * @param numClient numéro du client 
 * @param msg message réçu contenant la commande /password suivi du mot de passe actuel et du nouveau mot de 
 * passe à mettre au client
 * si le mot de passe actuel est incorrect on ne modifie pas le mot de passe 
 * */
void updatePassword(int numClient, char * msg);

/**
 * @brief Créer un compte client 
 * @param dSC Socket du nouveau client 
 * @param pseudo Pseudo saisi par le nouveau client, qui n'est pas utilisé par un autre client 
 * @param numClient numéro du client attribué au nouveau client 
 * @return 1 si la création à été effectué, 0 si le client à saisie 'cancel' pour annuler la création de compte
 * */
int createAccount(int dSC, char * pseudo, int numClient);

/**
 * @brief Permet à un client de se connecter 
 * @param dSC Socket du client 
 * @param numClient numéro du client trouvé à partir du pseudo saisie précédemment par le client 
 * @return 1 si la connexion à été effectué, 0 si le client à saisie 'cancel' pour annuler la connexion 
 * */
int connection(int dSC, int numClient);

/**
 * @brief Donne le nombre d'administrateurs du serveur 
 * @return Nombre d'administrateurs
 * */
int nbAdmin();

int deleteAccount(int numClient);

void giveRightServer(int numClient, char * msg);

void closingClient(int dS);

/**
 * Terminaison des threads pour lesquels les clients se sont déconnectés
 */
void killThread();

#endif