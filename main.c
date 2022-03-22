#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include "coinche.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>


// A L'ATTENTION DU CORRECTEUR, SI VOUS VOULEZ LANCER LE JEU ALORS QUE VOUS N'AVEZ PAS LES BIBLIOTHEQUES D'IMAGE SDL, LANCER LE .exe PRESENT
// DANS BIN/DEBUG QUI CONTIENT TOUT CE QU'IL FAUT POUR FONCTIONNER TOUT SEUL
// MERCI DE VOTRE COMPREHENSION

int main ( int argc, char** argv )
{
    SDL_Surface *fenetre = NULL, *img_fond = NULL;
    SDL_Rect positionFond;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO); //initialise la sdl

    fenetre = SDL_SetVideoMode(1280, 720, 32, SDL_HWSURFACE); //initialise la fenetre pour la sdl

    TTF_Init();//on initialise la bibliotheque de texte

    int continuer = 1; //on attend que la personne appuie sur un bouton, ou quitte ferme la fenetre

    while (continuer)
    {
        SDL_WM_SetCaption("Menu Belote", NULL);
        img_fond = IMG_Load("Src/menu_coinche.png");//charge l'image de fond
        positionFond.x = 0;//correspond a la posistion du fond
        positionFond.y = 0;
        SDL_BlitSurface(img_fond, NULL, fenetre, &positionFond);//colle la surface sur la fenetre
        SDL_Flip(fenetre);//rafraichis la fenetre

        SDL_WaitEvent(&event); //attend un evenement
        switch(event.type) //un switch selon l'evenement enregiste
        {
            case SDL_QUIT : continuer = 0; //si on clique sur la croix on quitte le jeu
                break;
            case SDL_MOUSEBUTTONDOWN : //si on clique on verifie que ce soit le bouton gauche et on regarde sur quoi il a cliqu
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (event.button.x > 720 && event.button.x <953 && event.button.y >582 && event.button.y < 642) //si il appuie sur le bouton quitter
                    {
                        continuer = 0;
                    }
                    else if (event.button.x > 30 && event.button.x <446 && event.button.y >268 && event.button.y < 331) //si il appuie sur le bouton jouer
                    {
                        lancement_jeu(fenetre);
                    }
                    else if (event.button.x > 30 && event.button.x <241 && event.button.y >395 && event.button.y < 461) //si il appuie sur le bouton regle
                    {
                        int decision = 1;
                        SDL_WM_SetCaption("Regles Belote", NULL);
                        img_fond = IMG_Load("Src/regles.png"); //affiche les regles
                        positionFond.x = 0;
                        positionFond.y = 0;
                        SDL_BlitSurface(img_fond, NULL, fenetre, &positionFond);
                        SDL_Flip(fenetre);
                        while (decision)
                        {
                            SDL_WaitEvent(&event);
                            switch(event.type)
                            {
                                case SDL_QUIT : decision = 0;
                                    break;
                                case SDL_MOUSEBUTTONDOWN :
                                    if (event.button.button == SDL_BUTTON_LEFT) //appuyer sur la souris pour quitter
                                    {
                                        decision = 0;
                                    }
                                    break;
                            }
                        }
                    }
                    else if (event.button.x > 30 && event.button.x <238 && event.button.y >528 && event.button.y < 576) //si il appuie sur le bouton score
                    {
                        int decision = 1;
                        SDL_WM_SetCaption("Scores Belote", NULL);
                        img_fond = IMG_Load("Src/fond_score.png");
                        positionFond.x = 0;
                        positionFond.y = 0;
                        SDL_BlitSurface(img_fond, NULL, fenetre, &positionFond);
                        SDL_Flip(fenetre);

                        Scores tableau_scores[20];
                        for (int i = 0; i < 20; i += 1) //initialise le tableau
                        {
                            strcpy(tableau_scores[i].nom, "");
                            tableau_scores[i].meilleur_score = -1000;
                            tableau_scores[i].nb_parties = 0;
                            tableau_scores[i].nb_victoires = 0;
                        }
                        FILE *fichier_scores = fopen("score.txt", "r");
                        int i = 0, k = 0;
                        if(fichier_scores != NULL) //recupere les donnes du ficher pour le metttre dans un tableau si le fichier a bien ete ouvert
                        {
                            do {
                                fscanf(fichier_scores, "%s ;%d ;%d ;%d ;", tableau_scores[i].nom, &tableau_scores[i].nb_parties, &tableau_scores[i].nb_victoires, &tableau_scores[i].meilleur_score);
                                i++;
                            } while (fgetc(fichier_scores) != EOF && i < 20);
                            fclose(fichier_scores);
                        }
                        fclose(fichier_scores);

                        SDL_Surface *texte = NULL;
                        SDL_Rect pos_texte;
                        TTF_Font *police = TTF_OpenFont("helvetica.ttf", 35);
                        SDL_Color blanc = {255,255,255};
                        char score[30] = "";
                        while (strcmp(tableau_scores[k].nom, "")!=0) //affiche les differents element des scores
                        {
                            pos_texte.x = 96;
                            pos_texte.y = 188 + 35*k;
                            sprintf(score, "%d", k+1);
                            texte = TTF_RenderText_Blended(police, score, blanc);
                            SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);

                            pos_texte.x = 320;
                            pos_texte.y = 188 + 35*k;
                            texte = TTF_RenderText_Blended(police, tableau_scores[k].nom, blanc);
                            SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);

                            pos_texte.x = 637;
                            pos_texte.y = 188 + 35*k;
                            sprintf(score, "%d", tableau_scores[k].nb_parties);
                            texte = TTF_RenderText_Blended(police, score, blanc);
                            SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);

                            pos_texte.x = 840;
                            pos_texte.y = 188 + 35*k;
                            sprintf(score, "%d", tableau_scores[k].nb_victoires);
                            texte = TTF_RenderText_Blended(police, score, blanc);
                            SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);

                            pos_texte.x = 1090;
                            pos_texte.y = 188 + 35*k;
                            sprintf(score, "%d", tableau_scores[k].meilleur_score);
                            texte = TTF_RenderText_Blended(police, score,blanc);
                            SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);

                            k += 1;
                        }
                        SDL_FreeSurface(texte);
                        TTF_CloseFont(police);
                        SDL_Flip(fenetre);

                        while (decision)
                        {
                            SDL_WaitEvent(&event);
                            switch(event.type)
                            {
                                case SDL_QUIT : decision = 0;
                                    break;
                                case SDL_MOUSEBUTTONDOWN :
                                    if (event.button.button == SDL_BUTTON_LEFT) //appuyer sur la souris pour quitter
                                    {
                                        decision = 0;
                                    }
                                    break;
                            }
                        }
                    }
                }
                break;
        }
    }

    SDL_FreeSurface(img_fond);
    SDL_Quit();
    TTF_Quit();

    return EXIT_SUCCESS;
}

