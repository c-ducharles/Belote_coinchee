#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <time.h>
#include "coinche.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>


void melange(char paquet[32][5]) //melange les cartes par inversion successives
{
    int pos1, pos2;
    char temp[5];
    for (int i = 0; i < 30; i += 1)
    {
        pos1 = rand()%32;
        pos2 = rand()%32;
        strcpy(temp, paquet[pos1]);
        strcpy(paquet[pos1], paquet[pos2]);
        strcpy(paquet[pos2], temp);
    }
}

void distri_total(char paquet[32][5], Joueur *W, Joueur *N, Joueur *E, Joueur *S, Jeu *partie) //distribue à tout le monde
{
    for (int i = 0; i < 8; i += 1)
    {
        strcpy(W->carte[i],paquet[0 + 4*i]);
        strcpy(N->carte[i],paquet[1 + 4*i]);
        strcpy(E->carte[i],paquet[2 + 4*i]);
        strcpy(S->carte[i],paquet[3 + 4*i]);
    }
    //range par ordre croissant les cartes
    range_croissant(W->carte, "SA", partie); //on classe les carte la premire fois simplement on considerant qu'il n'y a pas d'atout
    range_croissant(N->carte, "SA", partie);
    range_croissant(E->carte, "SA", partie);
    range_croissant(S->carte, "SA", partie);
}

void range_croissant(char paquet[8][5], char atout[3], Jeu *partie) //on range les cartes en deux etapes, on trie pas couleur et après on classe
{
    //on rassemble d'abord les trefle, puis les coeur, puis pic puis les carreau de la gauche vers la droite
    int curseur = 0; //atribu les cartes à tel case
    int i = 0; //parcours le paquet
    int num_choix_atout = 0;//donne le numéro correspondnat à chaque atout
    char choix_atout[4][4] = {"T","Co","P","Ca"};
    int nombre_choix_atout[4] = {0}; //donne le nombre de carte à l'atout d'après le schéma au dessus
    char temp[5] = ""; //pour l'inversion

    while (num_choix_atout < 3 && curseur < 8)
    {
        i = curseur; //on fait demarrer le i à la bonne case
        while (strcmp(choix_atout[num_choix_atout], &(paquet[i][2]))!= 0 && i < 8) //me ressort la position de la premiere carte qui correspond
            {
                i += 1;
            }

        if (i == 8) //ce qui veut dire que l'on a pas trouvé une carte avec l'atout qui correspondait
        {
            num_choix_atout += 1;
        }
        else
        {
            strcpy(temp, paquet[curseur]); //on intervertit
            strcpy(paquet[curseur], paquet[i]);
            strcpy(paquet[i], temp);
            curseur += 1;

        }
    }

    //maintenant on va classer par odre croissant les cartes

        //on va pouvoir traiter les cartes par paquet de couleur
        num_choix_atout = 0;
        int max; // la carte avec la valeur max
        int pos_carte_max; //la position de la carte avec la valeur max
        curseur = 0; //decale sur la premiere carte à l'atout concerné
        for (num_choix_atout = 0; num_choix_atout < 4; num_choix_atout += 1)
        {
            // on récupère les coordonnées des cartes correspondant à chaque atout
            for (int i = 0; i < 8; i += 1)
            {
                if (strcmp(&paquet[i][2], choix_atout[num_choix_atout]) == 0)
                {
                    nombre_choix_atout[num_choix_atout] += 1;

                }
            }

            if (nombre_choix_atout[num_choix_atout] >= 2)
            {

                for (int b = 0; b < nombre_choix_atout[num_choix_atout] - 1; b += 1)
                {
                    int k = 0;
                    max = -10;
                    int val = 0;
                    pos_carte_max = 0;
                    while (k < nombre_choix_atout[num_choix_atout] - b)
                    {
                        val = valeur(paquet[curseur + k],partie->atout);
                        if (val > max)
                        {
                            max = valeur(paquet[curseur + k],partie->atout);
                            pos_carte_max = curseur + k;
                        }

                        k += 1;
                    }
                    strcpy(temp, paquet[pos_carte_max]);
                    strcpy(paquet[pos_carte_max], paquet[curseur + nombre_choix_atout[num_choix_atout] - 1 - b]);
                    strcpy(paquet[curseur + nombre_choix_atout[num_choix_atout] - 1 - b], temp);
                }


            }


            curseur += nombre_choix_atout[num_choix_atout]; //on décale du nombre de carte traitee une fois fini de trier
        }

}


