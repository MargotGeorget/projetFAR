#include "managementMessage.h"

void addPseudoToMsg(char * msg, char * pseudoSender){
    char * msgToSend = (char *) malloc(SIZE_MSG);
    strcpy(msgToSend, pseudoSender);
    strcat(msgToSend, " : ");
    strcat(msgToSend, msg);
    strcat(msgToSend, "\n");
    strcpy(msg,msgToSend);
    free(msgToSend);
    return;
}

void displayMan(int numClient){

	char * line = (char *)malloc(SIZE_MSG);

    /*Ouverture du fichier*/
	FILE * file = fopen("./FunctionsServeur/man.txt", "r");
	if (file == NULL){
        perror("erreur displayMan: \n");
        exit(1);
    }
    printf("Envoi du manuel en cours...\n");
    while (fgets(line, SIZE_MSG, file) != NULL){
        printf(line);
        sending(tabClient[numClient].dSC, line);
    }

    free(line);
    fclose(file);
	return;
}

int numCommande(char * msg){
    /*Récupération de la commande*/
    char * msgCopy = (char *) malloc(SIZE_MSG);
    strcpy(msgCopy,msg);
    char * cmd = strtok(msgCopy," ");
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
    }else if(strcmp(cmd, "/descrRoom")==0){
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
    }else if (strcmp(cmd, "/descr")==0){
        num = 15;
    }else if (strcmp(cmd, "/profil")==0){
        num = 16;
    }else if (strcmp(cmd, "/deleteAccount")==0){
        num = 17;
    }else if (strcmp(cmd, "/admin")==0){        
        num = 18;
    }else if (strcmp(cmd, "/move")==0){
        num = 19;
    }else if (strcmp(cmd, "/ban")==0){
        num = 20;
    }else if (strcmp(cmd, "/kick")==0){        
        num = 21;
    }else if (strcmp(cmd, "/unban")==0){        
        num = 22;
    }else if (strcmp(cmd, "/rightRoom")==0){
        num = 23;
    }else if (strcmp(cmd, "/rightServer")==0){        
        num = 24;
    }else if (strcmp(cmd, "/password")==0){        
        num = 25;
    }else if (strcmp(cmd, "/shutdown")==0){        
        num = 26;
    }
    free(msgCopy);
    return num;
}

int isAvailableName(char * name){
    int isA;

    strtok(name," ");
    char * test = strtok(NULL," "); /*On regarde si il y a un truc après l'espace*/

    if (test==NULL){
        isA = 1;
    }else{
        isA = 0;
    }
    return isA;
}