void lancement_jeu(SDL_Surface *fenetre)
{

    char paquet_32_cartes[32][5] = {"A P","R P","Q P","V P","10P","9 P","8 P","7 P","A T","R T","Q T","V T","10T","9 T","8 T","7 T","A Co",
    "R Co","Q Co","V Co","10Co","9 Co","8 Co","7 Co","A Ca","R Ca","Q Ca","V Ca","10Ca","9 Ca","8 Ca","7 Ca"};

    Joueur ordi_W = {{{""},{""}}, 0, "W", 0,{{""},{""}},0,{""},0,0,{""},{""},0};
    Joueur ordi_N = {{{""},{""}}, 0, "N", 0,{{""},{""}},0,{""},0,0,{""},{""},0};
    Joueur ordi_E = {{{""},{""}}, 0, "E", 0,{{""},{""}},0,{""},0,0,{""},{""},0};
    Joueur joueur_S = {{{""},{""}}, 0, "S", 0,{{""},{""}},0,{""},0,0,{""},{""},0};

    Jeu partie = {79,{""},{""},{""},{""},{""},{{""},{""}},{""}, 0,{""}, 0, 0, 0, 0,{""},{""},{""}, 0, 0,0,{""},{""},{"T","Co","P","Ca"},{"V","9","A","1","R","Q","8","7"}};

    Equipe N_S = {{"N_S"},0,0,0,0,0};
    Equipe E_W = {{"E_W"},0,0,0,0,0};

    Scores tableau_scores[20];
    for (int i = 0; i < 20; i += 1) //initialise le teableau stockant les scores
    {
        strcpy(tableau_scores[i].nom, "");
        tableau_scores[i].meilleur_score = -1000;
        tableau_scores[i].nb_parties = 0;
        tableau_scores[i].nb_victoires = 0;
    }

    srand(time(NULL));

    int exit = 0; //permet de quitter le jeu en cas de dermeture du jeu

    //initialisation de la page de jeu

    SDL_WM_SetCaption("Jeu Belote", NULL);

    exit = recup_nom(&joueur_S, &partie, fenetre, tableau_scores); //recupere le nom du joueur
    if (exit == 1)
    {
        return EXIT_FAILURE;
    }

    do{
    //Initialisation de la manche
    //reinitialisation des variables
    flush_variables_manche(&ordi_W, &ordi_N, &ordi_E, &joueur_S, &N_S, &E_W, &partie);
        do
        {
            partie.ordre_de_jeu = rand()%4; //permet de savoir qui commence dans la partie

            strcpy(partie.evenements_ligne_1, "* Phase d'enchère *"); //libelle de la phase de jeu en cours

            rafraichir(fenetre, &partie, &N_S, &E_W, &ordi_W, &ordi_N, &ordi_E, &joueur_S); //rafraichis la fenetre pour commencer

            melange(paquet_32_cartes);

            distri_total(paquet_32_cartes, &ordi_W, &ordi_N, &ordi_E, &joueur_S, &partie); //dirtibuution des cartes aux joueurs

            affiche_carte(&ordi_W, &ordi_N, &ordi_E, &joueur_S, &partie, fenetre);

            exit = contrat(&ordi_W, &ordi_N, &ordi_E, &joueur_S, &partie, fenetre, &N_S, &E_W); //lance la phase d'enchere
            if(exit == 1)
            {
                return EXIT_FAILURE;
            }

            if (strcmp(partie.atout, "")==0) //relance le jeu si tout le monde passe
            {
                exit = bouton_relancer(fenetre);
            }

        }while (strcmp(partie.atout, "")==0); // relance s'il n'y a pas eu d'encheres

        bouton_debut_manche(fenetre);

        strcpy(partie.evenements_ligne_1, "   * Phase de jeu *");

        rafraichir(fenetre, &partie, &N_S, &E_W, &ordi_W, &ordi_N, &ordi_E, &joueur_S);

        //Debut de la manche
        attribution_equipe_preneuse(&N_S, &E_W, &partie); //attribue le contrat à l'equipe

        exit = lancement_manche(&ordi_W, &ordi_N, &ordi_E, &joueur_S, &partie, &N_S, &E_W, fenetre);
        if(exit == 1)
        {
            return EXIT_FAILURE;
        }

        exit = verification_des_contrats(&ordi_W, &ordi_N, &ordi_E, &joueur_S, &partie, &N_S, &E_W, fenetre); //verifie si les ocntrats on ete respectes

        if(exit == 1)
        {
            return EXIT_FAILURE;
        }
        //preparation de la manche suivante :
        flush_variables_manche(&ordi_W, &ordi_N, &ordi_E, &joueur_S, &N_S, &E_W, &partie);

        if (partie.ordre_de_jeu == 3)
        {
            partie.ordre_de_jeu = 0;
        }
        else
        {
            partie.ordre_de_jeu += 1;
        }

    } while(N_S.score_total <= 700 && E_W.score_total <= 700);

    strcpy(partie.evenements_ligne_1, "     * Debriefing *");

    rafraichir(fenetre, &partie, &N_S, &E_W, &ordi_W, &ordi_N, &ordi_E, &joueur_S);

    char text[64] = "";

    if (N_S.score_total > E_W.score_total)
    {
        sprintf(text,"L'equipe N_S gagne et marque %d points d'ecart", N_S.score_total - E_W.score_total);
    }
    else
    {
       sprintf(text,"L'equipe E_W gagne et marque %d points d'ecart", E_W.score_total - N_S.score_total);
    }
    affiche_evenements(fenetre,&partie,text,"");
    affiche_evenements(fenetre,&partie,"","Fin de la partie - Appuyez pour recommencer");

    bouton_continuer(fenetre);

    actualisation_scores(tableau_scores, &joueur_S, &partie, &N_S, &E_W); // actualise le fichier qui stocke les scores
}