int valeur(char carte[5], char atout[3]) //retourne la valeur liee a la carte
{
    int point = 0;

    if (strcmp(atout, "SA")!= 0 && strcmp(atout, "TA")!= 0)
    {
        if (strcmp(&carte[2], atout) == 0) //si la carte est de la couleur de l'atout
        {
            switch(carte[0])//On ne va comparer que la première case qui contient le numéro de la carte donc le 10 sera 1
            {
                case 'V' : point = 20;
                    break;
                case '9' : point = 14;
                    break;
                case 'A' : point = 11;
                    break;
                case '1' : point = 10;
                    break;
                case 'R' : point = 4;
                    break;
                case 'Q' : point = 3;
                    break;
                case '8' : point = 0;
                    break;
                case '7' : point = -1;
                    break;
            }
        }
        else
        {
            switch(carte[0])
            {
                case 'A' : point = 11;
                    break;
                case '1' : point = 10;
                    break;
                case 'R' : point = 4;
                    break;
                case 'Q' : point = 3;
                    break;
                case 'V' : point = 2;
                    break;
                case '9' : point = 0;
                    break;
                case '8' : point = -1;
                    break;
                case '7' : point = -2;
                    break;
            }
        }
    }
    else if (strcmp(atout, "SA")==0)
    {
        switch(carte[0])//On ne va comparer que la première case qui contient le numéro de la carte donc le 10 sera 1
            {
                case 'A' : point = 19;
                    break;
                case '1' : point = 10;
                    break;
                case 'R' : point = 4;
                    break;
                case 'Q' : point = 3;
                    break;
                case 'V' : point = 2;
                    break;
                case '9' : point = 0;
                    break;
                case '8' : point = -1;
                    break;
                case '7' : point = -2;
                    break;
            }
    }
    else
    {
        switch(carte[0])
            {
                case 'V' : point = 14;
                    break;
                case '9' : point = 9;
                    break;
                case 'A' : point = 6;
                    break;
                case '1' : point = 5;
                    break;
                case 'R' : point = 3;
                    break;
                case 'Q' : point = 1;
                    break;
                case '8' : point = 0;
                    break;
                case '7' : point = -1;
                    break;
            }
    }
    return point;
}

void affich_possibilite_carte(Joueur *j, SDL_Surface *fenetre) //affiche les cartes posssibles pour le joueur
{
    SDL_Surface *img_carte = NULL;
    SDL_Rect pos_image;
    for (int i = 0; i < j->nb_possibilite_carte; i += 1)
    {
        char adresse[25] = "";
        quelle_carte(j->possibilite_carte[i], adresse);
        img_carte = IMG_Load(adresse);
        pos_image.x = 697 + 73 * i;
        pos_image.y = 64;
        SDL_BlitSurface(img_carte, NULL, fenetre, &pos_image);
    }
    SDL_FreeSurface(img_carte);
    SDL_Flip(fenetre);

}

void rafraichir(SDL_Surface *fenetre, Jeu *partie, Equipe *N_S, Equipe *E_W, Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S)
{
    SDL_Surface *img_fond = NULL;
    SDL_Rect pos_fond;
    img_fond = IMG_Load("Src/fond_jeu.jpg");
    pos_fond.x = 0;
    pos_fond.y = 0;
    SDL_BlitSurface(img_fond, NULL, fenetre, &pos_fond);
    affiche_carte(ordi_W, ordi_N, ordi_E, joueur_S, partie, fenetre);
    affiche_contrat(fenetre, partie);
    affiche_recap_partie(fenetre, partie, N_S, E_W);
    affiche_recap_manche(fenetre, partie, N_S, E_W);
    affiche_evenements_fixe(fenetre, partie);
    affiche_infos_manche(fenetre, partie, N_S, E_W);
    affiche_table(partie, fenetre);
    SDL_Flip(fenetre);
    SDL_FreeSurface(img_fond);
}

