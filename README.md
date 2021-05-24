# projetFAR

Messagerie instantanée en langage C 

L'objectif de ce projet est de créer une messagerie instantanée à partir de thread et de socket en langage C. Le projet est composé de deux fichiers principaux, un fichier serveur.c mettant en place tout le serveur et les connexions clients, et un ficher client.c permettant au client de se connecter. Chacun de ses deux fichiers sont dépendants de répertoires FunctionsServeur/ pour le serveur et FunctionsClient/ pour le client, regroupant différents fichiers de code et des headers qui rassemblent toutes les fonctions utlisées par les fichiers main (serveur.c et client.c). La hiérarchie et le détail des répertoires et fichiers sont présentés plus bas. 
La messagerie créée permet au client de communiquer entre eux : à l'intèrieur d'un salon, avec tout les clients connectés ou encore en privée avec un seul client. Il est aussi possible de transférer des fichiers d'un client vers le serveur, les autres clients pourront alors à leur tour télécharger le fichier depuis le serveur vers leur appareil. 
Cette messagerie gère alors des salons (nom, description, administrateur et membres), des comptes clients (pseudo, mot de passe, description et rôle) et le transfert de fichier. 

## Pour commencer 

### Pré-requis 
- Maitriser le terminal de commande
- Comprendre la logique du langage C 
- Connaitre les bases des threads et des sockets en C

### Installation 
Il vous faudra dabord cloner le projet à partir de GitHub, commande à exécuter sur votre terminal : git clone https://github.com/MargotGeorget/projetFAR/
Ensuite vous aurez accés à l'ensemble des répertoires et fichiers. 

### Compilation 
Pour compiler les fichiers de ce projet, on vous a facilité le travail grâce à un petit script bash que vous n'aurez qu'à exécuter. 
Les scripts sont les suivants :
- serveurCompilation.sh pour compiler les fichiers serveurs. Commande à exécuter : ./serveurCompilation.sh 
- clientCompilation.sh pour compiler les fichiers clients. Commande à exécuter : ./clientCompilation.sh 

### Lancement du programme 
Maintenant que les fichiers sont bien compilés, vous allez pouvoir lancer l’application. 

Le serveur se lance en utilisant la commande suivante: ./serveur [votre_port] 
Le client se lance avec le même numéro de port (ex: 8000) que celui renseigné pour le serveur: ./client [votre_ip] [votre_port] 
Si vous lancez l’application sur une même machine, l’adresse IP est alors votre adresse locale : 127.0.0.1. 
Si vous lancez le serveur et le client sur 2 machines différentes, renseignez l’adresse IP de l’ordinateur ayant exécuté le serveur.  
Pour connecter plusieurs clients, ouvrez simplement de nouveaux terminaux de commande et renseignez à nouveau la commande permettant d’exécuter le client. 

## Fonctionnalités 

### Liste des commandes 

#### Commandes générales 
/man = afficher l'ensemble des commandes disponibles sur le serveur
/admin = afficher la liste des clients administrateurs du serveur
/end = quitter le serveur

#### Commandes relatives aux fichiers
/upload = afficher la liste des fichiers disponibles à l'envoi et choisir un fichier à envoyer au serveur.
/download = afficher la liste des fichiers disponibles au téléchargement et choisir un fichier à recevoir du serveur.

#### Commandes relatives aux clients 
/whoishere = afficher la liste des utilisateurs actuellement connectés au serveur.
/pseudo [newPseudo] = changer son pseudo.
@all [message] = envoi du message à tous les clients connectés sur le serveur. 
@pseudo [message] = envoi du message au client dont on a renseigné le pseudo.
/profil [pseudo] = afficher le profil d'un client (pseudo, rôle et description)
/deleteAccount = supprimer son compte
/password [oldPassword] [newPassword] = changer le mot de passe du client

#### Commandes relatives aux salons
/rooms = afficher l'ensemble des salons de discussions créés ainsi que leurs participants.
/join [nameRoom] = rejoindre un salon de discussion dont le nom est passé en paramètre. 
/create [nameRoom] = créer un salon de discussion dont le nom est passé en paramètre. 
/remove [nameRoom] = supprimer un salon de discussion dont le nom est passé en paramètre. 
/name [nameRoom] [newNameRoom] = changer le nom du salon.
/descrRoom [descRoom] [newDescRoom] = changer la description du salon.

#### Commandes administrateur serveur et salon 

/ban [pseudo] [nameRoom] = renvoyer le client dans le salon général et l'empêcher de revenir dans le salon dans lequel il était
/unban [pseudo] [nameRoom] = redonner les autorisations au client pour rejoindre le salon
/rightRoom [pseudo] [nameRoom] = donner le rôle administrateur d'un salon à un client

#### Commandes adminintrateurs serveur uniquement
/move [pseudo] [nameRoom] = déplacer le client demandé dans la room donnée
/rightServer [pseudo] [nameRoom] = donner le rôle administrateur du serveur au client

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

## Auteurs
* **Margot Georget** 
* **Ophélie Amarine** 
