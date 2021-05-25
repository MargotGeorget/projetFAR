#include "managementClient.h"

int giveNumClient(){
    int i = 0;
    int indice = -1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<MAX_CLIENT && indice==-1){
        if(!tabClient[i].created){ /*On cherche un emplacement où il n'y a pas de client créé*/
            indice = i;
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return indice;
}

int findClient(char * pseudo){
    int i = 0;
    int client = -1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<MAX_CLIENT && client==-1){
        if (tabClient[i].created){
            if (strcmp(pseudo, tabClient[i].pseudo)==0){ 
                client = i;
            }
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return client;
}

void displayClient(int numClient){

	char * msg = (char *)malloc(sizeof(char)*15*MAX_CLIENT);
	strcpy(msg, "_____ Liste des clients connectés _____ \n");
    int i;

    for(i = 0; i < MAX_CLIENT; i++){
    	
    	/*Si le client i est connecté, on l'affiche dans la liste*/
    	if(tabClient[i].connected){
    		strcat(msg, "-- ");
    		strcat(msg, tabClient[i].pseudo);
    		strcat(msg, "\n");
    	}
    }
    strcat(msg, "_______________________________________ \n\n");
    sending(tabClient[numClient].dSC, msg);

    free(msg);

	return;
}

void displayAdmin(int numClient){

	char * msg = (char *)malloc(100+sizeof(char)*15*MAX_CLIENT);
	strcpy(msg, "_____ Liste des administrateurs du serveur _____ \n");
    int i;

    for(i = 0; i < MAX_CLIENT; i++){
    	
    	/*Si le client i est administrateur, on l'affiche dans la liste*/
    	if(tabClient[i].isAdmin){
    		strcat(msg, "-- ");
    		strcat(msg, tabClient[i].pseudo);
    		strcat(msg, "\n");
    	}
    }
    strcat(msg, "_______________________________________ \n\n");
    sending(tabClient[numClient].dSC, msg);
    free(msg);

	return;
}

void displayOneClient(int numClient, char * msg){

    /*On récupère le pseudo du client à afficher*/
    strtok(msg," "); /*suppression de la commande dans le message*/
    char * hisPseudo = strtok(NULL,"\0"); /*récupération du pseudo*/

    if(hisPseudo==NULL){ /*Aucun pseudo saisi*/
        sending(tabClient[numClient].dSC, "Saisissez le pseudo d'un client.\n");  

    }else{
        int client = findClient(hisPseudo);

        if(client==-1){ /*Aucun client trouvé avec le pseudo saisi*/
            sending(tabClient[numClient].dSC, "Aucun client ne correspond à ce pseudo.\n");
        
        }else{ /*On peut afficher le profil du client*/

            char *  profil =  (char *) malloc(sizeof(char)*300);
            strcpy(profil,"\n** ");

            pthread_mutex_lock(&lock); /*Début d'une section critique*/
            
            strcat(profil,tabClient[client].pseudo);
            strcat(profil," **\n-- ");
            strcat(profil,tabClient[client].descr);
            strcat(profil," --\n");

            if(tabClient[client].isAdmin){
                strcat(profil,"~~ Administrateur du serveur ~~\n");
            }

            int i;
            for (i=1;i<NB_ROOMS;i++){
                if(rooms[i].admin[client]){
                    strcat(profil,"~~ Administrateur du salon : ");
                    strcat(profil,rooms[i].name);
                    strcat(profil," ~~\n");
                }
            }
            strcat(profil,"\n");

            pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

            sending(tabClient[numClient].dSC, profil); 
            free(profil);
        }
    }
}

void initClients(){
    int i = 0;
    char buffer[100] = "";

    /*Ouverture du fichier contenant la dernière sauvegarde des salons*/
    FILE *fp = fopen("FunctionsServeur/clients.txt","r");
    if(fp==NULL){
        perror("error\n");
    }

    pthread_mutex_lock(&lock); /*Début d'une section critique*/
    
    /*Initialisation du tableau des salons à partir du contenu du fichier*/
    while(fgets(buffer,100, (FILE *) fp)!=NULL && i<MAX_CLIENT){
        
        /*ID*/
        tabClient[i].id = atoi(strtok(buffer,","));

        /*PSEUDO*/
        strcpy(tabClient[i].pseudo,strtok(NULL,","));

        /*PASSWORD*/
        strcpy(tabClient[i].password,strtok(NULL,","));

        /*DESCRIPTION*/
        strcpy(tabClient[i].descr,strtok(NULL,","));

        /*CONNECTED*/
        tabClient[i].connected = 0;

        /*CREATED*/
        tabClient[i].created = atoi(strtok(NULL,","));

        /*ISADMIN*/
        tabClient[i].isAdmin = atoi(strtok(NULL,","));

        i++;
    }
    while(i<MAX_CLIENT){ /*Si le nombre de client à augmenté mais que le fichier ne contient pas les informations, 
        initialisation de client par défault : ne seront pas réélement créé tant qu'il n'y aura pas eu d'inscription*/

        /*ID*/
        tabClient[i].id = i;

        /*PSEUDO*/
        strcpy(tabClient[i].pseudo,"Default");

        /*PASSWORD*/
        strcpy(tabClient[i].password,"Default");

        /*DESCRIPTION*/
        strcpy(tabClient[i].descr,"Default");

        /*CONNECTED*/
        tabClient[i].connected = 0;

        /*CREATED*/
        tabClient[i].created = 0;

        /*ISADMIN*/
        tabClient[i].isAdmin = 0;

        i++;

    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    fclose(fp);

    printf("\n-- Clients initialisés --\n");
}

void saveClients(){
    char * line = (char *)malloc(sizeof(char)*200);
    
    /*Suppression fichier*/
    int ret = remove("./FunctionsServeur/clients.txt");
    if (ret == -1){
        perror("erreur suppression \n");
        exit(1);
    }

    /*Création du fichier*/
    int fd = open("./FunctionsServeur/clients.txt", O_CREAT | O_WRONLY, 0666);
    if (fd == -1){
        perror("erreur création saveClients: \n");
        exit(1);
    }

    /*Sauvegarde de toutes les données*/

    int i;
    for (i = 0; i < MAX_CLIENT; i++){
        char id[2];
        char created [2];
        char isAdmin [2];

        /*ID*/
        sprintf(id,"%d",tabClient[i].id);
        strcpy(line,id);
        strcat(line,",");

        /*PSEUDO*/
        strcat(line,tabClient[i].pseudo);
        strcat(line,",");

        /*PASSWORD*/
        strcat(line,tabClient[i].password);
        strcat(line,",");

        /*DESCRIPTION*/
        strcat(line,tabClient[i].descr);
        strcat(line,",");

        /*CREATED*/
        sprintf(created,"%d",tabClient[i].created);
        strcat(line, created);
        strcat(line, ",");

        /*ISADMIN*/
        sprintf(isAdmin,"%d",tabClient[i].isAdmin);
        strcat(line, isAdmin);
        strcat(line, "\n\0");
        
        int w = write(fd,line,strlen(line));
        if(w == -1){
            perror("erreur au write\n");
            exit(1);
        }
    }

    printf("\n-- Fichier clients mis à jour --\n");
    free(line);
    return;
}

void updatePseudo(int numClient, char * msg){

    /*On récupère le nouveau pseudo*/
    strtok(msg," "); /*suppression de la commande dans le message*/
    char * newPseudo = strtok(NULL,"\0"); /*récupération du nouveau pseudo*/

    if(newPseudo==NULL){ /*Aucun nouveau pseudo saisi*/
        sending(tabClient[numClient].dSC, "Saisissez un nouveau pseudo.\n\n");       
    
    }else if(!isAvailableName(newPseudo)){ /*Pseudo invalide saisi*/
        sending(tabClient[numClient].dSC, "Un pseudo ne peut pas contenir d'espace.\n\n");  
    
    }else{ /*On peut modifier le pseudo*/

        /*On informe les autres clients*/
        char * info = (char *)malloc(sizeof(char)*60);
        strcpy(info,"** est désormais ");
        strcat(info,newPseudo);
        strcat(info," **");
        
        sendingRoom(numClient,info);

        /*Modification*/
        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        strcpy(tabClient[numClient].pseudo,newPseudo);
    
        /*MAJ dans le fichier*/
        saveClients();

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

        sending(tabClient[numClient].dSC, "Votre pseudo a été mis à jour.\n\n");  

    }
}

void updateDescr(int numClient, char * msg){

    /*On récupère la nouvelle description*/
    strtok(msg," "); /*suppression de la commande dans le message*/
    char * newDescr = strtok(NULL,"\0"); /*récupération du nouveau peusdo*/

    if(newDescr==NULL){ /*Aucune nouvelle description saisie*/
        sending(tabClient[numClient].dSC, "Saisissez une nouvelle description.\n\n");       
    
    }else{ /*On peut modifier la description*/

        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        strcpy(tabClient[numClient].descr,newDescr);
    
        /*MAJ dans le fichier*/
        saveClients();

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

        sending(tabClient[numClient].dSC, "Votre description a été mise à jour.\n\n"); 
    }
}

void updatePassword(int numClient, char * msg){

    /*On récupère l'ancien mot de passe*/
    strtok(msg," "); /*suppression de la commande dans le message*/
    char * oldPassword = strtok(NULL," "); /*récupération du mot de passe*/

    if(oldPassword==NULL){ /*Aucun ancien mot de passe saisie*/
        sending(tabClient[numClient].dSC, "Saisissez votre ancien mot de passe.\n\n");       
    
    }else{ 

        char * newPassword = strtok(NULL,"\0"); /*récupération du nouveau mot de passe*/

        if(newPassword==NULL){ /*Aucun nouveau mot de passe saisi*/
            sending(tabClient[numClient].dSC, "Saisissez un nouveau mot de passe.\n\n");       
        
        }if(strcmp(oldPassword,tabClient[numClient].password)!=0){ /*Ancien mot de passe incorrect*/
            sending(tabClient[numClient].dSC, "Mot de passe incorrect\n\n");              
        
        }else{ /*On peut modifier le mot de passe*/

            pthread_mutex_lock(&lock); /*Début d'une section critique*/

            strcpy(tabClient[numClient].password,newPassword);
    
            /*MAJ dans le fichier*/
            saveClients();

            pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

            sending(tabClient[numClient].dSC, "Votre mot de passe a été mis à jour.\n\n");
        }

    }
}

int createAccount(int dSC, char * pseudo, int numClient){

    strcpy(tabClient[numClient].pseudo,pseudo);

    sending(dSC,"Aucun compte client n'a été trouvé, inscrivez vous!\nEnregistrez votre mot de passe : \n");

    /*Reception du mot de passe*/
    char * password = (char *) malloc(sizeof(char)*20);
    receiving(dSC, password, sizeof(char)*20);

    if(strcmp(password,"cancel")==0){ /*Si le client saisie 'cancel' on retourne au poitn d'avant*/
        free(password);
        return 0;
    }

    /*Enregistrement du client*/

    strcpy(tabClient[numClient].password,password);

    printf("-- Password : %s\n",tabClient[numClient].password);

    strcpy(tabClient[numClient].descr,"Default");
    tabClient[numClient].created = 1; 
    tabClient[numClient].dSC = dSC;

    /*A la création du compte le client est connecté et va rejoindre le salon général*/
    tabClient[numClient].connected = 1;
    saveClients();
    addMember(numClient,0);
    welcomeMsg(dSC);

    free(password);
    return 1;

}

int connection(int dSC, int numClient){

    sending(dSC,"\nUn compte client a été trouvé, connectez vous!\nSaissisez votre mot de passe : \n");

    /*Reception du mot de passe*/
    char * password = (char *) malloc(sizeof(char)*20);
    receiving(dSC, password, sizeof(char)*20);

    int availablePassword = strcmp(password,tabClient[numClient].password)==0;
    int cancel = strcmp(password,"cancel")==0;

    /*Saisie du mot de passe tant qu'il n'est pas correct ou que le client n'as pas saisie 'cancel'*/
    while(!availablePassword && !cancel){
        printf("Mot de passe incorrect!\n");
        sending(dSC,"Mot de passe incorrect!\nSaissisez votre mot de passe : \n");
        receiving(dSC, password, sizeof(char)*15);
        availablePassword = strcmp(password,tabClient[numClient].password)==0;
        cancel = strcmp(password,"cancel")==0;
    }

    if(cancel){ /*Le client souhaite annuler, on retourne à la saisie du pseudo*/
        free(password);
        return 0;
    }

    /*Mot de passe correct*/
    printf("Mot de passe correct!\n");

    /*On enregistre la socket du client*/
    tabClient[numClient].connected = 1;
    tabClient[numClient].dSC = dSC;

    /*Ajout du client dans le salon général*/
    addMember(numClient,0);
    welcomeMsg(dSC);

    free(password);
    return 1;
}

int nbAdmin(){
    int i = 0;
    int nb = 0;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    for(i=0;i<MAX_CLIENT;i++){
        if (tabClient[i].isAdmin){ 
            nb+=1;
        }
    }

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return nb;
}

int deleteAccount(int numClient){

    if(tabClient[numClient].isAdmin && nbAdmin()==1){ /*L'admin ne peut pas supprimer son compte avant d'avoir légué son rôle à un autre client*/

        sending(tabClient[numClient].dSC, "Vous êtes le seul administrateur du serveur, léguez votre rôle avant de supprimer votre compte.\n\n");
        return 0;

    }else{ /*Le compte peut être supprimé*/

        /*Message de confirmation*/
        sending(tabClient[numClient].dSC, "Etes vous sur de vouloir supprimer votre compte? y/n \n");
        char * rep = (char *)malloc(SIZE_MSG);
        receiving(tabClient[numClient].dSC,rep,sizeof(char)*60);

        if(strcmp(rep,"y")==0){ /*Suppresion du compte*/

            pthread_mutex_lock(&lock); /*Début d'une section critique*/

            sending(tabClient[numClient].dSC,"/end");

            tabClient[numClient].created=0;
            tabClient[numClient].isAdmin=0;
            strcpy(tabClient[numClient].descr,"Default");

            /*MAJ dans le fichier*/
            saveClients();

            pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
            return 1;

        }else{ /*LE client ne veut pas supprimer son compte, annulation*/

            sending(tabClient[numClient].dSC, "Votre compte n'a pas été supprimé.\n\n");
            return 0;
        }

        free(rep);

    }
}

int findClientBySocket(int dS){
    int i = 0;
    int client = -1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<MAX_CLIENT && client==-1){
        if (tabClient[i].connected){
            if (dS == tabClient[i].dSC){
                client = i;
            }
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return client;
}

void giveRightServer(int numClient, char * msg){

    strtok(msg," "); /*suppression de la commande dans le message*/
    char *  pseudo = strtok(NULL," "); /*récupération du pseudo du client pour lui donner les droits*/

    if(pseudo==NULL){ /*Aucun pseudo saisie*/
        sending(tabClient[numClient].dSC, "Saisissez le pseudo d'un client.\n\n");       
    
    }else{

        int client = findClient(pseudo); 
        if(client==-1){ /*Aucun client trouvé*/ 
            sending(tabClient[numClient].dSC, "Aucun client trouvé.\n\n"); 
        
        }else if(!tabClient[numClient].isAdmin){ /*Le client n'a pas les droits*/ 
            sending(tabClient[numClient].dSC, "Vous n'avez pas les droits pour donner les droits du server\n.\n"); 
        
        }else{ /*Un salon et un client ont été trouvés et le client à les droits*/
            
            tabClient[client].isAdmin=1;

            /*MAJ dans le fichier*/
            saveClients();
            sending(tabClient[numClient].dSC, "Les droits du serveur ont été transmis au client.\n\n"); 

            if(tabClient[client].connected){
                sending(tabClient[client].dSC, "Vous avez été déclaré administrateur du serveur.\n\n"); 
            }
        }
    }

    return;
}

void closingClient(int dS){
    int numClient = findClientBySocket(dS);

    /*On retire le client de son salon*/
    deleteMember(numClient,tabClient[numClient].idRoom);

    pthread_mutex_lock(&lock);
    tabClient[numClient].connected=0;

    /*On tue les threads du client*/
    tabThreadToKill[nbThreadToKill]=tabThread[numClient];
    nbThreadToKill+=1;
    tabThread[numClient] = ((pthread_t)0);

    /*Fermeture de la socket*/
    close(tabClient[numClient].dSC);

    pthread_mutex_unlock(&lock);
    sem_post(&semNbClient); /*Diminution du semaphore*/
}

void killThread(){
    int i;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/
    
    for (i=0;i<MAX_CLIENT*2;i++){
        if(tabThreadToKill[i]!=((pthread_t)0)){
            pthread_join(tabThreadToKill[i],NULL);
        }
    }
    nbThreadToKill=0;

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
}