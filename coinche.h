#ifndef _COINCHE_H
#define _COINCHE_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

typedef struct Joueur Joueur;
struct Joueur
{
    char carte[8][5]; //les cartes du jouer
    int contrat; //le contrat que detient le joueur
    char nom[2]; //le nom du joueur
    int possibilite; //recupere les choix du jouer
    char possibilite_carte[8][5]; //stockes les cartes possible pour le joueur à jouer
    int nb_possibilite_carte; //nb de cartes possibles à jouer
    char contrat_atout[3]; //nb de carte à cet atout
    int score_perso; // le score personnel pour verifier le general
    int annonce; //retient l'annonce du joueur
    char quelle_annonce[30]; //retient le type d'annoce pour l'afficher
    char vrai_nom[10]; //valable pour le joueur S : retient le vrai nom du joueur et non sa position
    int pos_score; //valable pour le joueur S : rentient sa position dans le tableau des socres pour l'actualiser a la fin
};

typedef struct Jeu Jeu;
struct Jeu
{
    int contrat; //recupere le contrat de la partie
    char type_contrat[3]; //recupere les caractéristiques du contrat : classique, general ou capot
    char detenteur_contrat[2]; //qui possède ce contrat
    char atout[3]; //quel est l'atout du contrat
    char equipe_preneuse[4]; //quel est l'equipe qui detient le contrat
    char gagnant_precedent[2]; //le nom de gagnant du pli precedent
    char table[8][5]; //stocke les cartes au milieu, 1 ere case : W, 2 eme N, 3eme E, 4 eme S
    char carte_maitresse[5];//permet de savoir la maitresse du pli
    int tour; //stocke le tour du pli actuel
    char detenteur_carte_maitresse[2];
    int score_pli; //le score du pli en cours
    int pli; //le nombre de pli en cours
    int compteur_belote_rebelote; //compteur qui compte le roi et la dame a l'atout
    int annonce_max;//stocke la valeur de l'annonce max
    char detenteur_annonce_max[2];
    char quelle_annonce_max[64];
    char evenements_ligne_1[64]; //stocke les evenments a afficher
    int ordre_de_jeu; //ordre de jeu lors du commencement du pli
    int ordre; //ordre de jeu a l'interieur de pli
    int surencherir; //variable qui stocke si quelqu'un a deja surencherit pour eviter de surencherir sans fin
    char detenteur_belote_rebelote[5]; //celui qui detient la belote rebelote, pour l'affichage
    char detenteur_dix_de_der[5];//celui qui detient le dix de der, pour l'affichage
    char atout_restant[4][3]; //stocke la couleur des cartes maitresses dispo, pour compter les cartes : si TA alors cela compte les valet, si SA c'est l'as
    char atout_couleur_restant[8][3]; //stocke les cartes a l'atout pour compter les cartes, classee de la plus fortes a la plus faibles
};

typedef struct Equipe Equipe;
struct Equipe
{
    char nom[4]; //stocke le nom des joueurs des equipes
    int annonce; //stocke le points dues aux annonces
    int nb_pli; //stocke le nombre de pli gagnés
    int score_manche; //stocke le score gagné lors de la manche uniquement avec les plis
    int score_total; //stcoke le score total additionné des manches
    int belote_rebelote; // les points attribués pour la belote rebolote
};

typedef struct Scores Scores; //structure qui contient toutes informations de score des joueurs
struct Scores
{
    char nom[10];
    int nb_parties;
    int nb_victoires;
    int meilleur_score;
};



