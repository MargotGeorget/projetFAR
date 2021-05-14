#include "managementRoom.h"

void initRoom(){
    int i = 0;
    char buffer[100] = "";

    /*Ouverture du fichier contenant la dernière sauvegarde des salons*/
    FILE *fp = fopen("FunctionsServeur/room.txt","r");
    if(fp==NULL){
        perror("error\n");
    }

    pthread_mutex_lock(&lock); /*Début d'une section critique*/
    
    /*Initialisation du tableau des salons à partir du contenu du fichier*/
    while(fgets(buffer,100, (FILE *) fp)!=NULL && i<NB_ROOMS){
        
        /*ID*/
        rooms[i].id = atoi(strtok(buffer,","));

        /*NAME*/
        rooms[i].name = (char *)malloc(sizeof(char)*100);
        strcpy(rooms[i].name,strtok(NULL,","));

        /*DESCRIPTION*/
        rooms[i].descr = (char *)malloc(sizeof(char)*100);
        strcpy(rooms[i].descr,strtok(NULL,","));

        /*CREATED*/
        rooms[i].created = atoi(strtok(NULL,","));

        /*MEMBERS*/
        /*Initialisation du tableau des membres à 0 (false: pas présent)*/
        int j;
        for (j=0;j<MAX_CLIENT;j++){
            rooms[i].members[j]=0;
        }
        i++;
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    fclose(fp);

    printf("\n-- Salons initialisés --\n");
}

void welcomeMsg(int dS){
    int i;
    char * msg = (char *)malloc(sizeof(char)*300);
    strcpy(msg,"\n___Bienvenue dans le salon général___\nVoici les membres présents : \n");

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    /*Ajout des membres présents dans la salon général*/
    for (i=0;i<MAX_CLIENT;i++){
        if(rooms[0].members[i]){
            strcat(msg,tabClient[i].pseudo);
            strcat(msg,"\n");
        }
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    strcat(msg,"________Bonne communication________\n");
    sending(dS,msg);
    free(msg);
}

void presentationRoom(int dS){
    int i;
    int j;
    char * msg = (char *)malloc(sizeof(char)*400);
    strcpy(msg,"\n______Liste des salons existants______\n");

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    for (i=0;i<NB_ROOMS;i++){
        if (rooms[i].created){

            /*NAME*/
            strcat(msg,"\n** ");
            strcat(msg,rooms[i].name);
            strcat(msg," **\n");

            /*DESCRIPTION*/
            strcat(msg," -- ");
            strcat(msg,rooms[i].descr);
            strcat(msg," --\n");

            /*MEMBERS*/
            for(j=0;j<MAX_CLIENT;j++){
                if(rooms[i].members[j]){
                    strcat(msg,tabClient[j].pseudo);
                    strcat(msg,"\n");
                }
            }
        } 
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    strcat(msg,"______________________________________\n\n");
    printf("%s\n",msg);

    /*Envoi du message de présentation*/
    sending(dS,msg);
    free(msg);
}

void createRoom(int numClient, char * msg) {
    char * error = (char *)malloc(sizeof(char)*60);

    char *  roomName =  (char *) malloc(sizeof(char)*300);
    strtok(msg," "); /*suppression de la commande dans le message*/
    roomName = strtok(NULL,"\0"); /*récupération du nom du salon à créer*/

    if(roomName==NULL){
        strcpy(error, "Saisissez un nom de salon.\n");
        sending(tabClient[numClient].dSC, error);       
    }else if(!isAvailableName(roomName)){
         strcpy(error, "Un nom de salon ne peut pas contenir d'espace.\n");
        sending(tabClient[numClient].dSC, error);  
    }else{

        /*ID*/
        int idRoom = getNonCreatedRoom();

        if(idRoom != NB_ROOMS){ /*Un salon est disponible*/
            pthread_mutex_lock(&lock); /*Début d'une section critique*/
            
            /*CREATED*/
            rooms[idRoom].created = 1;
            /*NAME*/
            strcpy(rooms[idRoom].name,roomName);
            
            /*MAJ NOM dans le fichier*/
            updateRoom();

            pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
            
        }else { /*Aucun salon n'est disponible*/
            strcpy(error, "Le nombre maximum de salons a été atteint.\n");
            sending(tabClient[numClient].dSC, error);
        }
    }
    /*free(roomName);
    free(error);*/
}

int getRoomByName(char * roomName){
    int found = 0;
    int i = 0; 
    int indice = -1;
    
    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while(i<NB_ROOMS && !found){
        if(strcmp(rooms[i].name, roomName) == 0){
            found = 1;
            indice = i;
        }
        i++;
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return indice;
}

void addMember(int numClient, int idRoom){

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    /*Ajout de l'id du salon au client*/
    tabClient[numClient].idRoom = idRoom;
    /*On indique dans les membres du salon que le client est présent*/
    rooms[idRoom].members[numClient] = 1;

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
    
    /*Envoi d'un message pour informer les autres membres du salon*/
    char * joinNotification =  (char *) malloc(sizeof(char)*100);
    strcpy(joinNotification,"** a rejoint le salon **\n");
    sendingRoom(numClient, joinNotification); 

    free(joinNotification);
}

void deleteMember(int numClient, int idRoom){
    /*Envoi d'un message pour informer les autres membres du salon*/
    char * leaveNotification =  (char *) malloc(sizeof(char)*100);
    strcpy(leaveNotification,"** a quitté le salon **\n");
    sendingRoom(numClient, leaveNotification); 
        
    free(leaveNotification);

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    /*On indique dans les membres du salon que le client n'est plus présent*/
    rooms[idRoom].members[numClient] = 0;

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

}

void joinRoom(int numClient, char * msg){
    char * error = (char *)malloc(sizeof(char)*60);

    char *  roomName =  (char *) malloc(sizeof(char)*300);
    strtok(msg," "); /*suppression de la commande dans le message*/
    roomName = strtok(NULL,"\0"); /*récupération du nom du salon à créer*/

    if(roomName==NULL){
        strcpy(error, "Saisissez un nom de salon.\n");
        sending(tabClient[numClient].dSC, error);       
    }else{
        /*ID*/
        int idRoom = getRoomByName(roomName);

        if(idRoom == -1){ /*Aucun salon n'a été trouvé*/

            strcpy(error, "Aucun salon trouvé.\n");
            sending(tabClient[numClient].dSC, error);

        }else { /*Un salon à été trouvé, on fait le changement de salon*/

            pthread_mutex_lock(&lock); /*Début d'une section critique*/
            int idRoomClient = tabClient[numClient].idRoom;
            pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

            deleteMember(numClient,idRoomClient);
            addMember(numClient,idRoom);
        }
    }

    return;
}

/*Attention cette fonction est toujours appelée dans une section critique, ne pas mettre de mutex dedans*/
void updateRoom(){
    char * line = (char *)malloc(sizeof(char)*200);
    
    /*Suppression fichier*/
    int ret = remove("./FunctionsServeur/room.txt");
    if (ret == -1){
        perror("erreur suppression updateRoom: \n");
        exit(1);
    }

    /*Création du fichier*/
    int fd = open("./FunctionsServeur/room.txt", O_CREAT | O_WRONLY, 0666);
    if (fd == -1){
        perror("erreur création updateRoom: \n");
        exit(1);
    }

    int i;
    for (i = 0; i < NB_ROOMS; i++){
        printf("for %d",i);
        char * id = malloc(sizeof(int));
        char * create = malloc(sizeof(int));

        /*ID*/
        sprintf(id,"%d",rooms[i].id);
        strcpy(line,id);
        strcat(line,",");

        /*NOM*/
        strcat(line,rooms[i].name);
        strcat(line,",");

        /*DESCRIPTION*/
        strcat(line,rooms[i].descr);
        strcat(line,",");

        /*CREATED*/
        sprintf(create,"%d",rooms[i].created);
        strcat(line, create);
        strcat(line, "\n\0");
        
        int w = write(fd,line,strlen(line));
        if(w == -1){
            perror("erreur au write\n");
            exit(1);
        }

        free(id);
        free(create);
    }
    free(line);
    return;
}
 
int isOccupiedRoom(idRoom){
    int isO = 0;
    int i = 0;
    while (i<MAX_CLIENT){
        isO = rooms[idRoom].members[i];
        i++;
    }
    return isO;
 }

void removeRoom(int numClient, char * msg){
    char * error = (char *)malloc(sizeof(char)*60);

    char *  roomName =  (char *) malloc(sizeof(char)*300);
    strtok(msg," "); /*suppression de la commande dans le message*/
    roomName = strtok(NULL,"\0"); /*récupération du nom du salon à créer*/

    if(roomName==NULL){
        strcpy(error, "Saisissez un nom de salon.\n");
        sending(tabClient[numClient].dSC, error);       
    }else{

        /*ID*/
        int idRoom = getRoomByName(roomName);

        if(idRoom == -1){ /*Aucun salon n'a été trouvé*/

            strcpy(error, "Aucun salon trouvé.\n");
            sending(tabClient[numClient].dSC, error);

        }else if(idRoom == 0){ /*On ne peut modifier le salon principal*/

            strcpy(error, "Vous ne pouvez pas supprimer le salon général.\n");
            sending(tabClient[numClient].dSC, error);

        }else if(isOccupiedRoom(idRoom)) { /*Des clients sont présents dans le salon à supprimer*/
            
            strcpy(error, "Vous ne pouvez pas supprimer un salon occupé.\n");
            sending(tabClient[numClient].dSC, error);

        }else{ /*Le salon peut être supprimé*/
            
            pthread_mutex_lock(&lock); /*Début d'une section critique*/

            rooms[idRoom].created=0;
            strcpy(rooms[idRoom].descr,"Default");

            /*MAJ NOM dans le fichier*/
            updateRoom();

            pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
            
        }
    }
}

int getNonCreatedRoom(){
    int i = 0;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while(i<NB_ROOMS && rooms[i].created){
        i++;
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return i;
}

void updateNameRoom(int numClient, char * msg){
    char * error = (char *)malloc(sizeof(char)*60);

    /*On récupère le nom du salon à modifier*/
    char *  roomName =  (char *) malloc(sizeof(char)*300);
    strtok(msg," "); /*suppression de la commande dans le message*/
    roomName = strtok(NULL," "); /*récupération du nom du salon à créer*/

    if(roomName==NULL){
        strcpy(error, "Saisissez un nom de salon existant et un nouveau nom de salon.\n");
        sending(tabClient[numClient].dSC, error);       
    }else {

        /*On récupère le nouveau nom*/
        char *  newName =  (char *) malloc(sizeof(char)*300);
        newName = strtok(NULL,"\0");

        if(newName==NULL){
            strcpy(error, "Saisissez un nouveau nom de salon.\n");
            sending(tabClient[numClient].dSC, error);     

        }else if(!isAvailableName(newName)){
            strcpy(error, "Un nom de salon ne peut pas contenir d'espace.\n");
            sending(tabClient[numClient].dSC, error);  
        }else{

            /*ID*/
            int idRoom = getRoomByName(roomName);

            if(idRoom == -1){ /*Aucun salon n'a été trouvé*/

                strcpy(error, "Aucun salon trouvé.\n");
                sending(tabClient[numClient].dSC, error);

            }else if(idRoom == 0){ /*On ne peut modifier le salon principal*/

                strcpy(error, "Vous ne pouvez pas modifier le salon général.\n");
                sending(tabClient[numClient].dSC, error);

            }else{ /*On peut modifier le salon*/

                pthread_mutex_lock(&lock); /*Début d'une section critique*/

                strcpy(rooms[idRoom].name,newName);
            
                /*MAJ NOM dans le fichier*/
                updateRoom();

                pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
            }
        }
    }
}

void updateDescrRoom(int numClient, char * msg){
    char * error = (char *)malloc(sizeof(char)*60);

    /*On récupère le nom du salon à modifier*/
    char *  roomName =  (char *) malloc(sizeof(char)*300);
    strtok(msg," "); /*suppression de la commande dans le message*/
    roomName = strtok(NULL," "); /*récupération du nom du salon à créer*/

    if(roomName==NULL){
        strcpy(error, "Saisissez un nom de salon existant et une nouvelle description.\n");
        sending(tabClient[numClient].dSC, error);       
    }else {

        /*On récupère la nouveau description*/
        char *  newDescr =  (char *) malloc(sizeof(char)*300);
        newDescr = strtok(NULL,"\0");

        if(newDescr==NULL){
            strcpy(error, "Saisissez une nouvelle description.\n");
            sending(tabClient[numClient].dSC, error);     

        }else{

            /*ID*/
            int idRoom = getRoomByName(roomName);

            if(idRoom == -1){ /*Aucun salon n'a été trouvé*/

                strcpy(error, "Aucun salon trouvé.\n");
                sending(tabClient[numClient].dSC, error);

            }else if(idRoom == 0){ /*On ne peut modifier le salon principal*/

                strcpy(error, "Vous ne pouvez pas modifier le salon général.\n");
                sending(tabClient[numClient].dSC, error);

            }else{ /*On peut modifier le salon*/

                pthread_mutex_lock(&lock); /*Début d'une section critique*/

                strcpy(rooms[idRoom].descr,newDescr);
            
                /*MAJ NOM dans le fichier*/
                updateRoom();

                pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
            }
        }
    }
}