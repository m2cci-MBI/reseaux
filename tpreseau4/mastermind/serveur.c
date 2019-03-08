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
#define BUFFER 100

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


void JouerUnePartie(int socket_client);
void CreationTabCouleurs(int n, int jeu[]);
void JouerUnTour(int *listeCouleur, int n, int tour, int socket_client, int nb_couleurs);
void ComparerSolutionUtilisateur(int *jeu, int *listeCouleur, int *tableauResultats, int tour, int n);
int PartieGagnee(int *tableauResultats, int n, int tour);
void conversionTabCouleurs(int tour, int n, int *tableauResultats, int *listeCouleur, int socket_client);
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

	/* Liaison de la socket � un num�ro de port du serveur */
	adr_socket(service, NULL, SOCK_STREAM, &p_adr_serv);
	h_bind(num_socket, p_adr_serv );

	/* mise en �coute du serveur */
	h_listen(num_socket, 10);
void JouerUnePartie(int socket_client);
	while (true) {
		/* Cr�ation d'une nouvelle socket connect�e avec un client */
		socket_client = h_accept(num_socket, &p_adr_client);
		/* Duplication du processus pour un nouveau client */
		p = fork();
		/* processus associ� au processus fils*/
		if (p==0) {
			h_close(num_socket);
			JouerUnePartie(socket_client);
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
void JouerUnePartie(int socket_client) {
	int i;
	int tour = 0; // le premier tour
	int partieGagnee = 0;
	int nb_couleurs;
	int gagne;
	/*-------------------------------------------------------------------*/
	/* S�lection de la difficult� */
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

	/*-------------------------------------------------------------------*/
	/* Cr�ation des tableaux du joueur
	int tableauCouleur[nb_couleurs*MAXTOUR]; //le tableau de couleur du joueur
	int tableauResultats[nb_couleurs*MAXTOUR]; //le tableau de r�sultats des diff�rent tours
	for (i=0; i<nb_couleurs*MAXTOUR; i++) { //initialisation � -1
		tableauCouleur[i] =-1;
		tableauResultats[i] = -1;
	}*/
	int tableauCouleur[nb_couleurs]; //le tableau de couleur du joueur
	int tableauResultats[nb_couleurs]; //le tableau de r�sultats des diff�rent tours
	for (i=0; i<nb_couleurs; i++) { //initialisation � -1
		tableauCouleur[i] =-1;
		tableauResultats[i] = -1;
	/*-------------------------------------------------------------------*/
	/* D�roulement de la partie */
	do {
		JouerUnTour(tableauCouleur,nb_couleurs,tour,socket_client, nb_couleurs);
		ComparerSolutionUtilisateur(jeuATrouver,tableauCouleur,tableauResultats,tour,nb_couleurs);
		conversionTabCouleurs(tour,nb_couleurs,tableauResultats,tableauCouleur, socket_client);
		gagne = PartieGagnee(tableauResultats,nb_couleurs,tour);
		printf("%d\n", gagne);
		tour++;
	} while (tour<MAXTOUR && gagne==0);

	/* Fin de la partie */
	messageFinDePartie(tour,socket_client);
	/*-------------------------------------------------------------------*/
}
}

/**
 * @brief Selection du nombre de jeton de couleur � deviner durant la partie
 * @return n le nombre de jeton de couleur
 */
/*int SelectionDifficultee(int socket_client) {
	int n = -1; //initialisation � -1 pour rentrer dans la boucle de gestion d'erreur
	int lg_msg_envoie, lg_msg_recu;
	char *bufferRecep = (char *) calloc (BUFFER,  sizeof(char));
	char *bufferEmi = (char *) calloc (BUFFER, sizeof(char));
    strcpy(bufferEmi,"Choisissez le nombre de couleur � deviner en 12 tours (entre 3 et 5): \n");
	lg_msg_envoie = h_writes(socket_client, bufferEmi, BUFFER);
	lg_msg_recu = h_reads(socket_client, bufferRecep, BUFFER);
	n = atoi(bufferRecep);
	switch(n)
		{
			case 3:
			strcpy(bufferEmi,"Difficult� facile: 3 jetons � deviner\n 0 indique couleur pr�sente mal plac�, 1 indique couleur bien plac�e\n");
			break;
			case 4:
			strcpy(bufferEmi,"Difficult� normale: 4 jetons � deviner\n 0 indique couleur pr�sente mal plac�, 1 indique couleur bien plac�e\n");
			break;
			case 5:
			strcpy(bufferEmi,"Difficult� difficile: 5 jetons � deviner\n 0 indique couleur pr�sente mal plac�, 1 indique couleur bien plac�e\n");
			break;
		}
		lg_msg_envoie = h_writes(socket_client, bufferEmi, BUFFER);
	return n;
} */


/**
 * @brief Cr�ation du jeu cach� de fa�on al�atoire chaque case
 * contient un entier correspondant � une couleur
 * @param n le nombre de couleur qui compose le tableau (difficult�)
 * @param le tableau de jeu � remplir
 */
void CreationTabCouleurs(int n, int jeu[]) {
	int i;
	srand(time(NULL)); //initialisation de rand
	for(i=0; i<n; i++){
		jeu[i] = (rand() % 6) + 1;
	}
}

/**
 * @brief Le joueur rempli sa liste de couleur dans le but de deviner le jeu cach�
 * @param listeCouleur le tableau de couleurs du joueur
 * @param n le nombre de jetons � deviner
 * @parma tour le num�ro de tour de jeu
 */
void JouerUnTour(int *listeCouleur, int n, int tour, int socket_client, int nb_couleurs) {
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


/**
 * @brief Fonction v�rifiant si les couleurs entr�es par le joueur sont bien plac�es
 * mal plac�es, ou non pr�sentes.
 * @param jeu tableau de couleur � deviner
 * @param listeCouleur le tableau de couleur entr� par le joueur
 * @param tableauResultats le tableau pr�sentant les r�sultats obtenus pour chaque tour
 * du jeu. 0 -> la couleur est mal plac�e, 1 -> la couleur est bien plac�e, pas de valeur
 * la couleur n'est pas pr�sente.
 * @param tour le num�ro du tour actuel (une partie se joue en 12 tours, le premier tour est 0)
 * @param n le nombre de jeton a deviner
 */
void ComparerSolutionUtilisateur(int *jeu, int *listeCouleur, int *tableauResultats, int tour, int n) {
	int *copieJeu = malloc(n * sizeof(int));
	int i, j;
	/*Initialisation du tableau de copie du tableau de jeu */
	for (i = 0; i < n; i++) {
		copieJeu[i] = jeu[i];
	}

	/*-------------------------------------------------------------*/
	/*Premi�re v�rification: il y a-t-il des ints bien plac�es?*/
	/*-------------------------------------------------------------*/
	/*Pour chaque couleur de la liste des couleurs du joueur
	 *on regarde si celle-ci est pr�sente et bien plac�e dans
	 *le jeu � trouver */
	j = 0;
	/*for (i=tour*n; i<(tour*n + n); i++)*/
	for (i=0; i<n; i++) {
		if (listeCouleur[i] == copieJeu[i]) {
			/*Le tableau des r�sultats est modifi� avec la valeur 1 (trouv� bien plac�) */
			tableauResultats[i] = 1;
			/*La couleur trouv�e est effac�e dans la copie du tableau
 de jeu avant la prochaine v�rification */
			copieJeu[j] = -1;
		}
		j++;
	}

	/*-------------------------------------------------------------*/
	/*Deuxi�me v�rification: il y a-t-il des couleurs mal plac�es?*/
	/*-------------------------------------------------------------*/
	/*Pour chaque couleur de la liste des couleurs du joueur
	 *on regarde si celle-ci est pr�sente mais mal plac�e
	 *dans le jeu � trouver */
	/*for (i=tour*n; i<(tour*n + n); i++)*/
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


/**
 * @brief Fonction v�rifiant si la partie est gagn�e � la fin du tour en cours
 * @param tableauResultats Le tableau des r�sultats
 * @param n le nombre de couleurs � deviner
 * @param tour le tour en cours
 * @return un booleen vrai si la partie est gagn�e, sinon faux
 */
int PartieGagnee(int *tableauResultats, int n, int tour) {
	int i, gagnee;
	gagnee = 1;
	/*for (i=n*tour; i< (n*tour + n); i++)*/
	for (i=0; i<n; i++) {
		gagnee =  gagnee && tableauResultats[i] == 1;
	}
	return gagnee;
}


/**
 * @brief Fonction renvoyant une chaine de caract�re avec les couleurs jou�es et les r�sultats
 * obtenus pour ces couleurs. La chaine inclue aussi les coups pr�c�dents et leurs r�sultats
 * @param tour le tour de jeu
 * @param n le nombre de couleurs � deviner
 * @param tableauResultats le tableau avec les r�sultats calcul�s pour les couleurs jou� � ce tour
 * et les r�sultats des tours pr�cedents
 * @param listeCouleur la liste de couleur jou�e par le joueur � ce tour ainsi qu'aux tours pr�c�dent
 * */
void conversionTabCouleurs(int tour, int n, int *tableauResultats, int *listeCouleur, int socket_client) {
	char buffer[50];
	int i, j, lg_msg_envoie;
	i=0;
	j=0;
	char *res = malloc(500*sizeof(char));
	//res = " Tableau des r�sultats      Couleurs du Jou�es  \n";
	strcat(res,"-----------------------  -----------------------\n");
	/*while (i < (tour+1) * n)*/
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
				/*case 3:
				strcat(res,"            ");
				break;
				case 4:*/
				strcat(res,"        ");
			/*	break;
				case 5:
				strcat(res,"   ");
				break;*/
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
/**
 * @brief La fonction qui �crit le message de fin de partie gagn�e ou non
 * @param tour le num�ro du tour de jeu
 */
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
