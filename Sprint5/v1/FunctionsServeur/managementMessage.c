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
    if (strcmp(msg, "/upload\n")==0){
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

int isPresentationRoom(char * msg){
    if (strcmp(msg, "/rooms\n")==0){
        return 1;
    }
    return 0;
}

int isJoinRoom(char * msg){
    char * msgCopy = (char *) malloc(sizeof(char)*30);
    strcpy(msgCopy,msg);
    char * cmd = (char *) malloc(sizeof(char)*30);
    cmd = strtok(msgCopy," ");

    if (strcmp(cmd, "/join")==0){
        return 1;
    }
    return 0;
}

int isCreateRoom(char * msg){
    char * msgCopy = (char *) malloc(sizeof(char)*30);
    strcpy(msgCopy,msg);
    char * cmd = (char *) malloc(sizeof(char)*30);
    cmd = strtok(msgCopy," ");

    if (strcmp(cmd, "/create")==0){
        return 1;
    }
    return 0;
}

int isRemoveRoom(char * msg){
    char * msgCopy = (char *) malloc(sizeof(char)*30);
    strcpy(msgCopy,msg);
    char * cmd = (char *) malloc(sizeof(char)*30);
    cmd = strtok(msgCopy," ");

    if (strcmp(cmd, "/remove")==0){
        return 1;
    }
    return 0;
}

int isUpdateNameRoom(char * msg){
    char * msgCopy = (char *) malloc(sizeof(char)*30);
    strcpy(msgCopy,msg);
    char * cmd = (char *) malloc(sizeof(char)*30);
    cmd = strtok(msgCopy," ");

    if (strcmp(cmd, "/name")==0){
        return 1;
    }
    return 0;
}

int isUpdateDescrRoom(char * msg){
    char * msgCopy = (char *) malloc(sizeof(char)*30);
    strcpy(msgCopy,msg);
    char * cmd = (char *) malloc(sizeof(char)*30);
    cmd = strtok(msgCopy," ");

    if (strcmp(cmd, "/descr")==0){
        return 1;
    }
    return 0;
}