void affiche_evenements_fixe(SDL_Surface *fenetre, Jeu *partie)
{
    SDL_Surface *img_evenements = NULL;
    SDL_Rect pos_evenements;
    //Affichage du evenement
    img_evenements = IMG_Load("Src/Evenements.png");
    pos_evenements.x = 704;
    pos_evenements.y = 168;
    SDL_BlitSurface(img_evenements, NULL, fenetre, &pos_evenements);
    //affichage des evenements enregisres
    SDL_Surface *texte = NULL;
    SDL_Rect pos_texte;
    TTF_Font *police = TTF_OpenFont("helvetica.ttf", 25);
    TTF_SetFontStyle(police, TTF_STYLE_BOLD);
    SDL_Color noir = {0,0,0};
    SDL_Color blanc = {255,255,255};
    pos_texte.x = 855;
    pos_texte.y = 215;
    texte = TTF_RenderText_Shaded(police, partie->evenements_ligne_1,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    SDL_FreeSurface(texte);
    TTF_CloseFont(police);
    SDL_FreeSurface(img_evenements);
}

void affiche_evenements(SDL_Surface *fenetre, Jeu *partie, char ligne_1[64], char ligne_2[64])
{
    SDL_Surface *texte = NULL;
    SDL_Rect pos_texte;
    TTF_Font *police = TTF_OpenFont("helvetica.ttf", 20);
    SDL_Color noir = {0,0,0};
    SDL_Color blanc = {255,255,255};
    if (strcmp(ligne_1, "")!=0)
    {
        pos_texte.x = 719;
        pos_texte.y = 245;
        texte = TTF_RenderText_Shaded(police, ligne_1,noir,blanc);
        SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    }
    if (strcmp(ligne_2, "")!=0)
    {
        pos_texte.x = 719;
        pos_texte.y = 275;
        texte = TTF_RenderText_Shaded(police, ligne_2,noir,blanc);
        SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    }
    SDL_Flip(fenetre);
    SDL_FreeSurface(texte);
    TTF_CloseFont(police);
}

void affiche_contrat(SDL_Surface *fenetre, Jeu *partie)
{
    SDL_Surface *img_contrat = NULL, *img_atout = NULL;
    SDL_Rect pos;
    //on affiche le fond
    img_contrat = IMG_Load("Src/Affichage_contrats.png");
    pos.x = 704;
    pos.y = 521;
    SDL_BlitSurface(img_contrat, NULL, fenetre, &pos);
    //on affiche l'atout
    if (strcmp(partie->atout,"")!=0)
    {
        if (strcmp(partie->atout, "T")==0)
        {
            img_atout = IMG_Load("Src/Contrats/Atout_T.jpg");
        }
        else if (strcmp(partie->atout, "Co")==0)
        {
            img_atout = IMG_Load("Src/Contrats/Atout_Co.jpg");
        }
        else if (strcmp(partie->atout, "P")==0)
        {
            img_atout = IMG_Load("Src/Contrats/Atout_P.jpg");
        }
        else if (strcmp(partie->atout, "Ca")==0)
        {
            img_atout = IMG_Load("Src/Contrats/Atout_Ca.jpg");
        }
        else if (strcmp(partie->atout, "SA")==0)
        {
            img_atout = IMG_Load("Src/Contrats/SA.png");
        }
        else if (strcmp(partie->atout, "TA")==0)
        {
            img_atout = IMG_Load("Src/Contrats/TA.png");
        }
        else
        {
            fprintf(stderr, "Erreur chargement atout contrat\n");
        }
    }
    else
    {
        img_atout = IMG_Load("Src/Contrats/Rien.png");
    }
    pos.x = 769;
    pos.y = 610;
    SDL_BlitSurface(img_atout, NULL, fenetre, &pos);
    //on affiche le detenteur du contrat
    SDL_Surface *texte = NULL;
    SDL_Rect pos_texte;
    TTF_Font *police = TTF_OpenFont("helvetica.ttf", 20);
    SDL_Color noir = {0,0,0};
    SDL_Color blanc = {255,255,255};
    pos_texte.x = 815;
    pos_texte.y = 555;
    texte = TTF_RenderText_Shaded(police, partie->detenteur_contrat,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    //on affiche le type de contrat
    pos_texte.x = 810;
    pos_texte.y = 578;
    texte = TTF_RenderText_Shaded(police, partie->type_contrat,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    //on affiche la valeur du contrat
    if (partie->contrat > 79)
    {
        char array_contrat[4];
        sprintf(array_contrat,"%d",partie->contrat);
        police = TTF_OpenFont("helvetica.ttf", 30);
        pos_texte.x = 713;
        pos_texte.y = 642;
        texte = TTF_RenderText_Shaded(police, array_contrat,noir,blanc);
        SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    }
    SDL_FreeSurface(texte);
    TTF_CloseFont(police);
    SDL_FreeSurface(img_contrat);
    SDL_FreeSurface(img_atout);
}

void affiche_recap_partie(SDL_Surface *fenetre, Jeu *partie, Equipe *N_S, Equipe *E_W)
{
    SDL_Surface *img_recap = NULL;
    SDL_Rect pos_recap;
    //Affichage du recap
    img_recap = IMG_Load("Src/recapitulatif_partie.png");
    pos_recap.x = 853;
    pos_recap.y = 521;
    SDL_BlitSurface(img_recap, NULL, fenetre, &pos_recap);
    //affichage des score des equipes pour la partie
    SDL_Surface *texte = NULL;
    SDL_Rect pos_texte;
    TTF_Font *police = TTF_OpenFont("helvetica.ttf", 50);
    SDL_Color noir = {0,0,0};
    SDL_Color blanc = {255,255,255};
    //score des allie
    pos_texte.x = 1081;
    pos_texte.y = 585;
    char array_points[5] = "";
    sprintf(array_points, "%d", N_S->score_total);
    texte = TTF_RenderText_Shaded(police, array_points,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    //score des ennemis
    pos_texte.x = 1081;
    pos_texte.y = 654;
    sprintf(array_points, "%d", E_W->score_total);
    texte = TTF_RenderText_Shaded(police, array_points,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    SDL_FreeSurface(texte);
    TTF_CloseFont(police);
    SDL_FreeSurface(img_recap);
}

void affiche_infos_manche(SDL_Surface *fenetre, Jeu *partie, Equipe *N_S, Equipe *E_W)
{
    SDL_Surface *img_infos_manche = NULL;
    SDL_Rect pos_infos;
    //Affichage du fond
    img_infos_manche = IMG_Load("Src/infos_manche.png");
    pos_infos.x = 704;
    pos_infos.y = 322;
    SDL_BlitSurface(img_infos_manche, NULL, fenetre, &pos_infos);
    //affichage gagnant precedent
    SDL_Surface *texte = NULL;
    SDL_Rect pos_texte;
    TTF_Font *police = TTF_OpenFont("helvetica.ttf", 20);
    SDL_Color noir = {0,0,0};
    SDL_Color blanc = {255,255,255};
    pos_texte.x = 927;
    pos_texte.y = 380;
    texte = TTF_RenderText_Shaded(police, partie->gagnant_precedent,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    //affichage annonce
    pos_texte.x = 825;
    pos_texte.y = 427;
    char array_points[5] = "";
    sprintf(array_points, "%d", partie->annonce_max);
    texte = TTF_RenderText_Shaded(police, array_points,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    //affichage detenteur annonce
    pos_texte.x = 927;
    pos_texte.y = 427;
    texte = TTF_RenderText_Shaded(police, partie->detenteur_annonce_max,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    //affichage detenteur belote rebelote
    pos_texte.x = 925;
    pos_texte.y = 476;
    texte = TTF_RenderText_Shaded(police, partie->detenteur_belote_rebelote,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);

    SDL_FreeSurface(texte);
    TTF_CloseFont(police);
    SDL_FreeSurface(img_infos_manche);
}

void affiche_recap_manche(SDL_Surface *fenetre, Jeu *partie, Equipe *N_S, Equipe *E_W)
{
    SDL_Surface *img_recap = NULL;
    SDL_Rect pos_recap;
    //Affichage du recap
    img_recap = IMG_Load("Src/recapitulatif_manche.png");
    pos_recap.x = 996;
    pos_recap.y = 322;
    SDL_BlitSurface(img_recap, NULL, fenetre, &pos_recap);
    //affichage des score des equipes pour la partie
    SDL_Surface *texte = NULL;
    SDL_Rect pos_texte;
    TTF_Font *police = TTF_OpenFont("helvetica.ttf", 45);
    SDL_Color noir = {0,0,0};
    SDL_Color blanc = {255,255,255};
    //score des allie
    pos_texte.x = 1135;
    pos_texte.y = 391;
    char array_points[5] = "";
    sprintf(array_points, "%d", N_S->score_manche);
    texte = TTF_RenderText_Shaded(police, array_points,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    //score des ennemis
    pos_texte.x = 1135;
    pos_texte.y = 456;
    sprintf(array_points, "%d", E_W->score_manche);
    texte = TTF_RenderText_Shaded(police, array_points,noir,blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
    SDL_FreeSurface(texte);
    TTF_CloseFont(police);
    SDL_FreeSurface(img_recap);
}


void rafraichir_possibilitees(SDL_Surface *fenetre)
{
    SDL_Surface *img_fond = NULL;
    SDL_Rect pos_fond;
    img_fond = IMG_Load("Src/fond_jeu_possibilitees.png");
    pos_fond.x = 696;
    pos_fond.y = 0;
    SDL_BlitSurface(img_fond, NULL, fenetre, &pos_fond);
}

void affiche_carte(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, SDL_Surface *fenetre)
{
    int i = 0;
    SDL_Surface *img_carte = NULL;
    SDL_Rect pos_carte;
    //on va afficher les cartes de tous les joueurs un par un
    i = 0;
    while (i < 8 && strcmp(joueur_S->carte[i], "")!=0)
    {
        char adresse[25] = "";
        quelle_carte(joueur_S->carte[i], adresse);
        img_carte = IMG_Load(adresse);
        if(!img_carte) {
            FILE *error_file;
            error_file = fopen("error_file.txt", "w+");
            fprintf(error_file, "IMG_Load: %s\n", IMG_GetError());
            fclose(error_file);
        }
        pos_carte.x = 56 + 2 * i + 71 * i;
        pos_carte.y = 585;
        SDL_BlitSurface(img_carte, NULL, fenetre, &pos_carte);
        i += 1;
    }
    i = 0;
    while (i < 8 && strcmp(ordi_W->carte[i], "")!=0)
    {
        img_carte = IMG_Load("Src/Cartes/E_W.png");
        pos_carte.x = 32;
        pos_carte.y = 145 + 50 * i;
        SDL_BlitSurface(img_carte, NULL, fenetre, &pos_carte);
        i += 1;
    }
    i = 0;
    while (i < 8 && strcmp(ordi_N->carte[i], "")!=0)
    {
        img_carte = IMG_Load("Src/Cartes/N.png");
        pos_carte.x = 135 + 50 * i;
        pos_carte.y = 39;
        SDL_BlitSurface(img_carte, NULL, fenetre, &pos_carte);
        i += 1;
    }
    i = 0;
    while (i < 8 && strcmp(ordi_E->carte[i], "")!=0)
    {
        img_carte = IMG_Load("Src/Cartes/E_W.png");
        pos_carte.x = 562;
        pos_carte.y = 145 + 50 * i;
        SDL_BlitSurface(img_carte, NULL, fenetre, &pos_carte);
        i += 1;
    }
    SDL_FreeSurface(img_carte);
}

void affiche_table(Jeu *partie, SDL_Surface *fenetre)
{
    SDL_Surface *img_carte = NULL;
    SDL_Rect pos_carte;
    for (int i = 0; i < 4; i += 1)
    {
        if (strcmp(partie->table[i], "")!=0)
        {
            char adresse[25] = "";
            quelle_carte(partie->table[i], adresse);
            img_carte = IMG_Load(adresse);
            switch(i)
            {
                case 0 : pos_carte.x = 248;//W
                            pos_carte.y = 311;
                        break;
                case 1 : pos_carte.x = 309;//N
                            pos_carte.y = 255;
                        break;
                case 2 : pos_carte.x = 375;//E
                            pos_carte.y = 311;
                        break;
                case 3 : pos_carte.x = 309;//S
                            pos_carte.y = 368;
                        break;
            }
            SDL_BlitSurface(img_carte, NULL, fenetre, &pos_carte);
        }
    }
    SDL_FreeSurface(img_carte);
}

int bouton_IA(SDL_Surface *fenetre)
{
    int decision = 1;
    SDL_Surface *bouton = NULL;
    SDL_Rect pos_bouton;
    bouton = IMG_Load("Src/Bouton_ia.png");
    pos_bouton.x = 704;
    pos_bouton.y = 64;
    SDL_BlitSurface(bouton, NULL, fenetre, &pos_bouton);
    SDL_Flip(fenetre);
    SDL_Event event;

    while (decision)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT : return EXIT_FAILURE;
                break;
            case SDL_MOUSEBUTTONDOWN :
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (event.button.x > 712 && event.button.x < 1257 && event.button.y > 64 && event.button.y < 160) //si il appuie sur le bouton encherir
                    {
                        decision = 0;
                    }
                }
                break;
        }
    }
    SDL_FreeSurface(bouton);
    return EXIT_SUCCESS;
}

int bouton_relancer(SDL_Surface *fenetre)
{
    int decision = 1;
    SDL_Surface *bouton = NULL;
    SDL_Rect pos_bouton;
    bouton = IMG_Load("Src/Bouton_relancer.png");
    pos_bouton.x = 704;
    pos_bouton.y = 64;
    SDL_BlitSurface(bouton, NULL, fenetre, &pos_bouton);
    SDL_Flip(fenetre);
    SDL_Event event;

    while (decision)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT : return EXIT_FAILURE;
                break;
            case SDL_MOUSEBUTTONDOWN :
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (event.button.x > 712 && event.button.x < 1257 && event.button.y > 64 && event.button.y < 160) //si il appuie sur le bouton encherir
                    {
                        decision = 0;
                    }
                }
                break;
        }
    }
    SDL_FreeSurface(bouton);
    return EXIT_SUCCESS;
}

int bouton_continuer(SDL_Surface *fenetre)
{
    int decision = 1;
    SDL_Surface *bouton = NULL;
    SDL_Rect pos_bouton;
    bouton = IMG_Load("Src/Bouton_continuer.png");
    pos_bouton.x = 704;
    pos_bouton.y = 64;
    SDL_BlitSurface(bouton, NULL, fenetre, &pos_bouton);
    SDL_Flip(fenetre);
    SDL_Event event;

    while (decision)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT : return EXIT_FAILURE;
                break;
            case SDL_MOUSEBUTTONDOWN :
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (event.button.x > 712 && event.button.x < 1257 && event.button.y > 64 && event.button.y < 160) //si il appuie sur le bouton encherir
                    {
                        decision = 0;
                    }
                }
                break;
        }
    }
    SDL_FreeSurface(bouton);
    return EXIT_SUCCESS;
}

void bouton_debut_manche(SDL_Surface *fenetre)
{
    int decision = 1;
    SDL_Surface *bouton = NULL;
    SDL_Rect pos_bouton;
    bouton = IMG_Load("Src/Bouton_debut_manche.png");
    pos_bouton.x = 704;
    pos_bouton.y = 64;
    SDL_BlitSurface(bouton, NULL, fenetre, &pos_bouton);
    SDL_Flip(fenetre);
    SDL_Event event;

    while (decision)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT : return EXIT_FAILURE;
                break;
            case SDL_MOUSEBUTTONDOWN :
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (event.button.x > 712 && event.button.x < 1257 && event.button.y > 64 && event.button.y < 160) //si il appuie sur le bouton encherir
                    {
                        decision = 0;
                    }
                }
                break;
        }
    }
    SDL_FreeSurface(bouton);
}

