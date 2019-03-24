/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/

#include<stdio.h>
#include <curses.h>
#include<string.h>
#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<time.h>
#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define MAXTOUR 10
#define BUFFER 1024

void serveur_appli (char *service);   /* programme serveur */


/******************************************************************************/
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{

	char *service= SERVICE_DEFAUT; /* numero de service par defaut */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  		  break;
 	case 2:
		  service=argv[1];
            break;

   	default :
		  printf("Usage:serveur service (nom ou port) \n");
		  exit(1);
 	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/

	serveur_appli(service);
}


void RoundDuJeu(int socket_client);
void CreationTabCouleurs(int n, int jeu[]);
void FaireUnTour(int *listeCouleur, int n, int tour, int socket_client, int nb_couleurs);
void TesterSolutionClient(int *jeu, int *listeCouleur, int *tableauResultats, int tour, int n);
int PartieGagnee(int *tableauResultats, int n, int tour);
void conversionCouleursClient(int tour, int n, int *tableauResultats, int *listeCouleur, int socket_client);
void messageFinDePartie (int tour, int socket_client);
/******************************************************************************/
void serveur_appli(char *service) {
	/* Cr�ation des variables syst�me */
	pid_t p;


/* Cr�ation de la socket serveur */
	int num_socket;
	int socket_client;
	struct sockaddr_in *p_adr_serv;
	struct sockaddr_in p_adr_client;
	num_socket = h_socket(AF_INET,SOCK_STREAM);
	char *bufferEmi = (char *) calloc (BUFFER, sizeof(char));
	/*----------------------Mise en �coute du serveur --------------------*/


	adr_socket(service, "127.0.0.1", SOCK_STREAM, &p_adr_serv);
	h_bind(num_socket, p_adr_serv );


	h_listen(num_socket, 10);
void RoundDuJeu(int socket_client);
	while (true) {

		socket_client = h_accept(num_socket, &p_adr_client);

		p = fork();

		if (p==0) {
			h_close(num_socket);
			RoundDuJeu(socket_client);
			h_close(socket_client);
			exit(0);
		} else {
			h_close(socket_client);
		}

	}
}

/*---------------------------------------------------------------------*/
/*----------------------- D�marrage du jeu de Mastermind --------------*/
/*---------------------------------------------------------------------*/
void RoundDuJeu(int socket_client) {
	int i;
	int tour = 0; // le premier tour
	int partieGagnee = 0;
	int nb_couleurs;
	int gagne;
	/*-------------------------------------------------------------------*/

	nb_couleurs = 4; /*SelectionDifficultee(socket_client);*/
	int lg_msg_envoie, lg_msg_recu;
	char *bufferRecep = (char *) calloc (BUFFER,  sizeof(char));
	char *bufferEmi = (char *) calloc (BUFFER, sizeof(char));
	strcpy(bufferEmi,"Bonjour et bienvenu(s) au jeu MasterMind de Marco&Sara!\n Vous avez 10 tours pour deviner une série de 4 couleurs.\nAprès soumission de votre séquence, dans la réponse du serveur 0 indique couleur presente mal placee, 1 indique couleur bien placee\n");

	lg_msg_envoie = h_writes(socket_client, bufferEmi, BUFFER);
	/*-------------------------------------------------------------------*/
	/* Cr�ation du jeu de couleur � deviner */
	int jeuATrouver[nb_couleurs];
	void CreationTabCouleurs(int n, int jeu[]);
	CreationTabCouleurs(nb_couleurs, jeuATrouver);
		/* Affichage du jeu � trouver */
#ifdef AFFICHER_JEU
	printf("-----------------------\n");
	for	(i = 0; i<nb_couleurs; i++){
		printf("  %d",jeuATrouver[i]);
	}
#endif


	int tableauCouleur[nb_couleurs]; //le tableau de couleur du joueur
	int tableauResultats[nb_couleurs]; //le tableau de r�sultats des diff�rent tours
	for (i=0; i<nb_couleurs; i++) { //initialisation � -1
		tableauCouleur[i] =-1;
		tableauResultats[i] = -1;
	/*-------------------------------------------------------------------*/
	/* D�roulement de la partie */
	do {
		FaireUnTour(tableauCouleur,nb_couleurs,tour,socket_client, nb_couleurs);
		TesterSolutionClient(jeuATrouver,tableauCouleur,tableauResultats,tour,nb_couleurs);
		conversionCouleursClient(tour,nb_couleurs,tableauResultats,tableauCouleur, socket_client);
		gagne = PartieGagnee(tableauResultats,nb_couleurs,tour);
		printf("%d\n", gagne);
		tour++;
	} while (tour<MAXTOUR && gagne==0);

	/* Fin de la partie */
	messageFinDePartie(tour,socket_client);
	/*-------------------------------------------------------------------*/
}
}


