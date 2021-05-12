#include "managementRoom.h"

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
        rooms[i].descr = (char *)malloc(sizeof(char)*100);
        strcpy(rooms[i].descr,strtok(NULL,","));
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

void welcomeMsg(int dS){
    int i;
    char * msg = (char *)malloc(sizeof(char)*300);
    strcpy(msg,"\n___Bienvenu dans le salon général___\nVoici les membres présents : \n");

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    for (i=0;i<MAX_CLIENT;i++){
        if(rooms[0].members[i] && tabClient[rooms[0].members[i]].occupied){
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
            strcat(msg,"\n** ");
            strcat(msg,rooms[i].name);
            strcat(msg," **\n");
            strcat(msg," -- ");
            strcat(msg,rooms[i].descr);
            strcat(msg," --\n");
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
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    strcat(msg,"______________________________________\n\n");
    printf("%s\n",msg);
    sending(dS,msg);
    free(msg);
}

void createRoom(int numClient, char * msg) {
    char * error = (char *)malloc(sizeof(char)*60);

    char *  roomName =  (char *) malloc(sizeof(char)*300);
    strtok(msg," ");
    roomName = strtok(NULL,"\0");

    /*ID*/
    int idRoom = getNonCreatedRoom();

    if(idRoom != NB_ROOMS){
        pthread_mutex_lock(&lock); /*Début d'une section critique*/
        
        /*CREATED*/
        rooms[idRoom].created = 1;
        strcpy(rooms[idRoom].name,roomName);
        /*MAJ NOM dans le fichier*/
        updateRoom();

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
        
    }else {
        strcpy(error, "Le nombre maximum de salons a été atteint.\n");
        sending(tabClient[numClient].dSC, error);
    }
    free(error);
}

int getRoomByName(char * roomName){
    int found = 0;
    int i = 0; 
    int indice = -1;
    
    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while(i<NB_ROOMS && !found){
        printf("nom du salon : .%s.\n",rooms[i].name);
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

    char *  nameRoom =  (char *) malloc(sizeof(char)*300);
    strtok(msg," ");
    nameRoom = strtok(NULL,"\0");

    /*Récupération de l'id du salon choisi par le client*/
    int idRoom = getRoomByName(nameRoom);

    if(idRoom == -1){

        strcpy(error, "Aucun salon trouvé.\n");
        sending(tabClient[numClient].dSC, error);

    }else {

        pthread_mutex_lock(&lock); /*Début d'une section critique*/
        int idRoomClient = tabClient[numClient].idRoom;
        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

        deleteMember(numClient,idRoomClient);
        addMember(numClient,idRoom);
    }

    return;
}

/*Attention cette fonction est toujours appelée dans une section critique, ne pas mettre de mutex dedans*/
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
    strtok(msg," ");
    roomName = strtok(NULL,"\0");

    /*ID*/
    int idRoom = getRoomByName(roomName);

    if(idRoom == -1){

        strcpy(error, "Aucun salon trouvé.\n");
        sending(tabClient[numClient].dSC, error);

    }else if(idRoom == 0){ /*On ne peut modifier le salon principal*/

        strcpy(error, "Vous ne pouvez pas supprimer le salon général.\n");
        sending(tabClient[numClient].dSC, error);

    }else if(isOccupiedRoom(idRoom)) {
        /*Des clients sont présents dans le salon à supprimer*/
        strcpy(error, "Vous ne pouvez pas supprimer un salon occupé.\n");
        sending(tabClient[numClient].dSC, error);
    }else{
        
        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        rooms[idRoom].created=0;
        strcpy(rooms[idRoom].descr,"Default");

        /*MAJ NOM dans le fichier*/
        updateRoom();

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
        
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
    strtok(msg," ");
    roomName = strtok(NULL," ");

    /*On récupère le nouveau nom*/
    char *  newName =  (char *) malloc(sizeof(char)*300);
    newName = strtok(NULL,"\0");

    /*ID*/
    int idRoom = getRoomByName(roomName);

    if(idRoom == -1){

        strcpy(error, "Aucun salon trouvé.\n");
        sending(tabClient[numClient].dSC, error);

    }else if(idRoom == 0){ /*On ne peut modifier le salon principal*/

        strcpy(error, "Vous ne pouvez pas modifier le salon général.\n");
        sending(tabClient[numClient].dSC, error);

    }else{

        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        strcpy(rooms[idRoom].name,newName);
    
        /*MAJ NOM dans le fichier*/
        updateRoom();

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
    }
}

void updateDescrRoom(int numClient, char * msg){
    char * error = (char *)malloc(sizeof(char)*60);

    /*On récupère le nom du salon à modifier*/
    char *  roomName =  (char *) malloc(sizeof(char)*300);
    strtok(msg," ");
    roomName = strtok(NULL," ");

    /*On récupère le nouveau nom*/
    char *  newDescr =  (char *) malloc(sizeof(char)*300);
    newDescr = strtok(NULL,"\0");

    /*ID*/
    int idRoom = getRoomByName(roomName);

    if(idRoom == -1){

        strcpy(error, "Aucun salon trouvé.\n");
        sending(tabClient[numClient].dSC, error);

    }else if(idRoom == 0){ /*On ne peut modifier le salon principal*/

        strcpy(error, "Vous ne pouvez pas modifier le salon général.\n");
        sending(tabClient[numClient].dSC, error);

    }else{

        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        strcpy(rooms[idRoom].descr,newDescr);
    
        /*MAJ NOM dans le fichier*/
        updateRoom();

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
    }
}