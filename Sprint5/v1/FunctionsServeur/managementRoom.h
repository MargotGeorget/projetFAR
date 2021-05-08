#ifndef MANAGEMENTROOM_H_
#define MANAGEMENTROOM_H_

/* ----- FICHIER DE GESTION DES ROOMS ----- */

#include "library.h"
#include "dialogFunctions.h"

int giveIdRoom();

void initRoom();

void presentationRoom(int dS);

void createRoom(int numClient, char * msg);

int getRoomByName(char * nameRoom);

void addMember(int numClient, int idRoom);

void deleteMember(int numClient, int idRoom);

void joinRoom(int numClient, char * msg);

void updateRoom();

void removeRoom(int numClient, char * msg);

int getNonCreatedRoom();

#endif