void quelle_carte(char carte[5], char *adresse) //donne l'image correspondante à chaque carte
{
    if (carte[2]=='T')
    {
        switch(carte[0])
        {
            case '7' : strcpy(adresse,"Src/Cartes/7 T.jpg");
                break;
            case '8' : strcpy(adresse,"Src/Cartes/8 T.jpg");
                break;
            case '9' : strcpy(adresse,"Src/Cartes/9 T.jpg");
                break;
            case '1' : strcpy(adresse,"Src/Cartes/10T.jpg");
                break;
            case 'V' : strcpy(adresse,"Src/Cartes/V T.jpg");
                break;
            case 'Q' : strcpy(adresse,"Src/Cartes/Q T.jpg");
                break;
            case 'R' : strcpy(adresse,"Src/Cartes/R T.jpg");
                break;
            case 'A' : strcpy(adresse,"Src/Cartes/A T.jpg");
                break;
        }
    }
    else if (carte[2] == 'P')
    {
        switch(carte[0])
        {
            case '7' : strcpy(adresse,"Src/Cartes/7 P.jpg");
                break;
            case '8' : strcpy(adresse,"Src/Cartes/8 P.jpg");
                break;
            case '9' : strcpy(adresse,"Src/Cartes/9 P.jpg");
                break;
            case '1' : strcpy(adresse,"Src/Cartes/10P.jpg");
                break;
            case 'V' : strcpy(adresse,"Src/Cartes/V P.jpg");
                break;
            case 'Q' : strcpy(adresse,"Src/Cartes/Q P.jpg");
                break;
            case 'R' : strcpy(adresse,"Src/Cartes/R P.jpg");
                break;
            case 'A' : strcpy(adresse,"Src/Cartes/A P.jpg");
                break;
        }
    }
    else if (strncmp(&carte[2],"Ca", 2) == 0)
    {
        switch(carte[0])
        {
            case '7' : strcpy(adresse,"Src/Cartes/7 Ca.jpg");
                break;
            case '8' : strcpy(adresse,"Src/Cartes/8 Ca.jpg");
                break;
            case '9' : strcpy(adresse,"Src/Cartes/9 Ca.jpg");
                break;
            case '1' : strcpy(adresse,"Src/Cartes/10Ca.jpg");
                break;
            case 'V' : strcpy(adresse,"Src/Cartes/V Ca.jpg");
                break;
            case 'Q' : strcpy(adresse,"Src/Cartes/Q Ca.jpg");
                break;
            case 'R' : strcpy(adresse,"Src/Cartes/R Ca.jpg");
                break;
            case 'A' : strcpy(adresse,"Src/Cartes/A Ca.jpg");
                break;
        }
    }
    else if (strncmp(&carte[2],"Co", 2) == 0)
    {
        switch(carte[0])
        {
            case '7' : strcpy(adresse,"Src/Cartes/7 Co.jpg");
                break;
            case '8' : strcpy(adresse,"Src/Cartes/8 Co.jpg");
                break;
            case '9' : strcpy(adresse,"Src/Cartes/9 Co.jpg");
                break;
            case '1' : strcpy(adresse,"Src/Cartes/10Co.jpg");
                break;
            case 'V' : strcpy(adresse,"Src/Cartes/V Co.jpg");
                break;
            case 'Q' : strcpy(adresse,"Src/Cartes/Q Co.jpg");
                break;
            case 'R' : strcpy(adresse,"Src/Cartes/R Co.jpg");
                break;
            case 'A' : strcpy(adresse,"Src/Cartes/A Co.jpg");
                break;
        }
    }
    else EXIT_FAILURE;
}

