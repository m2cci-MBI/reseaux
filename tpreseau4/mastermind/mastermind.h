/********************************************************************************/
/*	Prototypage des fonctions sur le je de MasterMind ( pour mastermind.c ).*/
/********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "fon.h"

#define AFFICHER_JEU
#define MAXTOUR 11 // le nombre maximum de tour pour une partie
#define BUFFER 1000 //la taille des buffers de réception et d'émission

/********************************************************************************/
/*                              Les fonctions de jeu                            */
/********************************************************************************/

int SelectionDifficultee(int sock_client);

void CreationJeuATrouver(int n, int *jeu);

void JouerUnTour(int *listeCouleur, int n, int tour, int sock_client) ;

void VerifierLesCouleurs(int *jeu, int *listeCouleur, int *tableauResultats, int tour, int n) ;

//void AfficherTableauResultats(int tour, int n, int *tableauResultats, int *listeCouleur);

int PartieGagnee(int *tableauResultats, int n, int tour);

void toStringTableauResultats(int tour, int n, int *tableauResultats, int *listeCouleur, int sock_client) ;

void messageFinDePartie (int tour,int sock_client);

void JouerUnePartie (int socket_client);


