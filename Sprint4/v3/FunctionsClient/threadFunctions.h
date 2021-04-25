#ifndef THREADFUNCTIONS_H_
#define THREADFUNCTIONS_H_

#include "library.h"
#include "managementMessage.h"
#include "dialogFunctions.h"

/* -- Fonction pour le thread d'envoi de fichier -- */
void * sendingFile_th(void * fileNameParam);

/* -- Fonction pour le thread de reception de fichier -- */
void * receivingFile_th(void * fileNameParam);

/* -- Fonction pour le thread d'envoi -- */
void * sending_th(void * dSparam);

/* -- Fonction pour le thread de reception -- */
void * receiving_th(void * dSparam);

#endif