void CreationTabCouleurs(int n, int jeu[]) {
	int i;
	srand(time(NULL)); //initialisation de rand
	for(i=0; i<n; i++){
		jeu[i] = (rand() % 6) + 1;
	}
}


void FaireUnTour(int *listeCouleur, int n, int tour, int socket_client, int nb_couleurs) {
	int i, j;
	int lg_msg_envoie, lg_msg_recu;
	char buffer[50];
	/* R�initialisation du tableau de couleur du joueur */
	for(i=0; i<nb_couleurs; i++) {
		char *bufferRecep = (char *) calloc (BUFFER,  sizeof(char));
		char *bufferEmi = (char *) calloc (BUFFER, sizeof(char));

		/* S�lection de la couleur par le client */
		strcpy(bufferEmi,"Couleur de la case: (1: bleu, 2: rouge, 3: vert, 4: jaune, 5: orange, 6: marron)\n");
		lg_msg_envoie = h_writes(socket_client, bufferEmi, BUFFER);
		/* Lecture de la couleur selectionn�e */
		lg_msg_recu = h_reads(socket_client, bufferRecep, BUFFER);
		listeCouleur[i] = atoi(bufferRecep);

		/* Affichage des couleurs s�lectionn�es */

		strcpy(bufferEmi,"-----------------------\n");
		for (j=0; j<=i; j++){
			sprintf(buffer,"   %d", listeCouleur[j]);
			strcat(bufferEmi,buffer);
		}
		strcat(bufferEmi,"\n-----------------------\n");
		lg_msg_envoie = h_writes(socket_client, bufferEmi, BUFFER);
	}
}



void TesterSolutionClient(int *jeu, int *listeCouleur, int *tableauResultats, int tour, int n) {
	int *copieJeu = malloc(n * sizeof(int));
	int i, j;

	for (i = 0; i < n; i++) {
		copieJeu[i] = jeu[i];
	}


	j = 0;

	for (i=0; i<n; i++) {
		if (listeCouleur[i] == copieJeu[i]) {

			tableauResultats[i] = 1;

			copieJeu[j] = -1;
		}
		j++;
	}


	for (i=0; i<n; i++) {
		j = 0; //initialisation de la variable de recherche
		while (j<n && listeCouleur[i] != copieJeu[j]) {
			j++;
		}
		if(j != n && tableauResultats[i] != 1) {
			copieJeu[j] = -1;
			tableauResultats[i] = 0;
		}
	}
}



int PartieGagnee(int *tableauResultats, int n, int tour) {
	int i, gagnee;
	gagnee = 1;

	for (i=0; i<n; i++) {
		gagnee =  gagnee && tableauResultats[i] == 1;
	}
	return gagnee;
}

leur jou�e par le joueur � ce tour ainsi qu'aux tours pr�c�dent
 * */
void conversionCouleursClient(int tour, int n, int *tableauResultats, int *listeCouleur, int socket_client) {
	char buffer[50];
	int i, j, lg_msg_envoie;
	i=0;
	j=0;
	char *res = malloc(500*sizeof(char));

	strcat(res,"-----------------------  -----------------------\n");

	while (i < n) {
		if (tableauResultats[i] == 1 || tableauResultats[i] == 0) {
		strcat(res,"   ");
		sprintf(buffer, "%d",tableauResultats[i]);
		strcat(res,buffer);
		}
		else {
			strcat(res,"    ");
		}
		i++;
		j++;
		if (j == n) {
			switch(n)
			{
						strcat(res,"        ");
					}
			strcat(res,"  ");
			j = i - n;
			while (j<i) {
				strcat(res,"   ");
				sprintf(buffer, "%d",listeCouleur[j]);
				strcat(res,buffer);;
				j++;
			}
			j = 0;
			strcat(res,"\n-----------------------  -----------------------\n");
		}
	}
	lg_msg_envoie = h_writes(socket_client, res, BUFFER);
}

void messageFinDePartie (int tour, int socket_client) {
	char *bufferEmi = calloc(BUFFER,sizeof(char));
	int lg_msg_envoie;
	if(tour==MAXTOUR) {
		lg_msg_envoie = h_writes(socket_client, "Partie perdue, dommage! Vous etiez proche!", BUFFER);
	}
	else {
		lg_msg_envoie = h_writes(socket_client, "Bravo! Partie gagnee!", BUFFER);
	}
}

/******************************************************************************/
