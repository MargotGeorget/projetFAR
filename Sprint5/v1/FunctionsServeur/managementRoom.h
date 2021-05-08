#ifndef MANAGEMENTROOM_H_
#define MANAGEMENTROOM_H_

/* ----- FICHIER DE GESTION DES ROOMS ----- */

#include "library.h"
#include "dialogFunctions.h"

int giveIdRoom();

void initRoom();

void presentationRoom(int dS);

void createRoom(char * nameRoom, int idRoom);

Room getRoomById(int idRoom);

int getFreePlaceMember(int idRoom);

int getIdRoomByName(char * nameRoom);

void addMember(int numClient, int idRoom);

void deleteMember(int numClient, int idRoom);

void joinRoom(int numClient, char * msg);

#endif