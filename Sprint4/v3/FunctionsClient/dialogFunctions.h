#ifndef DIALOGFUNCTIONS_H_
#define DIALOGFUNCTIONS_H_

#include "library.h"

/*
 * Envoi un message à une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              char * msg : message à envoyer
 * Retour : pas de retour
 * */
void sending(int dS, char * msg);

void sendingInt(int dS, int number);

/*
 * Receptionne un message d'une socket et teste que tout se passe bien
 * Paramètres : int dS : la socket
 *              char * msg : message à recevoir
 *              ssize_t size : taille maximum du message à recevoir
 * Retour : pas de retour
 * */
void receiving(int dS, char * rep, ssize_t size);

int receivingInt(long dS);

int createSocketCLient(char * port, char * ip);

#endif