void lancement_jeu();
/**
Cette fonction permet de lancer le jeu,initialise le packet de carte, initialise toutes les variables des joueurs, de la partie, puis
lance la manche ainsi que le premier plis : melange, distribution et affichages des cartes pour chaques joueurs, definition d un ordre
 de jeu, appelle de la fonction lancement_manche...
**/
void melange(char paquet[32][5]);
/**
Cette fonction permet de melanger le paquet de cartes au debut de la manche, par inversion successive
**/
void distri_total(char paquet[32][5], Joueur *W, Joueur *N, Joueur *E, Joueur *S, Jeu *partie);
/**
Cette fonction va permettre la distribution du paquet de cartes pour chaques joueurs, elle va distribuer pour chaques joueurs 8 cartes deja mélanger,
parmis le paquet de 32 cartes
**/
void range_croissant(char paquet[8][5], char atout[3], Jeu *partie);
/**
Elle permet de ranger pour chaques joueurs, le paquet de cartes suivant un ordre precis : trefle, coeur, pic, carreau suivant l ordre croissant des
nombres
**/
int valeur(char carte[5], char atout[3]);
/**
Cette fonction permet d affecter une valeur pour chaques cartes du paquet du joueur a l aide d un switch
**/
void quelle_carte(char carte[5], char adresse[25]); //donne l'image correspondante à chaque carte
/**
Cette fonction va permettre d afficher sur l interface graphique l image de la carte correpondante, elle renvoie l adresse de l image de la carte
correspondate du joueur
**/
void selection_joueur(SDL_Surface *fenetre, char position);
/**
Elle a pour but d afficher un rond rouge autour du joueur qui doit jouer
**/
void rafraichir(SDL_Surface *fenetre, Jeu *partie, Equipe *N_S, Equipe *E_W, Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S);
/**
Elle a pour but d afficher un nouveau fond sur la fenetre, ainsi que d appeler les fonctions d affichage des contrats, recap partie, recap
manche...
**/
void rafraichir_possibilitees(SDL_Surface *fenetre);
/**
Elle rafraichi juste l'encadre possibilite pour eviter de tout rafraichir inutilement
**/
void affiche_table(Jeu *partie, SDL_Surface *fenetre);
/**
Elle affiche les cartes de la table en appelant la fonction quelle carte
**/
void affiche_carte(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, SDL_Surface *fenetre);
/**
Elle affiche les cartes du paquet du joueur en appelant la fonction quelle carte
**/
void affich_possibilite_carte(Joueur *j, SDL_Surface *fenetre);
/**
Elle affiche les differentes cartes que le joueur peut jouer en fonction des cartes posees sur la table
**/
void affiche_infos_manche(SDL_Surface *fenetre, Jeu *partie, Equipe *N_S, Equipe *E_W);
/**
elle affiche les informations de l encadre info manche
**/
void affiche_indication_enchere(SDL_Surface *fenetre, int changement, char nom, char historique_enchere[5]);
/**
elle affiche les informations en cours de la phase d'enchere en prenant pour variable l'historique des personnes ayant encherit
**/
void affiche_contrat(SDL_Surface *fenetre, Jeu *partie);
/**
elle affiche les informations de l encadre info manche
**/
void affiche_recap_partie(SDL_Surface *fenetre, Jeu *partie, Equipe *N_S, Equipe *E_W);
/**
elle affiche dans l'encadre specifique quelle type d'atout a ete retenu pour les contrats
**/
void affiche_evenements(SDL_Surface *fenetre, Jeu *partie, char ligne_1[64], char ligne_2[64]); //affiche les elements
/**
elle affiche le texte correspondant aux evenement en cours
**/
void affiche_evenements_fixe(SDL_Surface *fenetre, Jeu *partie); //affiche le cadre
/**
elle affiche l'encadre evenement et affiche le libelle correspond a la phase de jeu en cours
**/
void affiche_recap_manche(SDL_Surface *fenetre, Jeu *partie, Equipe *N_S, Equipe *E_W);
/**
affiche les informations de l'encadre recap manche
**/
void affiche_possibilite(Joueur *j, Jeu *partie, SDL_Surface *fenetre);
/**
Elle repli le tableau de caractere joueur.possihbilitees cartes sans les afficher
**/
int bouton_IA(SDL_Surface *fenetre);
/**
Fonction permettant d afficher le bouton faire jouer IA dans une partie dans l interface graphique
**/
int bouton_relancer(SDL_Surface *fenetre);
/**
Fonction permettant d afficher le bouton relancer une partie dans l interface graphique
**/
void bouton_debut_manche(SDL_Surface *fenetre);
/**
Fonction permettant d afficher le bouton debut manche dans l interface graphique
**/
int bouton_continuer(SDL_Surface *fenetre);
/**
Fonction permettant d afficher le bouton continuer dans l interface graphique
**/
int contrat(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, SDL_Surface *fenetre, Equipe *N_S, Equipe *E_W);
/**
fonction qui gere les enchères de la partie
fait encherire chaque joueur suivant un ordre en appellant la fonction choix
affiche les indications des encheres

**/
int choix(Joueur *j, Jeu *partie, SDL_Surface *fenetre, char *historique_enchere);
/**
Si le joueur est reel, alors il peut encherire ou passer
si il encherit, alors il peux choisir le mode de jeu : capot, general, classique
L utilisateur choisit si il veut jouer TA/SA ou couleur, puis tape son annonce
si c est un robot, on appelle la fonction determination contrat pour faire une approximation de son contrat possible
**/
int demande_enchere_couleur (Jeu *partie, int ench_max, Joueur *j);
/**
fait encherir le robot de 10 par 10 juqu'a la somme maximal qu'il veut miser
actualise le detenteur du contrat
**/
void attribution_equipe_preneuse(Equipe *N_S, Equipe *E_W, Jeu *partie);
/**
Cette fonction va actualiser le nom de l equipe preneuse pour chaque pli
**/
int lancement_manche(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, Equipe *N_S, Equipe *E_W, SDL_Surface *fenetre);
/**
Va initialiser le lancement d une manche
Appelle les fonctions necessaires au deroulement d une manche : distribution cartes, rangements croissants..
**/
int lancement_pli(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, Equipe *N_S, Equipe *E_W, SDL_Surface *fenetre);
/**
Va initialiser le lancement d un pli
Appelle les differentes fonctions necessaires au deroulement d un pli (belote rebelotte, contrat..)
**/
int pose(Joueur *j, Jeu *partie, SDL_Surface *fenetre, Equipe *N_S, Equipe *E_W);
/**
Acutalise les donnees de la partie : detenteur annoonce, annonce max ...
Affiche les possibilite au joueur reel, et lui fait choisir en appellant choisir_possibilite
**/
int choisir_possibilite(Joueur *j, Jeu *partie);
/**
Cette fonction permet de faire choisir au joueur reel, la cartes qu'il veut poser suivant les differentes possibilites
**/
void actualisation_carte_maitresse(Joueur *j, Jeu *partie);
/**
Va actualiser la carte maitresse lors d une manche
Actualise egalement le nom du detenteur de la carte maitresse
**/
void actualisation_atout(Jeu *partie);
/**
Actualise les atouts pour le comptages des atouts dans la partie
**/
void calcul_des_points(Jeu *partie);
/**
Calcul la sommes des points de chaques equipes et de chaques joueurs a la fin de chaques plis
**/
void attribution_des_points(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Equipe *N_S, Equipe *E_W, Jeu *partie);
/**
Permet l attribution des points de chaque equipe et de chaque joueurs dans les plis
**/
int verification_des_contrats(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, Equipe *N_S, Equipe *E_W, SDL_Surface *fenetre);
/**
Va verifier pour chaque equipe, la valeur du contrat en fonction des points obtenue dans la manche
**/
void detection_belote_rebelote(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, Equipe *N_S, Equipe *E_W);
/**
Cette fonction va determiner si une equipe possede le rois et la dame dans une partie
Va attribuer a l equipe 10 points supplementaire
**/
void meilleur_annonce(Joueur *joueurN,Joueur *joueurS, Joueur *joueurE, Joueur *joueurW, Equipe *equipeNS, Equipe *equipeEW);
/**
Cette fonction va determiner et retenir la meilleur annonce parmis les quatres joueurs
**/
void calcul_annonce(char paquet[8][5], Joueur *j, Jeu *partie);
/**
Va relerver la presence (ou non) de chaque cartes du paquet du joueur
va appeller la fonction annonce_suite
elle va determiner si le joueur possède des carres ou non
determine l annonce maximum que le joueur peux avoir
**/
void jeu_ordi(Joueur *j, Jeu *partie);
/**
Permet de faire choisir a l ordinateur la cartes qu il veut poser
**/
void choix_possibilite_ordi(Joueur *j, Jeu *partie);
/**
Cette fonction va afficher les differentes possibilites pour chaques ordinateurs
Effectue un choix parmis les possibilites en appellant la fonction jeu_ordi
**/
int annonce_suite(char paquet[8][5], Joueur *j);
/**
Elle va trier le paquet suivant les quatres couleurs, puis appeler 4 fois la fonction annonce_famille afin de garder la plus grosse annonce parmis
les 4
**/
int annonce_famille(char temp[8][5], Joueur *j);
/**
Cette fonction va relever la présence de chaque carte dans une couleur, puis calculer l annonce possible (ou non) pour une couleur du paquet de cartes
**/
int contrat_TA(char paquet[8][5],int compte_V,int compte_9, int compte_A, int compte_9P,int compte_9Ca,int compte_9Co,int compte_9T);
/**
Fonction qui va determiner une approximation du contrat si le joueur joue SA
**/
int contrat_SA(char paquet[8][5],int compte_A, int compte_10, int compte_R,int compte_10P,int compte_10Ca,int compte_10Co,int compte_10T);
/**
Fonction qui va determiner une approximation du contrat si le joueur joue TA
**/
int determination_contrat(char paquet[8][5], Joueur *j, Jeu *partie);
/**
Cette fonction va relever la presence de chaque cartes dans le paquet, puis appeler les fonctions contrat_TA, contrat_SA et contrat_famille afin de
determiner et garder uniquement le contrat le plus haut
**/
int qui_commence(Jeu *partie);
/**
Elle va permettre de determiner un ordre de rotation des quatres joueurs dans une partie
**/
void flush_variables_manche(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Equipe *N_S, Equipe *E_W, Jeu *partie);
/**
Cette fonction initialise l integralite des fonctions et des variables des joueurs et de la partie a 0
**/
int contrat_famille(char temp[8][5]);
/**
Elle a pour but de trier le paquet de chaque joueur selon les quatres couleurs, d appeler la fonction contrat_famille pour chaque couleur et de garder
que le contrat le plus haut
**/
int contrat_couleur(char paquet[8][5], char couleur[3]);
/**
Cette fonction va determiner une approximation du contrat par couleur pour chaque joueur
**/
int recup_nom(Joueur *joueur_S, Jeu *partie, SDL_Surface *fenetre, Scores tableau_scores[]);
/**
Fonction qui recupere le nom du joueur et lui dit combien de partie il a deja fait
**/

int chargement_tableau_scores(Scores tableau_scores[], Joueur *joueur_S);
/**
Fonction qui charge les donnes du fichier score dans le tableau
**/

void actualisation_scores(Scores tableau_scores[], Joueur *joueur_S, Jeu *partie, Equipe *N_S, Equipe *E_W);
/**
Fonction qui charge les donnes du tableau dans le fichier
**/

#endif