void selection_joueur(SDL_Surface *fenetre, char position) //affiche un cercle autour de la personne qui doit jouer
{
    SDL_Surface *cercle;
    SDL_Rect pos_cercle;
    cercle = IMG_Load("Src/Select.png");
    switch (position)
    {
        case 'N' : pos_cercle.x = 301;
                pos_cercle.y = 182;
            break;
        case 'S' : pos_cercle.x = 300;
                pos_cercle.y = 448;
            break;
        case 'W' : pos_cercle.x = 175;
                pos_cercle.y = 315;
            break;
        case 'E' : pos_cercle.x = 434;
                pos_cercle.y = 316;
            break;
    }
    SDL_BlitSurface(cercle, NULL, fenetre, &pos_cercle);
    SDL_Flip(fenetre);
    SDL_FreeSurface(cercle);
}

int recup_nom(Joueur *joueur_S, Jeu *partie, SDL_Surface *fenetre, Scores tableau_scores[])
{
    int decision = 1;
    SDL_Surface *nom = NULL;
    SDL_Rect pos_nom;
    nom = IMG_Load("Src/fond.jpg");
    pos_nom.x = 0;
    pos_nom.y = 0;
    SDL_BlitSurface(nom, NULL, fenetre, &pos_nom);
    SDL_Flip(fenetre);

    nom = IMG_Load("Src/recup_nom.png");
    pos_nom.x = 640 - nom->w/2;
    pos_nom.y = 360 - nom->h/2;
    SDL_BlitSurface(nom, NULL, fenetre, &pos_nom);

    nom = IMG_Load("Src/reset.png");
    pos_nom.x = 843;
    pos_nom.y = 362;
    SDL_BlitSurface(nom, NULL, fenetre, &pos_nom);
    SDL_Flip(fenetre);

    SDL_Surface *bouton = NULL;
    SDL_Rect pos_bouton;
    bouton = IMG_Load("Src/Bouton_valider.png");
    pos_bouton.x = 640 - bouton->w/2;
    pos_bouton.y = 500;
    SDL_BlitSurface(bouton, NULL, fenetre, &pos_bouton);
    SDL_Flip(fenetre);

    SDL_Surface *texte = NULL;
    SDL_Rect pos_texte;
    TTF_Font *police = TTF_OpenFont("helvetica.ttf", 55);
    SDL_Color noir = {0,0,0};
    SDL_Color blanc = {255,255,255};

    SDL_Event event;
    SDL_EnableUNICODE(1);
    char lettre = NULL;

    while (decision)
    {
        pos_texte.x = 395;
        pos_texte.y = 365;
        texte = TTF_RenderText_Shaded(police, joueur_S->vrai_nom, noir, blanc);
        SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
        SDL_Flip(fenetre);

        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT : return EXIT_FAILURE;
                break;

            case SDL_KEYDOWN:
                lettre = event.key.keysym.unicode;
                if (((lettre >= 'A' && lettre <= 'z') || (lettre >= '0' && lettre <= '9')) && strlen(joueur_S->vrai_nom) < 10)
                {
                    sprintf(joueur_S->vrai_nom, "%s%c", joueur_S->vrai_nom, lettre); //on va recuperer lettre par lettre et on va verifier si c'est bien une lettre, au quel cas on l'ajoute à la chaine de carartère qui va consituer le nom
                }
                break;

            case SDL_MOUSEBUTTONDOWN :
                if (event.button.button == SDL_BUTTON_LEFT && strlen(joueur_S->vrai_nom)>0) //pour passer a l'etape suivante il faut qu'il ait au moins tape un caractere
                {
                    if (event.button.x > 356 && event.button.x < 924 && event.button.y > 500 && event.button.y < 646) //si il appuie sur le bouton valider
                    {
                        decision = 0;
                    }
                    else if (event.button.x > 843 && event.button.x < 902 && event.button.y > 362 && event.button.y < 414) //si il appuie sur le bouton reset, on va nettoyer la chaine de caratere qui fait le nom
                    {
                        strcpy(joueur_S->vrai_nom,"");
                        nom = IMG_Load("Src/recup_nom.png");
                        pos_nom.x = 640 - nom->w/2;
                        pos_nom.y = 360 - nom->h/2;
                        SDL_BlitSurface(nom, NULL, fenetre, &pos_nom);

                        nom = IMG_Load("Src/reset.png");
                        pos_nom.x = 843;
                        pos_nom.y = 362;
                        SDL_BlitSurface(nom, NULL, fenetre, &pos_nom);
                        SDL_Flip(fenetre);
                    }
                }
                break;
        }
    }
    int existe = 0; //retient si le joueur existait deja avant ou pas
    char evenement[50] = ""; //stcoke du texte pour l'afficher

    existe = chargement_tableau_scores(tableau_scores, joueur_S);

    if (existe == 1)
    {
        sprintf(evenement, "Rebonjour pour votre %dème partie %s",tableau_scores[joueur_S->pos_score].nb_parties + 1, joueur_S->vrai_nom);
    }
    else
    {
        sprintf(evenement, "Bonjour pour votre premiere partie %s",joueur_S->vrai_nom);
    }
    pos_texte.x = 110;
    pos_texte.y = 100;
    texte = TTF_RenderText_Shaded(police, evenement, noir, blanc);
    SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);

    bouton = IMG_Load("Src/Bouton_continuer.png");
    SDL_BlitSurface(bouton, NULL, fenetre, &pos_bouton);
    SDL_Flip(fenetre);

    decision = 1;
    while (decision)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT : return EXIT_FAILURE;
                break;
            case SDL_MOUSEBUTTONDOWN :
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (event.button.x > 356 && event.button.x < 924 && event.button.y > 500 && event.button.y < 646) //si il appuie sur le bouton continuer
                    {
                        decision = 0;
                    }
                }
                break;
        }
    }

    SDL_FreeSurface(nom);
    SDL_FreeSurface(bouton);
    TTF_CloseFont(police);

    return EXIT_SUCCESS;
}

