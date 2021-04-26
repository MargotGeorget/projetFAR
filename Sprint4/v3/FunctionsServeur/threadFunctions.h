#ifndef THREADFUNCTIONS_H_
#define THREADFUNCTIONS_H_

#include "library.h"
#include "managementMessage.h"
#include "dialogFunctions.h"

void * receivingFile_th(void * fileNameParam);

void * sendingFile_th(void * fp);

/*
 * Start routine de pthread_create()
 * */
void * broadcast(void * clientParam);

#endif