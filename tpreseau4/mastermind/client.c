/******************************************************************************/
/*			Application: ...					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/


#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include "fon.h"   		/* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"
#define BUFFER 1024

void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  break;
    default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		  exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */

	client_appli(serveur,service);
}

/*****************************************************************************/
void client_appli (char *serveur,char *service)

/* procedure correspondant au traitement du client de votre application */

{

/*
---------------------------------- Connection au serveur ---------------------
*/
//1. Je cree une socket
struct sockaddr_in  *p_adr_distant;
int *namelent;
int socket_no;
int lg_msg_envoie, lg_msg_recu; // longueur des messages effectivement envoy�s et lus
char *bufferRecep = ( char * ) calloc ( BUFFER, sizeof (char) ) ;
char *bufferEmi = ( char * ) calloc ( BUFFER, sizeof (char) ) ;

int n;

//2. je definis sockaddr_in
adr_socket(service, serveur, SOCK_STREAM, &p_adr_distant);
socket_no = h_socket(AF_INET, SOCK_STREAM);

//3.Connection au serveur
h_connect(socket_no, p_adr_distant);
/*------------------------------------------------------------------------------*/

lg_msg_recu = h_reads(socket_no, bufferRecep, BUFFER);
printf("%s",bufferRecep);

while (strcmp(bufferRecep,"Partie perdue, dommage! Vous etiez proche!")!=0 &&
		strcmp(bufferRecep,"Bravo! Partie gagnee!")!=0) {
			lg_msg_recu = h_reads(socket_no, bufferRecep, BUFFER);
			if (strcmp(bufferRecep,"Couleur de la case: (1: bleu, 2: rouge, 3: vert, 4: jaune, 5: orange, 6: marron)\n")==0) {
				do {
					printf("%s\n",bufferRecep);
					scanf("%s",bufferEmi);
					n = atoi(bufferEmi);
				} while (n<0 || n>= 6);
				lg_msg_envoie = h_writes(socket_no,bufferEmi,BUFFER);
			} else {
				printf("%s\n",bufferRecep);
			}
		}
}