int chargement_tableau_scores(Scores tableau_scores[], Joueur *joueur_S)
{
    FILE *fichier_scores = fopen("score.txt", "r");
    int i = 0, k = 0, existe = 0;
	if(fichier_scores != NULL) //  si le fichier a bien ete ouvert
    {
		do {
			fscanf(fichier_scores, "%s ;%d ;%d ;%d ;", tableau_scores[i].nom, &tableau_scores[i].nb_parties, &tableau_scores[i].nb_victoires, &tableau_scores[i].meilleur_score);
			i++;
		} while (fgetc(fichier_scores) != EOF && i < 20); //on va completer notre tableau qui contient les scores avec le fichier jusqu'a arriver a la fin du fichier
		fclose(fichier_scores);
	}
	else
	{
		fprintf(stderr,"le fichier n'existe pas");
		return EXIT_FAILURE;
	}
	while (k < i && existe == 0)
    {
        if (strcmp(tableau_scores[k].nom, joueur_S->vrai_nom)==0)
        {
            existe = 1;
        }
        else
        {
            k += 1;
        }
    }

    if (existe == 1)
    {
        joueur_S->pos_score = k; //permet de savoir a quel position notre joueur actuel se trouve dans le tableau
    }
    else
    {
        if (i == 20)
        {
            fprintf(stderr,"Plus de place dispo");
            return EXIT_FAILURE;
        }
        else
        {
            joueur_S->pos_score = i;//permet de savoir a quel position notre joueur actuel se trouve dans le tableau
        }
    }

    fclose(fichier_scores);
    return existe;
}

