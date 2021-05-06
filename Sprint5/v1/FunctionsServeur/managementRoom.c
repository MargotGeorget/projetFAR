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
        printf("room: %s\n",rooms[i].name);
        rooms[i].created = atoi(strtok(NULL,","));
        i++;
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    fclose(fp);
}

void presentationRoom(int dS){
    int i;
    int j;
    char * msg = (char *)malloc(sizeof(char)*300);
    strcpy(msg," ");
    printf("ici\n");

    for (i=0;i<NB_ROOMS;i++){
        if (rooms[i].created){
            strcat(msg,"\n** ");
            strcat(msg,rooms[i].name);
            strcat(msg," **\n");
            strcat(msg,"id : ");
            char * id = malloc(sizeof(int));
            snprintf(id,sizeof(int),"%d",rooms[i].id);
            strcat(msg,id);
            printf("llla\n");
            strcat(msg,"\n");
            
            for(j=0;j<MAX_CLIENT;j++){
                printf("ouccipied%d\n",rooms[i].&members[j].occupied);
                if(rooms[i].&members[j].occupied){
                    strcat(msg,rooms[i].&members[j].pseudo);
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
        if(!rooms[idRoom-1].&members[i].occupied){
            indice = i;
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return indice;
}

void addMember(int numClient, int idRoom){
    tabClient[numClient].idRoom = idRoom;

    int i = getFreePlaceMember(idRoom);
    printf("indice : %d",i);
    rooms[idRoom-1].members[i] = &tabClient[numClient];

    return;
}

void deleteMember(int numClient, int idRoom){

    return;
}