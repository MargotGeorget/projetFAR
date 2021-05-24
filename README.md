# projetFAR

## Organisation des fichiers 

/projetFAR/Sprint6/

    FileReceived/         //fichiers téléchargés depuis le serveur
    FileServeur/          //fichiers stockés sur le serveur
    FileToSend/           //fichiers dont l'envoi est possible au serveur
    listeFichier.txt      //liste des fichiers disponibles au téléchargement
    serveur.c
    client.c
    serveurCompilation.sh
    clientCompilation.sh
    FunctionsServer/

      adminRoom.txt       //sauvegarde des administrateurs des salons
      ban.txt             //sauvegarde des clients bannis
      clients.txt         //stockage des comptes clients
      rooms.txt           //stockage des salons de discussion
      man.txt             //manuel d'utilisation, rassemble toutes les fonctions disponibles sur le serveur
      library.h           //déclarations des structures et variables globales
      managementClient.h  //fichier header, fonctions de gestion des clients
      managementClient.c
      managementRoom.h    //fichier header, fonctions de gestion des salons de discussion
      managementRoom.c
      dialogFunctions.h   //fichier header, fonctions d'échanges entre le client et le serveur
      dialogFunctions.c
      managementMessage.h //fichier header, fonctions de gestion des messages entre le client et le serveur
      managementMessage.c
      threadFunctions.h   //fichier header, fonctions de gestion des threads clients
      threadFunctions.c

    FunctionsClient/
      library.h           //déclarations des structures et variables globales
      dialogFunctions.h   //fichier header, fonctions d'échanges entre le client et le serveur
      dialogFunctions.c
      managementMessage.h //fichier header, fonctions de gestion des messages entre le client et le serveur
      managementMessage.c
      threadFunctions.h   //fichier header, fonctions de gestion des threads clients
      threadFunctions.c

##Auteurs
* **Margot Georget** 
* **Ophélie Amarian** 