void actualisation_scores(Scores tableau_scores[], Joueur *joueur_S, Jeu *partie, Equipe *N_S, Equipe *E_W)
{
    FILE *fichier_scores = fopen("score.txt", "w");
    int i = 0, modification = 0, nb_joueur = 0;
    //on rempli les information du joueur dans le tableau
    strcpy(tableau_scores[joueur_S->pos_score].nom, joueur_S->vrai_nom);
    tableau_scores[joueur_S->pos_score].nb_parties += 1; //on lui augmente son nombre de partie
    if (N_S->score_total > E_W->score_total) //on s'il a gagne si oui on lui augmente son nombre de victoire
    {
        tableau_scores[joueur_S->pos_score].nb_victoires += 1;
    }

    if (N_S->score_total - E_W->score_total > tableau_scores[joueur_S->pos_score].meilleur_score)//on verifie si son score est meilleur que son ancien, si oui on lui met son nouveau score
    {
        tableau_scores[joueur_S->pos_score].meilleur_score = N_S->score_total - E_W->score_total;
    }

    //on compte le nombre de joueur
    while (strcmp(tableau_scores[nb_joueur].nom, "")!=0)
    {
        nb_joueur += 1;
    }
    //on reclasse le tableau par ordre croissant des scores avec un tri a bulles
    Scores temp;
    do
    {
        modification = 0;
        for (int k = 0; k < nb_joueur-1; k += 1)
        {
            if (tableau_scores[k+1].meilleur_score > tableau_scores[k].meilleur_score)
            {
                modification = 1;
                temp = tableau_scores[k+1];
                tableau_scores[k+1] = tableau_scores[k];
                tableau_scores[k] = temp;
            }
        }
    } while (modification != 0);

    //on actualise le fichier
    while (strcmp(tableau_scores[i].nom, "")!=0)
    {
        if (i != 0)
        {
            fprintf(fichier_scores, "\n");
        }

        fprintf(fichier_scores,"%s ;%d ;%d ;%d ;", tableau_scores[i].nom, tableau_scores[i].nb_parties, tableau_scores[i].nb_victoires, tableau_scores[i].meilleur_score);
        i += 1;
    }


    fclose(fichier_scores);
}

