#include "managementMessage.h"

void addPseudoToMsg(char * msg, char * pseudoSender){
    char * msgToSend = (char *) malloc(sizeof(char)*150);
    strcpy(msgToSend, pseudoSender);
    strcat(msgToSend, " : ");
    strcat(msgToSend, msg);
    strcpy(msg,msgToSend);
    free(msgToSend);
    return;
}

int endOfCommunication(char * msg){
    if (strcmp(msg, "** a quitté la communication **\n")==0){
        return 1;
    }
    return 0;
}

int isReceivingFile(char * msg){
    if (strcmp(msg, "/file\n")==0){
        return 1;
    }
    return 0;
}

int isSendingFile(char * msg){
    if (strcmp(msg, "/download\n")==0){
        return 1;
    }
    return 0;
}