/*--------------------------------------------------------------*/
/* Les fonctions du jeu de Mastermind                           */
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "fon.h"
#include "mastermind.h"


#define MAXTOUR 11
/**
 * @brief Selection du nombre de jeton de couleur à deviner durant la partie
 * @return n le nombre de jeton de couleur
 */
int SelectionDifficultee(int num_socket, int taille) {
	int n = -1; //initialisation à -1 poru rentrer dans la boucle de gestion d'erreur
	int write, read;
	while (n < 3 || n > 5) {
		char *message = (char *) calloc (taille, sizeof(char));
		write = h_writes(num_socket, "Choisissez le nombre de couleur à deviner en 12 tours (entre 3 - 8): \n",taille);
		read = h_reads(num_socket, message, taille);
		n = atoi(message);
	}
	return n;
}
/**
 * @brief Création du jeu caché de façon aléatoire chaque case
 * contient un entier correspondant à une couleur
 * @param n le nombre de couleur qui compose le tableau (difficulté)
 * @param le tableau de jeu à remplir
 */
void CreationJeuATrouver(int n, int *jeu) {
	int i;
	srand(time(NULL)); //initialisation de rand
	for(i=0; i<n; i++){
		jeu[i] = (rand() % 8) + 1;
	}
}



/**
 * @brief Le joueur rempli sa liste de couleur dans le but de deviner le jeu caché
 * @param listeCouleur le tableau de couleu du joueur
 * @param n le nombre de jeton a deviner
 * @parma tour le numéro de tour de jeu
 */
void JouerUnTour(int *listeCouleur, int n, int tour) {
	int i, j;
	/*Réinitialisation du tableau de couleur du joueur*/
	for(i=tour*n; i<(tour*n + n); i++) {
		while(listeCouleur[i] < 0 || listeCouleur[i] > 8) {
			printf("Couleur de la case numéro %d:\n", i+1);
			scanf("%d",listeCouleur+i);
		}
		printf("-----------------------\n");
		for (j=tour*n; j<=i; j++){
			printf("   %d", listeCouleur[j]);
		}
		printf("\n-----------------------\n");
	}
}

/**
 * @brief Fonction vérifiant si les couleurs entrées par le joueur sont bien placées
 * mal placées, ou non présentes.
 * @param jeu tableau de couleur à deviner
 * @param listeCouleur le tableau de couleur entré par le joueur
 * @param tableauResultats le tableau présentant les résultats obtenus pour chaque tour
 * du jeu. 0 -> la couleur est mal placée, 1 -> la couleur est bien placée, pas de valeur
 * la couleur n'est pas présente.
 * @param tour le numéro du tour actuel (une partie se joue en 12 tours, le premier tour est 0)
 * @param n le nombre de jeton a deviner
 */
 
void VerifierLesCouleurs(int *jeu, int *listeCouleur, int *tableauResultats, int tour, int n) {
	int *copieJeu = malloc(n * sizeof(int));
	int i, j;
	/*Initialisation du tableau de copie du tableau de jeu */
	for (i = 0; i < n; i++) {
		copieJeu[i] = jeu[i];
	}
	
	/*-------------------------------------------------------------*/
	/*Première vérification: il y a-t-il des ints bien placées?*/
	/*-------------------------------------------------------------*/
	/*Pour chaque couleur de la liste des couleurs du joueur
	 *on regarde si celle-ci est présente et bien placée dans
	 *le jeu à trouver */
	j = 0;
	for (i=tour*n; i<(tour*n + n); i++) {
		if (listeCouleur[i] == copieJeu[j]) {
			/*Le tableau des résultats est modifié avec la valeur 1 (trouvé bien placé) */
			tableauResultats[i] = 1;
			/*La couleur trouvée est effacée dans la copie du tableau
 de jeu avant la prochaine vérification */
			copieJeu[j] = -1;
		}
		j++;
	}
	
	/*-------------------------------------------------------------*/
	/*Deuxième vérification: il y a-t-il des couleurs mal placées?*/
	/*-------------------------------------------------------------*/
	/*Pour chaque couleur de la liste des couleurs du joueur
	 *on regarde si celle-ci est présente mais mal placée
	 *dans le jeu à trouver */
	for (i=tour*n; i<(tour*n + n); i++) {
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
 * @brief Affiche sur la console la totalité des résultats depuis
 *le début de la partie
 * @param tour le numéro de tour (le premier tour est le tour 0)
 * @param n le niveau de difficulté (nombre de jeton de couleur à deviner)
 * @param tableauResultats le tableau des résultats
 * @param listeCouleur le tableau des essais de couleur du joueur
 */
void AfficherTableauResultats(int tour, int n, int *tableauResultats, int *listeCouleur) {
	int i, j;
	i=0;
	j=0;
	printf(" Tableau des résultats      Couleurs du Jouées  \n");
	printf("-----------------------  -----------------------\n");
	while (i < (tour+1) * n) {
		if (tableauResultats[i] == 1 || tableauResultats[i] == 0) {
		printf("   %d",tableauResultats[i]);
		}
		else {
			printf("    ");
		}
		i++;
		j++;
		if (j == n) {
			switch(n)
			{
				case 3: 
				printf("            ");
				break;
				case 4: 
				printf("        ");
				break;
				case 5: 
				printf("   ");
				break;
			}
			printf("  ");
			j = i - n;
			while (j<i) {
				printf("   %d",listeCouleur[j]);
				j++;
			}
			j = 0;
			printf("\n-----------------------  -----------------------\n");
		}
	}
}


/**
 * @brief Fonction renvoyant une chaine de caractère avec les couleurs jouées et les résultats
 * obtenus pour ces couleurs. La chaine inclue aussi les coups précédents et leurs résultats
 * @param tour le tour de jeu 
 * @param n le nombre de couleurs à deviner
 * @param tableauResultats le tableau avec les résultats calculés pour les couleurs joué à ce tour
 * et les résultats des tours précedents
 * @param listeCouleur la liste de couleur jouée par le joueur à ce tour ainsi qu'aux tours précédent
 * @return Une chaine de caractère
 */
char* toStringTableauResultats(int tour, int n, int *tableauResultats, int *listeCouleur) {
	char buffer[50];
	int i, j;
	i=0;
	j=0;
	char *res = malloc(500*sizeof(char));
	//res = " Tableau des résultats      Couleurs du Jouées  \n";
	strcat(res,"-----------------------  -----------------------\n");
	while (i < (tour+1) * n) {
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
				case 3: 
				strcat(res,"            ");
				break;
				case 4: 
				strcat(res,"        ");
				break;
				case 5: 
				strcat(res,"   ");
				break;
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
	return res;
}


/**
 * @brief Fonction vérifiant si la partie est gagnée à la fin du tour en cours
 * @param tableauResultats Le tableau des résultats
 * @param n le nombre de couleurs à deviner
 * @param tour le tour en cours
 * @return un booleen vrai si la partie est gagnée, sinon faux
 */
int PartieGagnee(int *tableauResultats, int n, int tour) {
	int i, gagnee;
	gagnee = 1;
	for (i=n*tour; i< (n*tour + n); i++) {
		gagnee =  gagnee && tableauResultats[i] == 1;
	}
	return gagnee;
}