void flush_variables_manche(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Equipe *N_S, Equipe *E_W, Jeu *partie)
{
    partie->contrat = 79;
    strcpy(partie->type_contrat, "");
    strcpy(partie->detenteur_contrat, "");
    strcpy(partie->atout, "");
    strcpy(partie->equipe_preneuse, "");
    strcpy(partie->gagnant_precedent, "");
    strcpy(partie->carte_maitresse, "");
    partie->tour = 0;
    strcpy(partie->detenteur_carte_maitresse, "");
    partie->score_pli = 0;
    partie->pli = 0;
    partie->compteur_belote_rebelote = 0;
    partie->annonce_max = 0;
    strcpy(partie->detenteur_annonce_max,"");
    strcpy(partie->quelle_annonce_max,"");
    strcpy(partie->evenements_ligne_1, "");
    partie->surencherir = 0;
    strcpy(partie->detenteur_belote_rebelote, "");
    strcpy(partie->atout_couleur_restant[0],"V");
    strcpy(partie->atout_couleur_restant[1],"9");
    strcpy(partie->atout_couleur_restant[2],"A");
    strcpy(partie->atout_couleur_restant[3],"1");
    strcpy(partie->atout_couleur_restant[4],"R");
    strcpy(partie->atout_couleur_restant[5],"Q");
    strcpy(partie->atout_couleur_restant[6],"8");
    strcpy(partie->atout_couleur_restant[7],"7");
    strcpy(partie->atout_restant[0],"T");
    strcpy(partie->atout_restant[1],"Co");
    strcpy(partie->atout_restant[2],"P");
    strcpy(partie->atout_restant[3],"Ca");
    ordi_W->contrat = 0;
    ordi_W->possibilite = 0;
    ordi_W->nb_possibilite_carte = 0;
    strcpy(ordi_W->contrat_atout, "");
    ordi_W->annonce = 0;
    ordi_W->score_perso = 0;

    ordi_E->contrat = 0;
    ordi_E->possibilite = 0;
    ordi_E->nb_possibilite_carte = 0;
    strcpy(ordi_E->contrat_atout, "");
    ordi_E->annonce = 0;
    ordi_E->score_perso = 0;

    ordi_N->contrat = 0;
    ordi_N->possibilite = 0;
    ordi_N->nb_possibilite_carte = 0;
    strcpy(ordi_N->contrat_atout, "");
    ordi_N->annonce = 0;
    ordi_N->score_perso = 0;

    joueur_S->contrat = 0;
    joueur_S->possibilite = 0;
    joueur_S->nb_possibilite_carte = 0;
    strcpy(joueur_S->contrat_atout, "");
    joueur_S->annonce = 0;
    joueur_S->score_perso = 0;

    E_W->annonce = 0;
    E_W->nb_pli = 0;
    E_W->score_manche = 0;
    E_W->belote_rebelote = 0;

    N_S->annonce = 0;
    N_S->nb_pli = 0;
    N_S->score_manche = 0;
    N_S->belote_rebelote = 0;

}
