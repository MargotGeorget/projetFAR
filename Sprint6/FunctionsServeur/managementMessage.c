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

void displayMan(int numClient){

	char * line = (char *)malloc(sizeof(char)*100);

	FILE * file = fopen("./FunctionsServeur/man.txt", "r");
	if (file == NULL){
        perror("erreur displayMan: \n");
        exit(1);
    }

    int i;
    while (fgets(line, sizeof(char)*100, file) != NULL){
        sending(tabClient[numClient].dSC, line);
        i++;
    }

    free(line);

	return;
}

int numCommande(char * msg){
    /*Récupération de la commande*/
    char * msgCopy = (char *) malloc(sizeof(char)*30);
    strcpy(msgCopy,msg);
    char * cmd = (char *) malloc(sizeof(char)*30);
    cmd = strtok(msgCopy," ");
    char first = cmd[0];

    /*Analyse de la commande pour trouver son numéro*/
    int num = 0;
    if (strcmp(cmd, "/man")==0){
        num = 1;
    }else if(strcmp(cmd, "/whoishere")==0){
        num = 2;
    }else if(strcmp(cmd, "/pseudo")==0){
        num = 3;
    }else if(strcmp(cmd, "/rooms")==0){
        num = 4;
    }else if(strcmp(cmd, "/join")==0){
        num = 5;
    }else if(strcmp(cmd, "/create")==0){
        num = 6;
    }else if(strcmp(cmd, "/remove")==0){
        num = 7;
    }else if(strcmp(cmd, "/name")==0){
        num = 8;
    }else if(strcmp(cmd, "/descr")==0){
        num = 9;
    }else if(strcmp(cmd, "/upload")==0){
        num = 10;
    }else if (strcmp(cmd, "/download")==0){
        num = 11;
    }else if (strcmp(cmd, "/end")==0){
        num = 12;
    }else if (strcmp(cmd, "@all")==0){
        num = 13;
    }else if (strcmp(&first, "@")==0){        
        num = 14;
    }
    return num;
}

int isAvailableName(char * name){
    int isA;

    char * test = (char *)malloc(sizeof(char)*100); 
    strtok(name," ");
    test = strtok(NULL," ");
    printf("test :%s\n",test);

    if (test==NULL){
        isA = 1;
    }else{
        isA = 0;
    }
    return isA;
}