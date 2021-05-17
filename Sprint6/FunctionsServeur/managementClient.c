#include "managementClient.h"

int giveNumClient(){
    int i = 0;
    int indice = -1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/

    while (i<MAX_CLIENT && indice==-1){
        if(!tabClient[i].created){
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
            printf("%s === %s", pseudo, tabClient[i].pseudo);
            if (strcmp(pseudo, tabClient[i].pseudo)==0){
                client = i;
            }
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

    return client;
}

int isFreePseudo(char * pseudo){
    int i= 0;
    int available = 1;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/
    
    while (i<MAX_CLIENT && available){
        
        if(tabClient[i].created){
            if(strcmp(tabClient[i].pseudo,pseudo)==0){
                available = 0;
            }
        }
        i+=1;
    }
    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
    
    return available;
}

void displayClient(int numClient){

	char * msg = (char *)malloc(sizeof(char)*12*MAX_CLIENT);
	strcpy(msg, "_____ Liste des clients connectés _____ \n");
    int i;

    for(i = 0; i < MAX_CLIENT; i++){
    	
    	/*Si le client i est connecté*/
    	if(tabClient[i].connected){
    		strcat(msg, "-- ");
    		strcat(msg, tabClient[i].pseudo);
    		strcat(msg, "\n");
    	}
    }
    strcat(msg, "_______________________________________ \n");
    sending(tabClient[numClient].dSC, msg);
    free(msg);

	return;
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
        
        printf("Identifiant\n");
        /*ID*/
        tabClient[i].id = atoi(strtok(buffer,","));

        printf("Pseudo\n");
        /*PSEUDO*/
        tabClient[i].pseudo = (char *)malloc(sizeof(char)*15);
        strcpy(tabClient[i].pseudo,strtok(NULL,","));

        printf("password\n");
        /*PASSWORD*/
        tabClient[i].password = (char *)malloc(sizeof(char)*12);
        strcpy(tabClient[i].password,strtok(NULL,","));

        printf("description\n");
        /*DESCRIPTION*/
        tabClient[i].descr = (char *)malloc(sizeof(char)*100);
        strcpy(tabClient[i].descr,strtok(NULL,","));

        printf("connected\n");
        /*CONNECTED*/
        tabClient[i].connected = 0;

        printf("created\n");
        /*CREATED*/
        tabClient[i].created = atoi(strtok(NULL,","));

        printf("admin\n");
        /*ISADMIN*/
        tabClient[i].isAdmin = atoi(strtok(NULL,","));

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

    printf("Fichier créé\n");

    int i;
    for (i = 0; i < MAX_CLIENT; i++){
        char * id = malloc(sizeof(int));
        char * created = malloc(sizeof(int));
        char * isAdmin = malloc(sizeof(int));

        /*ID*/
        printf("Identifiant\n");

        sprintf(id,"%d",tabClient[i].id);
        strcpy(line,id);
        strcat(line,",");

        /*PSEUDO*/
        printf("Pseudo\n");

        strcat(line,tabClient[i].pseudo);
        strcat(line,",");

        /*PASSWORD*/
        printf("Password\n");

        strcat(line,tabClient[i].password);
        strcat(line,",");

        /*DESCRIPTION*/
        printf("Decription\n");

        strcat(line,tabClient[i].descr);
        strcat(line,",");

        /*CREATED*/
        printf("Created\n");
        sprintf(created,"%d",tabClient[i].created);
        strcat(line, created);
        strcat(line, ",");

        /*ISADMIN*/
        printf("isAdmin\n");
        sprintf(isAdmin,"%d",tabClient[i].isAdmin);
        strcat(line, isAdmin);
        strcat(line, "\n\0");
        
        int w = write(fd,line,strlen(line));
        if(w == -1){
            perror("erreur au write\n");
            exit(1);
        }


        free(id);
        free(created);
        free(isAdmin);
    }
    free(line);
    return;
}

void updatePseudo(int numClient, char * msg){
    char * error = (char *)malloc(sizeof(char)*60);

    /*On récupère le nouveau pseudo*/
    char *  newPseudo =  (char *) malloc(sizeof(char)*300);
    strtok(msg," "); /*suppression de la commande dans le message*/
    newPseudo = strtok(NULL,"\0"); /*récupération du nouveau peusdo*/

    if(newPseudo==NULL){
        strcpy(error, "Saisissez un nouveau peusdo.\n");
        sending(tabClient[numClient].dSC, error);       
    }else if(!isAvailableName(newPseudo)){
        strcpy(error, "Un pseudo ne peut pas contenir d'espace.\n");
        sending(tabClient[numClient].dSC, error);  
    }else{ /*On peut modifier le pseudo*/

        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        strcpy(tabClient[numClient].pseudo,newPseudo);
    
        /*MAJ dans le fichier*/
        saveClients();

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/

        /*ToDo : informer les autres clients que le pseudo à changé*/ 
    }
}

void updateDescr(int numClient, char * msg){
    char * error = (char *)malloc(sizeof(char)*60);

    /*On récupère la nouvelle description*/
    char *  newDescr =  (char *) malloc(sizeof(char)*300);
    strtok(msg," "); /*suppression de la commande dans le message*/
    newDescr = strtok(NULL,"\0"); /*récupération du nouveau peusdo*/

    if(newDescr==NULL){
        strcpy(error, "Saisissez une nouvelle description.\n");
        sending(tabClient[numClient].dSC, error);       
    }else{ /*On peut modifier la description*/

        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        strcpy(tabClient[numClient].descr,newDescr);
    
        /*MAJ dans le fichier*/
        saveClients();

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
    }
}

void createAccount(int dSC, char * pseudo, int numClient){
    printf("Création de compte pour le client %d\n",numClient);

    strcpy(tabClient[numClient].pseudo,pseudo);

    printf("Pseudo : %s\n",tabClient[numClient].pseudo);

    char * password = (char *) malloc(sizeof(char)*100);
    receiving(dSC, password, sizeof(char)*15);
    password = strtok(password, "\n");
    strcpy(tabClient[numClient].password,password);

    printf("Password : %s\n",tabClient[numClient].password);

    strcpy(tabClient[numClient].descr,"Default");

    tabClient[numClient].created = 1; 
    tabClient[numClient].dSC = dSC;
    /*A la création du compte le client est connecté et va rejoindre le salon général*/
    tabClient[numClient].connected = 1;

    printf("Fin de la création du compte, client créé : %d, connecté : %d\n",tabClient[numClient].created,tabClient[numClient].connected);

    printf("Sauvegarde dans fichier\n");
    saveClients();
    printf("Fin de la sauvegare\n");

    addMember(numClient,0);

    welcomeMsg(dSC);

    free(password);


    /*ToDo : envoyer un message de bienvenu expliquant les commandes pour modifier le profil*/

}

void connection(int dSC, int numClient){
    /*ToDO: 
        - demander pseudo, 
        - regarder si le pseudo correspond à un client, 
        - si oui demander un mdp et le comparer avec celui du client trouvé
        - si les mdp correspondent enregistrer la socket et ajouter le client au salon général */

    char * password = (char *) malloc(sizeof(char)*100);
    receiving(dSC, password, sizeof(char)*15);
    password = strtok(password, "\n");
    int availablePassword = strcmp(password,tabClient[numClient].password)==0;
    printf("%s == %s, %d\n",password, tabClient[numClient].password, availablePassword );
    sendingInt(dSC,availablePassword);
    while(!availablePassword){
        printf("Mot de passe incorrecte!\n");
        receiving(dSC, password, sizeof(char)*15);
        password = strtok(password, "\n");
        availablePassword = strcmp(password,tabClient[numClient].password)==0;
        sendingInt(dSC,availablePassword);
    }
    /*Mot de passe correct*/
    
    /*On enregistre la socket du client*/
    tabClient[numClient].connected = 1;
    tabClient[numClient].dSC = dSC;

    /*Ajout du client dans le salon général*/
    addMember(numClient,0);

    welcomeMsg(dSC);
}

void deleteAccount(int numClient){
    char * error = (char *)malloc(sizeof(char)*60);

    if(tabClient[numClient].isAdmin){ /*L'admin ne peut pas supprimer son compte avant d'avoir légué son rôle à un autre client*/

        strcpy(error, "Vous êtes l'admin, léguez votre rôle avant de supprimer votre compte.\n");
        sending(tabClient[numClient].dSC, error);

    }else{ /*Le compte peut être supprimé*/
        
        pthread_mutex_lock(&lock); /*Début d'une section critique*/

        tabClient[numClient].created=0;
        strcpy(tabClient[numClient].descr,"Default");

        /*MAJ dans le fichier*/
        saveClients();

        pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
    }
}

void killThread(){
    int i;

    pthread_mutex_lock(&lock); /*Début d'une section critique*/
    
    for (i=0;i<MAX_CLIENT*2;i++){
        if(tabThreadToKill[i]!=((void *)0)){
            pthread_join(tabThreadToKill[i],NULL);
        }
    }
    nbThreadToKill=0;

    pthread_mutex_unlock(&lock); /*Fin d'une section critique*/
}