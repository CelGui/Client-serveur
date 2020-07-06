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

//Verifie les erreurs

int check_error(int check)
{
        if ( check == -1 )
        {
                perror("[-] Erreur de transfert / réception de données");
                exit(-1);
        }

}

// Transférer un fichier du serveur vers le client
void receive_file(char *filename, int  socketCli)
{
	char doc[2048];
	int n, file_content;

	FILE *fp;
	fp=fopen(filename, "w");

	if( fp == NULL )
        {
                printf("[-] Erreur en creant le fichier... \n");
        }

	file_content = recv(socketCli, doc, 2048, 0);
	fprintf(fp, "%s", doc);
	fclose(fp);
	bzero(doc, 2048);

	printf("\n[+] Réception de fichiers ... \n");
	printf("[+] Fichier envoyé avec succès \n");
}

int main()
{
	int socketCli, len_cli, connect_serv, receive_serv, send_serv, closing;

	struct sockaddr_in serv_addr;

	char messageCli[1024], messageServ[2048];

	// Création de socket; ajouter une adresse IP, un numéro de port ...
	socketCli = socket(AF_INET, SOCK_STREAM, 0);
	if ( socketCli == -1 )
        {
                perror("[-1] Création de socket de serveur infructueuse ");
                exit(-1);
        }
        else
        {
                printf("[+] Socket de serveur créé avec succès \n");
        }

	memset(&(serv_addr.sin_zero), '\0', 8);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(44440);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// Pour connecter au serveur
	len_cli = sizeof(serv_addr);
	connect_serv = connect(socketCli, (struct sockaddr *)&serv_addr, len_cli);
	if ( connect_serv == -1 )
        {
                perror("[-] Erreur lors de l'établissement de la connexion ");
                exit(-1);
        }
        else
        {
                printf("[+] Connexion établie avec succès \n");
        }

        //Chat entre le client et le serveur en utilisant send () et recv ()
	int exit_loop = 1;
	while(exit_loop == 1)
	{
		bzero(messageServ, 2048);
		receive_serv = recv(socketCli, messageServ, 2048, 0);
		check_error(receive_serv);
        	printf("\nSERVEUR:\n%s \n", messageServ);

		bzero(messageCli, 1024);
		printf("CLIENT:      ");
		scanf("%s", messageCli);
		send_serv = send(socketCli, messageCli, 1024, 0);
		check_error(send_serv);

		//Pour quitter la boucle pour terminer la connexion
		if ( strcmp(messageCli, "4") == 0 )
		{
			exit_loop = 4;
		}
		else
		{
			int x = atoi(messageCli);
			switch(x)
			{
				// Pour obtenir un document specifique sur le serveur
				case 1:
				{
					bzero(messageServ, 2048);
	                		receive_serv = recv(socketCli, messageServ, 2048, 0);
        	        		check_error(receive_serv);
			           	printf("\nSERVEUR:\n%s \n", messageServ);

                			bzero(messageCli, 1024);
                			printf("CLIENT:     ");
                			scanf("%s", messageCli);
                			send_serv = send(socketCli, messageCli, 1024, 0);
					check_error(send_serv);

					receive_file(messageCli, socketCli);

					strcpy(messageCli, "Le client a bien reçu le fichier\n");
					send_serv = send(socketCli, messageCli, 1024, 0);
                                        check_error(send_serv);

					break;
				}

                                // Obtenir les documents disponible sur le serveur

				case 2:
				{
					bzero(messageServ, 2048);
                                        receive_serv = recv(socketCli, messageServ, 2048, 0);
                                        check_error(receive_serv);
                                        printf("\nSERVER:\n%s \n", messageServ);
					break;
				}

				// Supprimer un document depuis le serveur
				case 3:
				{
					bzero(messageServ, 2048);
                                        receive_serv = recv(socketCli, messageServ, 2048, 0);
                                        check_error(receive_serv);
                                        printf("\nSERVEUR:\n%s \n", messageServ);

                                        bzero(messageCli, 1024);
                                        printf("CLIENT:     ");
                                        scanf("%s", messageCli);
                                        send_serv = send(socketCli, messageCli, 1024, 0);
                                        check_error(send_serv);

					strcpy(messageCli, " Demande client terminée avec succès\n");
                                        send_serv = send(socketCli, messageCli, 1024, 0);
                                        check_error(send_serv);

					printf("\n%s \n",messageCli);

                                        break;
				}

				default:
				{
					bzero(messageCli, 1024);
                                        strcpy(messageCli, " Mauvaise sélection \n");
					printf("\n%s \n", messageCli);
                                        send_serv = send(socketCli, messageCli, 1024, 0);
                                        check_error(send_serv);
                                        break;

				}
			}
		}
	}


	//Pour fermer la connexion entre le client et le serveur
	closing = close(socketCli);
	if ( closing == -1 )
	{
        	perror("[-] Fermer la connexion sans succès ");
                exit(-1);
        }
       	else
       	{
                printf("[+] Déconnexion ...\n");
                printf("[+] Connexion fermée avec succès \n");
	}

	return 0;
}
