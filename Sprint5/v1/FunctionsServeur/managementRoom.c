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

void createRoom(char * nameRoom, int idRoom){
}

Room getRoomById(int idRoom){
    int i = 0;
    int found = 0;
    Room result;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<NB_ROOMS && !found){
        if(rooms[i].id==idRoom){
            result = rooms[i];
            found = 1;
        }
        i+=1;
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return result;
}

int getFreePlaceMember(int idRoom){
    int i = 0;
    int indice = -1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<MAX_CLIENT && indice==-1){
        if(rooms[idRoom].members[i]==-1 || !tabClient[rooms[idRoom].members[i]].occupied){
            indice = i;
            printf("indice libre: %d\n",indice);
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return indice;
}

int getIdRoomByName(char * nameRoom){
    return 1;
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

    /*On indique dans les membres du salon que le client n'est plus présent*/
    rooms[idRoom].members[numClient] = 0;

    /*Envoi d'un message pour informer les autres membres du salon*/
    char * leaveNotification =  (char *) malloc(sizeof(char)*100);
    strcpy(leaveNotification,"** a quitté le salon **\n");
    sendingRoom(numClient, leaveNotification); 
    
    free(leaveNotification);
}

void joinRoom(int numClient, char * msg){

    deleteMember(numClient,tabClient[numClient].idRoom);
    
    char *  nameRoom =  (char *) malloc(sizeof(char)*300);
    strtok(msg," ");
    nameRoom = strtok(NULL," ");

    /*Récupération de l'id du salon choisi par le client*/
    int idRoom = getIdRoomByName(nameRoom);

    addMember(numClient,idRoom);
    return;
}

void updateRoomName(char * newName, int idRoom){
    strcpy(rooms[idRoom].name, newName);

    /*toDo : changer dans le fichier*/
}

void removeRoom(int idRoom){
    rooms[idRoom].created=0;

    /*toDo : changer dans le fichier*/
}