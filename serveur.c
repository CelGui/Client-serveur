#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>

// Verifie les erreurs
int check_error(int check)
{
        if ( check == -1 )
        {
                perror("[-] Erreur de transfert / réception de données");
                exit(-1);
        }

}

// Transférer un fichier du serveur vers le client
void send_file(char *filename, int acceptserv)
{
	char doc[2048];

	FILE *fp;
	fp=fopen(filename, "r");

	if( fp == NULL )
	{
		printf("[-] Erreur lors de l'ouverture du fichier ... \n");
		return;
	}

	while( fgets(doc, 2048, fp) != NULL )
	{
		int send_file = send(acceptserv, doc, sizeof(doc), 0);
		check_error(send_file);
	}
	bzero(doc, 2048);

	printf("\n[+] Réception de fichiers ... \n");
	printf("[+] Fichier envoyé avec succès \n");
	return;
}

int main()
{
	int socketserv, bindserv, listenserv, len, acceptserv, send_cli, receiv_cli, closing;
	struct sockaddr_in serv_addr, cli_addr;

	char msgCli[1024], msgServ[2048];

	// Création de socket; ajouter une adresse IP, un numéro de port ...
	socketserv = socket(AF_INET, SOCK_STREAM, 0);
	if ( socketserv == -1 )
	{
		perror("[-] Création de socket de serveur infructueuse ");
		exit(-1);
	}
	else
	{
		printf("[+] Socket de serveur créé avec succès \n");
	}

        memset(&(serv_addr.sin_zero), '\0', 8);

	bzero((char *)&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family= AF_INET;
	serv_addr.sin_port = htons(44440);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// Pour associer un socket au port de la machine locale
	bindserv = bind(socketserv, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if ( bindserv == -1 )
        {
                perror("[-] Liaison au port infructueuse  ");
                exit(-1);
        }
        else
        {
                printf("[+] Liaison au port réussie \n");
        }

	// En attente des connexions entrantes
	listenserv = listen(socketserv, 3);
	if ( listenserv == -1 )
        {
                perror("[-] Écoute infructueuse ");
                exit(-1);
        }
        else
        {
                printf("[+] En ecoute... \n");
        }

	// Accept the connection of the client who is connecting with the server
	len = sizeof(cli_addr);
	acceptserv = accept(socketserv, (struct sockaddr *)&cli_addr, &len);
	if ( acceptserv == -1 )
        {
                perror("[-] Connexion au client infructueuse ");
                exit(-1);
        }
        else
        {
                printf("[+] Connexion au client réussie \n");
        }

        //Chat entre le client et le serveur en utilisant send () et recv ()
	bzero(msgServ, 2048);
	strcpy(msgServ, "salut! Comment puis-je vous aider? Voulez-vous: \n 1) Obtenir un document \n 2) Obtenir la liste des documents sur le serveur \n 3) Supprimer un document sur le serveur \n 4) Fermer la connexion \n");
        send_cli = send(acceptserv, msgServ, 2048, 0);
	check_error(send_cli);
	printf("\nSERVER Response:\n%s \n", msgServ);

	int exit_loop = 1;
	while(exit_loop == 1)
	{
		bzero(msgCli, 1024);
		receiv_cli = recv(acceptserv, msgCli, 1024, 0);
		check_error(receiv_cli);
		printf("CLIENT Response:\n%s \n", msgCli);

		//Pour quitter la boucle pour terminer la connexion
		if ( strcmp(msgCli, "4") == 0 )
		{
			exit_loop = 4;
		}
		else
		{
			int x = atoi(msgCli);
			switch(x)
			{
				// Pour obtenir un document specifique sur le serveur
				case 1:
				{
					bzero(msgServ, 2048);
				        strcpy(msgServ, " Quel document avez vous besoin? \n");
					send_cli = send(acceptserv, msgServ, 2048, 0);
                                        check_error(send_cli);
                                        printf("\nRéponse du SERVEUR\n%s \n", msgServ);

					bzero(msgCli, 1024);
                			receiv_cli = recv(acceptserv, msgCli, 1024, 0);
                			check_error(receiv_cli);
		                	printf("Réponse du CLIENT:\n%s \n", msgCli);

					send_file(msgCli, acceptserv);

					break;
				}

				// Avoir les documents disponible sur le serveur
				case 2:
				{
					char list[2048];

        				DIR *d;
       					struct dirent *dir;
        				d = opendir("~/Client-serveur");

				        if (d)
        				{
                				while ((dir = readdir(d)) != NULL)
                				{
                        				printf("%s\n", dir->d_name);
                				}
                				closedir(d);
        				}

					bzero(msgServ, 2048);
                                        strcpy(msgServ, dir->d_name);
                                        send_cli = send(acceptserv, msgServ, 2048, 0);
					check_error(send_cli);
                                        printf("Réponse du SERVER:\\n%s \n", msgServ);
					break;
				}

				// Supprimer un document depuis le serveur
				case 3:
				{
					bzero(msgServ, 2048);
                                        strcpy(msgServ, " Quel document souhaitez-vous supprimer? \n");
                                        send_cli = send(acceptserv, msgServ, 2048, 0);
                                        check_error(send_cli);
                                        printf("\nRéponse du SERVER:\n%s \n", msgServ);

                                        bzero(msgCli, 1024);
                                        receiv_cli = recv(acceptserv, msgCli, 1024, 0);
                                        check_error(receiv_cli);
                                        printf("Réponse du CLIENT\n%s \n", msgCli);

                                        remove(msgCli);

					printf("\n[+] Suppression d'un fichier ... \n");
					printf("\n[+] Fichier effacer avec succès \n");

					break;
				}

				default:
				{
					bzero(msgServ, 2048);
					strcpy(msgServ, " Vous pouvez continuer ou réessayer \n");
					send_cli = send(acceptserv, msgServ, 2048, 0);
					check_error(send_cli);
	     				printf("\nRéponse du SERVEUR:\n%s \n", msgServ);
					break;
				}
			}
		}
	}

	// Pour fermer la connexion entre le client et le serveur
	closing = close(socketserv);
        if ( closing == -1 )
        {
	        perror("[-] Fermer la connexion sans succès ");
	        exit(-1);
	}
	else
        {
        	printf("[+] Déconnexion ... \n");
                printf("[+] Connexion fermée avec succès \n");
        }

	return 0;
}
