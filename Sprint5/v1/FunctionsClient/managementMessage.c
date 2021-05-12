#include "managementMessage.h"

int endOfCommunication(char * msg){
    if (strcmp(msg, "/end\n")==0){
        return 1;
    }
    return 0;
}

int isSendingFile(char * msg){
    if (strcmp(msg, "/upload")==0){
        return 1;
    }
    return 0;
}

int isReceivingFile(char * msg){
    if (strcmp(msg, "/download\n")==0){
        return 1;
    }
    return 0;
}
