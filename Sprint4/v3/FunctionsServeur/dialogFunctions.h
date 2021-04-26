#ifndef DIALOGFUNCTIONS_H_
#define DIALOGFUNCTIONS_H_

#include "library.h"
#include "managementMessage.h"
#include "managementClient.h"
/*
 * Receptionne un message d'une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              char * msg : message à recevoir
 *              ssize_t size : taille maximum du message à recevoir
 * Retour : pas de retour
*/
void receiving(int dS, char * rep, ssize_t size);

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
 * Envoi un message à toutes les sockets présentent dans le tableau des clients
 * et teste que tout se passe bien
 * Paramètres : int numClient: expéditeur du message
 *              char * msg : message à envoyer
 * Retour : pas de retour
*/
void sendingAll(int numClient, char * msg);

/*
 * Envoi un message à un seul client
 * et teste que tout se passe bien
 * Paramètres : int numClient : expéditeur du message
 *              char * msg : message à envoyer contenant un @ suivi d'un pseudo
 * Retour : pas de retour
*/
void sendingPrivate(int numClient, char * msg);

int createSocketServeur(int port);

int acceptConnection(int dS);

void sendFile(int dS, FILE * fp);

#endif
