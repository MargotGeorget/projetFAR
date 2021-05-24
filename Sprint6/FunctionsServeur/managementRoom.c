#include "managementRoom.h"

void initRoom(){
    int i = 0;
    char buffer[100] = "";
    char bufferAdmin[100] = "";
    char bufferBan[100] = "";
    

    /*Ouverture du fichier contenant la dernière sauvegarde des salons*/
    FILE *fp = fopen("FunctionsServeur/room.txt","r");
    if(fp==NULL){
        perror("error\n");
    }
    FILE *fpAdmin = fopen("FunctionsServeur/adminRoom.txt","r");
    if(fpAdmin==NULL){
        perror("error\n");
    }
    FILE *fpBan = fopen("FunctionsServeur/ban.txt","r");
    if(fpBan==NULL){
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
        rooms[i].descr = (char *)malloc(sizeof(char)*300);
        strcpy(rooms[i].descr,strtok(NULL,","));

        /*CREATED*/
        rooms[i].created = atoi(strtok(NULL,","));

        /*MEMBERS*/
        /*Initialisation du tableau des membres à 0 (false: pas présent)*/
        int j;
        for (j=0;j<MAX_CLIENT;j++){
            rooms[i].members[j]=0;
        }

        /*ADMIN*/
        /*Initialisation du tableau des admins à partir du fichier adminRoom.txt*/
        int b = 0;
        fgets(bufferAdmin,100, (FILE *) fpAdmin);
        char * isAdmin = strtok(bufferAdmin,",");
        while (b<MAX_CLIENT && isAdmin!=NULL){
            rooms[i].admin[b]= atoi(isAdmin);
            isAdmin = strtok(NULL,",");
            b++;
        }
        while(b<MAX_CLIENT){ 
            /*Si le fichier n'était pas complet (on à peut être augmenté le nombre MAX_CLIENT) alors on initialise le reste à 0*/
            rooms[i].admin[b] = 0;
            b++;
        }

        /*BAN*/
        /*Initialisation du tableau des bans à partir du fichier ban.txt*/
        b = 0;
        fgets(bufferBan,100, (FILE *) fpBan);
        char * isBan = strtok(bufferAdmin,",");
        while (b<MAX_CLIENT && isBan!=NULL){
            rooms[i].ban[b]= atoi(isBan);
            isBan = strtok(NULL,",");
            b++;
        }
        while(b<MAX_CLIENT){ 
            /*Si le fichier n'était pas complet (on à peut être augmenté le nombre MAX_CLIENT) alors on initialise le reste à 0*/
            rooms[i].ban[b] = 0;
            b++;
        }
        
        i++;
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    fclose(fp);
    fclose(fpAdmin);
    fclose(fpBan);

    printf("\n-- Salons initialisés --\n");
}

void welcomeMsg(int dS){
    int i;
    char * msg = (char *)malloc(sizeof(char)*(200+15*MAX_CLIENT));
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
    char * msg = (char *)malloc(sizeof(char)*(350+(2*20*MAX_CLIENT)));
    strcpy(msg,"\n______Liste des salons existants______\n");

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    for (i=0;i<NB_ROOMS;i++){
        if (rooms[i].created){

            /*NAME*/
            strcat(msg,"\n** ");
            strcat(msg,rooms[i].name);
            strcat(msg," **\n");

            /*DESCRIPTION*/
            strcat(msg,"-- ");
            strcat(msg,rooms[i].descr);
            strcat(msg," --\n");

            /*ADMIN*/
            if(i!=0){
                strcat(msg,"~~ Admin : ");
                for(j=0;j<MAX_CLIENT;j++){
                    if(rooms[i].admin[j]){
                        strcat(msg,tabClient[j].pseudo);
                        strcat(msg," ~ ");
                    }
                }
                strcat(msg,"~\n");
            }
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

    strtok(msg," "); /*suppression de la commande dans le message*/
    char * roomName = strtok(NULL,"\0"); /*récupération du nom du salon à créer*/

    if(roomName==NULL){
        sending(tabClient[numClient].dSC, "Saisissez un nom de salon.\n");       
    }else if(!isAvailableName(roomName)){
        sending(tabClient[numClient].dSC, "Un nom de salon ne peut pas contenir d'espace.\n");  
    }else{

        /*ID*/
        int idRoom = getNonCreatedRoom();
        printf("idRoom : %d\n", idRoom);

        if(idRoom != NB_ROOMS){ /*Un salon est disponible*/
            pthread_mutex_lock(&lock); /*Début d'une section critique*/
            
            /*CREATED*/
            rooms[idRoom].created = 1;
            /*NAME*/
            strcpy(rooms[idRoom].name,roomName);
            /*Le client créateur devient l'administrateur de la room*/
            rooms[idRoom].admin[numClient]=1; 
            /*MAJ NOM dans le fichier*/
            updateRoom(1,1,0);

            sending(tabClient[numClient].dSC,  "Le salon à été créé, vous êtes l'administrateur!\n");

            pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
            
        }else { /*Aucun salon n'est disponible*/
            sending(tabClient[numClient].dSC, "Le nombre maximum de salons a été atteint.\n");
        }
    }
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
    char * msg = (char *)malloc(sizeof(char)*100);
    strcpy(msg,"** a rejoint le salon **\n");
    sendingRoom(numClient, msg);
    free(msg);

}

void deleteMember(int numClient, int idRoom){
    /*Envoi d'un message pour informer les autres membres du salon*/
    char * msg = (char *)malloc(sizeof(char)*100);
    strcpy(msg,"** a quitté le salon **\n");
    sendingRoom(numClient, msg);
    free(msg);

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    /*On indique dans les membres du salon que le client n'est plus présent*/
    rooms[idRoom].members[numClient] = 0;

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

}

void joinRoom(int numClient, char * msg){

    strtok(msg," "); /*suppression de la commande dans le message*/
    char * roomName = strtok(NULL,"\0"); /*récupération du nom du salon à créer*/

    if(roomName==NULL){
        sending(tabClient[numClient].dSC, "Saisissez un nom de salon.\n");       
    }else{
        /*ID*/
        int idRoom = getRoomByName(roomName);

        if(idRoom == -1){ /*Aucun salon n'a été trouvé*/

            sending(tabClient[numClient].dSC, "Aucun salon trouvé.\n");

        }else if(rooms[idRoom].ban[numClient]){

            sending(tabClient[numClient].dSC, "Vous avez été banni de ce salon, vous ne pouvez plus le rejoindre.\n");

        }else{ /*Un salon à été trouvé, on fait le changement de salon*/

            pthread_mutex_lock(&lock); /*Début d'une section critique*/
            int idRoomClient = tabClient[numClient].idRoom;
            pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

            deleteMember(numClient,idRoomClient);
            addMember(numClient,idRoom);

            sending(tabClient[numClient].dSC, "Vous avez changé de salon.\n"); 
        }
    }

    return;
}

void moveClient(int numClient, char * msg){

    strtok(msg," "); /*suppression de la commande dans le message*/
    char * pseudo = strtok(NULL," "); /*récupération du pseudo du client à déplacer*/

    if(pseudo==NULL){
        sending(tabClient[numClient].dSC, "Saisissez le pseudo d'un client.\n");       
    }else{
        char * roomName = strtok(NULL,"\0"); /*récupération du nom du salon où envoyer le client*/

        if(roomName==NULL){
            sending(tabClient[numClient].dSC, "Saisissez le nom d'un salon.\n"); 
        }else {
            int client = findClient(pseudo);
            int idRoom = getRoomByName(roomName);
            if(client==-1){
                sending(tabClient[numClient].dSC, "Aucun client trouvé.\n"); 
            }else if(idRoom==-1){
                sending(tabClient[numClient].dSC, "Aucun salon trouvé.\n"); 
            }else if(!tabClient[numClient].isAdmin) { 
                sending(tabClient[numClient].dSC, "Vous n'avez pas les droits pour déplacer le client\n"); 
            }else{ /*Un salon et un client ont été trouvés et le client à la droit, on fait le déplacement*/

                pthread_mutex_lock(&lock); /*Début d'une section critique*/
                int idRoomClient = tabClient[client].idRoom;
                pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

                deleteMember(client,idRoomClient);
                addMember(client,idRoom);

                sending(tabClient[numClient].dSC, "Le client a été déplacé.\n"); 

                sending(tabClient[client].dSC, "Vous avez été déplacé dans un autre salon.\n"); 
            }
        }
    }

    return;
}

void kickClient(int numClient, char * msg){

    strtok(msg," "); /*suppression de la commande dans le message*/
    char * pseudo = strtok(NULL," "); /*récupération du pseudo du client à kick*/

    if(pseudo==NULL){
        sending(tabClient[numClient].dSC, "Saisissez le pseudo d'un client.\n");       
    }else{
        
        int client = findClient(pseudo);
        if(client==-1){
            sending(tabClient[numClient].dSC, "Aucun client trouvé.\n");  
        }else {
            int idRoomClient = tabClient[client].idRoom;
            if(!rooms[idRoomClient].admin[numClient] || !tabClient[numClient].isAdmin) { 
                sending(tabClient[numClient].dSC, "Vous n'avez pas les droits pour exclure le client\n"); 
            }else{ /*Un salon et un client ont été trouvés et le client à la droit, on fait le déplacement*/

                pthread_mutex_lock(&lock); /*Début d'une section critique*/
                int idRoomClient = tabClient[client].idRoom;
                pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

                deleteMember(client,idRoomClient);
                addMember(client,0);

                sending(tabClient[numClient].dSC, "Le client a été exclu du salon.\n"); 

                sending(tabClient[client].dSC, "Vous avez été exclu du salon. Vous êtes retourné dans le salon général.\n"); 
            }
        }
    }

    return;
}

void banClient(int numClient, char * msg){

    strtok(msg," "); /*suppression de la commande dans le message*/
    char * pseudo = strtok(NULL," "); /*récupération du pseudo du client à ban*/

    if(pseudo==NULL){
        sending(tabClient[numClient].dSC, "Saisissez le pseudo d'un client.\n");       
    }else{
        char * roomName = strtok(NULL,"\0"); /*récupération du nom du salon dans lequel ban le client*/

        if(roomName==NULL){
            sending(tabClient[numClient].dSC, "Saisissez le nom d'un salon.\n"); 
        }else {
            int client = findClient(pseudo);
            int idRoom = getRoomByName(roomName);
            if(client==-1){
                sending(tabClient[numClient].dSC, "Aucun client trouvé.\n"); 
            }else if(idRoom==-1){
                sending(tabClient[numClient].dSC, "Aucun salon trouvé.\n"); 
            }else if(!tabClient[numClient].isAdmin && !rooms[idRoom].admin[numClient]) { 
                sending(tabClient[numClient].dSC,  "Vous n'avez pas les droits pour bannir le client.\n"); 
            }else if(idRoom==0){
                sending(tabClient[numClient].dSC,  "Vous ne pouvez pas banir un client du salon général.\n");            
            }else{ /*Un salon et un client ont été trouvés et le client à les droits*/
                pthread_mutex_lock(&lock); /*Début d'une section critique*/
                int idRoomClient = tabClient[client].idRoom;
                pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
                if (idRoomClient==idRoom){
                    deleteMember(client,idRoomClient);
                    addMember(client,0); /*On le renvoie dans le général*/
                }
                
                rooms[idRoom].ban[client]=1;

                /*MAJ NOM dans le fichier*/
                updateRoom(0,0,1);

                sending(tabClient[numClient].dSC, "Le client a été banni du salon\n"); 

                char * info = (char *)malloc(sizeof(char)*60);
                strcpy(info, "Vous avez été banni du salon ");
                strcat(info,roomName);
                strcat(info,"\n");
                sending(tabClient[client].dSC, info); 
                free(info);
            }
        }
    }

    return;
}

void unbanClient(int numClient, char * msg){

    strtok(msg," "); /*suppression de la commande dans le message*/
    char * pseudo = strtok(NULL," "); /*récupération du pseudo du client à ban*/

    if(pseudo==NULL){
        sending(tabClient[numClient].dSC, "Saisissez le pseudo d'un client.\n");       
    }else{
        char * roomName = strtok(NULL,"\0"); /*récupération du nom du salon dans lequel ban le client*/

        if(roomName==NULL){
            sending(tabClient[numClient].dSC, "Saisissez le nom d'un salon.\n"); 
        }else {
            int client = findClient(pseudo);
            int idRoom = getRoomByName(roomName);
            if(client==-1){
                sending(tabClient[numClient].dSC, "Aucun client trouvé.\n"); 
            }else if(idRoom==-1){
                sending(tabClient[numClient].dSC, "Aucun salon trouvé.\n"); 
            }else if(!tabClient[numClient].isAdmin && !rooms[idRoom].admin[numClient]) { 
                sending(tabClient[numClient].dSC, "Vous n'avez pas les droits pour débannir le client\n"); 
            }else{ /*Un salon et un client ont été trouvés et le client à les droits*/
                
                rooms[idRoom].ban[client]=0;

                /*MAJ NOM dans le fichier*/
                updateRoom(0,0,1);

                sending(tabClient[numClient].dSC, "Le client pourra de nouveau rejoindre le salon.\n"); 
                
                char * info = (char *)malloc(sizeof(char)*60);
                strcpy(info, "Vous pouvez à nouveau rejoindre le salon ");
                strcat(info,roomName);
                strcat(info,"\n");
                sending(tabClient[client].dSC, info); 
                free(info);
            }
        }
    }

    return;
}

void giveRightRoom(int numClient, char * msg){

    strtok(msg," "); /*suppression de la commande dans le message*/
    char * pseudo = strtok(NULL," "); /*récupération du pseudo du client pour lui donner les droits*/

    if(pseudo==NULL){
        sending(tabClient[numClient].dSC, "Saisissez le pseudo d'un client.\n");       
    }else{
        char * roomName = strtok(NULL,"\0"); /*récupération du nom du salon auquel lui donner les droits*/

        if(roomName==NULL){
            sending(tabClient[numClient].dSC, "Saisissez le nom d'un salon.\n"); 
        }else {
            int client = findClient(pseudo);
            int idRoom = getRoomByName(roomName);
            if(client==-1){
                sending(tabClient[numClient].dSC, "Aucun client trouvé.\n"); 
            }else if(idRoom==-1){
                sending(tabClient[numClient].dSC, "Aucun salon trouvé.\n"); 
            }else if(!tabClient[numClient].isAdmin && !rooms[idRoom].admin[numClient]) { 
                sending(tabClient[numClient].dSC, "Vous n'avez pas les droits pour donner des droits sur ce salon\n"); 
            }else{ /*Un salon et un client ont été trouvés et le client à les droits*/
              
                rooms[idRoom].admin[client]=1;
                
                /*MAJ NOM dans le fichier*/
                updateRoom(0,1,0);

                sending(tabClient[numClient].dSC, "Les droits du salon ont été transmis au client.\n"); 

                char * info = (char *)malloc(sizeof(char)*60);
                strcpy(info, "Vous avez été déclaré administrateur du salon ");
                strcat(info,roomName);
                strcat(info,"\n");
                sending(tabClient[client].dSC, info); 
                free(info);
            }
        }
    }

    return;
}
/*Attention cette fonction est toujours appelée dans une section critique, ne pas mettre de mutex dedans*/
void updateRoom(int room, int admin, int ban){
    char * line = (char *)malloc(sizeof(char)*200);
    
    if(room){
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
            char id[2];
            char create[2];

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

        }

        close(fd);
    }

    if(admin){
        /*Mise à jour fichier admin*/

        /*Suppression fichier*/
        int ret = remove("./FunctionsServeur/adminRoom.txt");
        if (ret == -1){
            perror("erreur suppression adminRoom: \n");
            exit(1);
        }

        int fd = open("./FunctionsServeur/adminRoom.txt", O_CREAT | O_WRONLY, 0666);
        if (fd == -1){
            perror("erreur création adminRoom: \n");
            exit(1);
        }
        char isAdmin[2];
        int i;
        for (i = 0; i < NB_ROOMS; i++){
            int j;
            strcpy(line,"");
            for (j = 0; j < MAX_CLIENT; j++){
                sprintf(isAdmin,"%d",rooms[i].admin[j]);
                strcat(line,isAdmin);
                strcat(line,",");
            }
            strcat(line,"\n");
            int w = write(fd,line,strlen(line));
            if(w == -1){
                perror("erreur au write\n");
                exit(1);
            }
        }
        close(fd);
    }

    if(ban){
        /*Mise à jour fichier ban*/

        /*Suppression fichier*/
        int ret = remove("./FunctionsServeur/ban.txt");
        if (ret == -1){
            perror("erreur suppression ban: \n");
            exit(1);
        }

        int fd = open("./FunctionsServeur/ban.txt", O_CREAT | O_WRONLY, 0666);
        if (fd == -1){
            perror("erreur création ban: \n");
            exit(1);
        }
        char isBan[2];
        int i;
        for (i = 0; i < NB_ROOMS; i++){
            int j;
            strcpy(line,"");
            for (j = 0; j < MAX_CLIENT; j++){
                sprintf(isBan,"%d",rooms[i].ban[j]);
                strcat(line,isBan);
                strcat(line,",");
            }
            strcat(line,"\n");
            int w = write(fd,line,strlen(line));
            if(w == -1){
                perror("erreur au write\n");
                exit(1);
            }
        }
        close(fd);
    }
    
    free(line);

    return;
}
 
int isOccupiedRoom(idRoom){
    int isO = 0;
    int i = 0;
    while (i<MAX_CLIENT && !isO){
        isO = rooms[idRoom].members[i];
        i++;
    }
    return isO;
 }

void removeRoom(int numClient, char * msg){

    strtok(msg," "); /*suppression de la commande dans le message*/
    char * roomName = strtok(NULL,"\0"); /*récupération du nom du salon à créer*/

    if(roomName==NULL){
        sending(tabClient[numClient].dSC, "Saisissez un nom de salon.\n");       
    }else{

        /*ID*/
        int idRoom = getRoomByName(roomName);

        if(idRoom == -1){ /*Aucun salon n'a été trouvé*/

            sending(tabClient[numClient].dSC, "Aucun salon trouvé.\n");

        }else if(idRoom == 0){ /*On ne peut modifier le salon principal*/

            sending(tabClient[numClient].dSC, "Vous ne pouvez pas supprimer le salon général.\n");

        }else if(isOccupiedRoom(idRoom)) { /*Des clients sont présents dans le salon à supprimer*/

            sending(tabClient[numClient].dSC, "Vous ne pouvez pas supprimer un salon occupé.\n");

        }else if(!tabClient[numClient].isAdmin && !rooms[idRoom].admin[numClient]){ /*Seul l'admin du serveur ou de la room peut supprimer le salon*/
        
            sending(tabClient[numClient].dSC, "Vous n'avez pas les droits pour supprimer ce salon.\n");

        }else{ /*Le salon peut être supprimé*/
            
            pthread_mutex_lock(&lock); /*Début d'une section critique*/

            rooms[idRoom].created=0;
            strcpy(rooms[idRoom].descr,"Default");
            /*Réinitialisation des admins à false*/
            int i;
            for(i=0;i<MAX_CLIENT;i++){
                rooms[idRoom].admin[i]=0;
            }

            /*Réinitialisation des ban à false*/
            for(i=0;i<MAX_CLIENT;i++){
                rooms[idRoom].ban[i]=0;
            }

            /*MAJ NOM dans le fichier*/
            updateRoom(1,1,1);

            pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

            sending(tabClient[numClient].dSC, "Le salon a été supprimé.\n"); 
            
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

    /*On récupère le nom du salon à modifier*/
    strtok(msg," "); /*suppression de la commande dans le message*/
    char * roomName = strtok(NULL," "); /*récupération du nom du salon à créer*/

    if(roomName==NULL){
        sending(tabClient[numClient].dSC, "Saisissez un nom de salon existant et un nouveau nom de salon.\n");       
    }else {

        /*On récupère le nouveau nom*/
        char * newName = strtok(NULL,"\0");

        if(newName==NULL){
            sending(tabClient[numClient].dSC, "Saisissez un nouveau nom de salon.\n");     

        }else if(!isAvailableName(newName)){
            sending(tabClient[numClient].dSC, "Un nom de salon ne peut pas contenir d'espace.\n");  
        }else{

            /*ID*/
            int idRoom = getRoomByName(roomName);

            if(idRoom == -1){ /*Aucun salon n'a été trouvé*/

                sending(tabClient[numClient].dSC, "Aucun salon trouvé.\n");

            }else if(idRoom == 0){ /*On ne peut modifier le salon principal*/

                sending(tabClient[numClient].dSC, "Vous ne pouvez pas modifier le salon général.\n");

            }else if(!tabClient[numClient].isAdmin && !rooms[idRoom].admin[numClient]){ /*Seul l'admin du serveur ou de la room peut modifier le salon*/
        
                sending(tabClient[numClient].dSC, "Vous n'avez pas les droits pour modifier ce salon.\n");

        }   else{ /*On peut modifier le salon*/

                pthread_mutex_lock(&lock); /*Début d'une section critique*/

                strcpy(rooms[idRoom].name,newName);
            
                /*MAJ NOM dans le fichier*/
                updateRoom(1,0,0);

                pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

                sending(tabClient[numClient].dSC, "Le nom du salon a été mis à jour.\n"); 
            }
        }
    }
}

void updateDescrRoom(int numClient, char * msg){

    /*On récupère le nom du salon à modifier*/
    strtok(msg," "); /*suppression de la commande dans le message*/
    char * roomName = strtok(NULL," "); /*récupération du nom du salon à créer*/

    if(roomName==NULL){
        sending(tabClient[numClient].dSC, "Saisissez un nom de salon existant et une nouvelle description.\n");       
    }else {

        /*On récupère la nouveau description*/
        char * newDescr = strtok(NULL,"\0");

        if(newDescr==NULL){
            sending(tabClient[numClient].dSC, "Saisissez une nouvelle description.\n");     

        }else{

            /*ID*/
            int idRoom = getRoomByName(roomName);

            if(idRoom == -1){ /*Aucun salon n'a été trouvé*/

                sending(tabClient[numClient].dSC, "Aucun salon trouvé.\n");

            }else if(idRoom == 0){ /*On ne peut modifier le salon principal*/

                sending(tabClient[numClient].dSC, "Vous ne pouvez pas modifier le salon général.\n");

            }else if(!tabClient[numClient].isAdmin && !rooms[idRoom].admin[numClient]){ /*Seul l'admin du serveur ou de la room peut modifier le salon*/
        
                sending(tabClient[numClient].dSC, "Vous n'avez pas les droits pour modifier ce salon.\n");

            }else{ /*On peut modifier le salon*/

                pthread_mutex_lock(&lock); /*Début d'une section critique*/

                strcpy(rooms[idRoom].descr,newDescr);
            
                /*MAJ NOM dans le fichier*/
                updateRoom(1,0,0);

                pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

                sending(tabClient[numClient].dSC, "La description du salon a été mise à jour.\n"); 
            }
        }
    }
}