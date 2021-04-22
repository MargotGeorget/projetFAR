#include "library.h"

int endOfCommunication(char * msg){
    if (strcmp(msg, "fin\n")==0){
        strcpy(msg, "** a quitté la communication **\n");
        return 1;
    }
    return 0;
}

int isSendingFile(char * msg){
    if (strcmp(msg, "/file\n")==0){
        return 1;
    }
    return 0;
}
