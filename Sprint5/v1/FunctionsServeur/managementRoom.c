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
    FILE *fp = fopen("room.txt","r");
    if(fp==NULL){
        printf("error\n");
    }
    printf("fichier ouvert\n");

    pthread_mutex_lock(&lock); /*Début d'une section critique*/
    printf("luuuu\n");
    while(fgets(buffer,100, (FILE *) fp)!=NULL && i<NB_ROOMS){
        printf("lecture fichier\n");
        rooms[i].id = atoi(strtok(buffer,","));
        rooms[i].name = (char *)malloc(sizeof(char)*100);
        rooms[i].name = strtok(NULL,",");
        rooms[i].created = 0;
        printf("fin d'une boucle\n");
        i++;
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    fclose(fp);
}

void presentationRoom(int dS){
    int i;
    int j;
    char * msg = (char *)malloc(sizeof(char)*100);

    for (i=0;i<NB_ROOMS;i++){
        if (rooms[i].created){
            strcpy(msg,"** ");
            strcat(msg,rooms[i].name);
            strcat(msg," **\n");
            
            for(j=0;j<rooms[i].nbClient;j++){
                strcat(msg,rooms[i].members[j].pseudo);
                strcat(msg,"\n");
            }
        }
    }
    sending(dS,msg);
    free(msg);
}

void createRoom(char * nameRoom, int idRoom){

    int i = 0;
    FILE *fp;
    fp = fopen("room.txt","w");

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    rooms[idRoom].name = nameRoom;

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
}