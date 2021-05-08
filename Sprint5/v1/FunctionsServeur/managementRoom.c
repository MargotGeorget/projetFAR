#include "managementRoom.h"

int giveIdRoom(){
    int i = 0;
    int indice = -1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<NB_ROOMS && indice==-1){
        if(!rooms[i].created){
            indice = i;
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return indice;
}

void initRoom(){
    int i = 0;
    char buffer[100] = "";
    FILE *fp = fopen("FunctionsServeur/room.txt","r");
    if(fp==NULL){
        perror("error\n");
    }

    pthread_mutex_lock(&lock); /*Début d'une section critique*/
    
    while(fgets(buffer,100, (FILE *) fp)!=NULL && i<NB_ROOMS){
        
        rooms[i].id = atoi(strtok(buffer,","));
        rooms[i].name = (char *)malloc(sizeof(char)*100);
        strcpy(rooms[i].name,strtok(NULL,","));
        rooms[i].created = atoi(strtok(NULL,","));
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

void presentationRoom(int dS){
    int i;
    int j;
    char * msg = (char *)malloc(sizeof(char)*300);
    strcpy(msg," ");

    for (i=0;i<NB_ROOMS;i++){
        if (rooms[i].created){
            strcat(msg,"\n** ");
            strcat(msg,rooms[i].name);
            strcat(msg," **\n");
            /*
             * tabClient[rooms[i].members[j]].occupied
             * On récupère la room d'indice i dans le tableau des rooms
             * On regarde un a un les membres en retournant leur numClient c a d qui correspond à leur indice dans le tableau des clients 
             * A partir de cet indice on recupère la struc client associé dans le tableau des Clients et on regarde si le client est présent c a d occupied 
             * */
            for(j=0;j<MAX_CLIENT;j++){
                if(rooms[i].members[j] && tabClient[rooms[i].members[j]].occupied){
                    strcat(msg,tabClient[j].pseudo);
                    strcat(msg,"\n");
                }

            }
        }
    }
    printf("%s\n",msg);
    sending(dS,msg);
    free(msg);
}

void createRoom(int numClient, char * msg) {
    char * error = (char *)malloc(sizeof(char)*60);

    char *  roomName =  (char *) malloc(sizeof(char)*300);
    strtok(msg," ");
    roomName = strtok(NULL,"\n");

    /*ID*/
    int idRoom = getNonCreatedRoom();

    if(idRoom != NB_ROOMS){
        /*CREATED*/
        if(idRoom != 0){ /*On ne peut modifier le salon principal*/
            rooms[idRoom].created = 1;
            strcpy(rooms[idRoom].name,roomName);
            /*MAJ NOM dans le fichier*/
            updateRoom();
        }else {
            strcpy(error, "Vous ne pouvez pas changer le nom du salon général.\n");
            sending(tabClient[numClient].dSC, error);
        }
    }else {
        strcpy(error, "Le nombre maximum de salons a été créé.\n");
        sending(tabClient[numClient].dSC, error);
    }
    free(error);
}

int getRoomByName(char * roomName){
    int found = 0;
    int i = 0; 
    int indice = -1;
    

    while(i<NB_ROOMS && !found){
        printf("nom du salon : %s\n",rooms[i].name);
        if(strcmp(rooms[i].name, roomName) == 0){
            found = 1;
            indice = i;
        }

        i++;
    }
    return indice;
}

void addMember(int numClient, int idRoom){
    /*Ajout de l'id du salon au client*/
    tabClient[numClient].idRoom = idRoom;
    /*On indique dans les membres du salon que le client est présent*/
    printf("avant add member : %s\n", tabClient[rooms[idRoom].members[numClient]].pseudo);
    rooms[idRoom].members[numClient] = 1;
    printf("add member : %s\n", tabClient[rooms[idRoom].members[numClient]].pseudo);
    
    /*Envoi d'un message pour informer les autres membres du salon*/
    char * joinNotification =  (char *) malloc(sizeof(char)*100);
    strcpy(joinNotification,"** a rejoint le salon **\n");
    sendingRoom(numClient, joinNotification); 

    free(joinNotification);
}

void deleteMember(int numClient, int idRoom){
    printf("Fonction deleteMember()\n");

    /*Envoi d'un message pour informer les autres membres du salon*/
    char * leaveNotification =  (char *) malloc(sizeof(char)*100);
    strcpy(leaveNotification,"** a quitté le salon **\n");
    sendingRoom(numClient, leaveNotification); 

    printf("message de déconnexion envoyé\n");

    /*On indique dans les membres du salon que le client n'est plus présent*/
    rooms[idRoom].members[numClient] = 0;
    
    free(leaveNotification);

    printf("fin de la fonction deleteMember\n");
}

void joinRoom(int numClient, char * msg){
    printf("Fonction joinRoom()\n");

    deleteMember(numClient,tabClient[numClient].idRoom);
    
    char *  nameRoom =  (char *) malloc(sizeof(char)*300);
    strtok(msg," ");
    nameRoom = strtok(NULL,"\n");

    printf("gestion nom du salon : %s\n",nameRoom);

    /*Récupération de l'id du salon choisi par le client*/
    int idRoom = getRoomByName(nameRoom);

    printf("indice salon à rejoindre : %d\n",idRoom);
    /*ToDo vérifier retour si -1 aucun salon à ce nom*/
    addMember(numClient,idRoom);
    return;
}

void updateRoom(){

    char * line = (char *)malloc(sizeof(char)*100);
    
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

        char * id = malloc(sizeof(int));
        char * create = malloc(sizeof(int));

        /*ID*/
        sprintf(id,"%d",rooms[i].id);
        strcpy(line,id);
        strcat(line,",");

        /*NOM*/
        strcat(line,rooms[i].name);
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

void removeRoom(int numClient, char * msg){
    char *  roomName =  (char *) malloc(sizeof(char)*300);
    strtok(msg," ");
    roomName = strtok(NULL,"\n");

    
    /*ToDo message de confirmation si salon occupé par d'autres membres*/

    /*ToDo: renvoyer les membres du salon dans le général*/

    /*ID*/
    int idRoom = getRoomByName(roomName);

    rooms[idRoom].created=0;

    /*MAJ NOM dans le fichier*/
    updateRoom();
}

int getNonCreatedRoom(){
    int i = 0;
    while(i<NB_ROOMS && rooms[i].created){
        i++;
    }
    return i;
}

