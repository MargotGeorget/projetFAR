#include "managementMessage.h"

int endOfCommunication(char * msg){
    if (strcmp(msg, "/end")==0){
        return 1;
    }
    return 0;
}

int isUploadFile(char * msg){
    if (strcmp(msg, "/upload")==0){
        return 1;
    }
    return 0;
}

int isDownloadFile(char * msg){
    if (strcmp(msg, "/download")==0){
        return 1;
    }
    return 0;
}

int isServeurShutdown(char * msg){
    if (strcmp(msg, "/shutdown")==0){
        printf("Le serveur à été fermé! Revenez plus tard\n");
        return 1;
    }
    return 0;
}