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

int contrat(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, SDL_Surface *fenetre, Equipe *N_S, Equipe *E_W) //gere les enchères
{
    int ordre = partie->ordre_de_jeu; //on décide qui commence
    int exit = 0;
    int occurence = 0;
    int changement = 0; //booleen 0 si non, 1 si oui
    char historique_enchere[5] = ""; //stocke l'historique des personnes qui ont passe pour les afficher, le 5 eme stocke celui qui a l'enchere

    //printf("Phase d'enchere des contrats\n\n");

    do {
        switch(ordre) {
            case 0 : selection_joueur(fenetre, 'W'); //met le rond autour de la personne qui doit jouer
                exit = bouton_IA(fenetre);//met en pause et met un bouton
                changement = choix(ordi_W, partie, fenetre, historique_enchere); //fait encherir un joueur puis le suivant
                break;
            case 1 : selection_joueur(fenetre, 'N');
                exit = bouton_IA(fenetre);
                changement = choix(ordi_N, partie, fenetre, historique_enchere);
                break;
            case 2 : selection_joueur(fenetre, 'E');
                exit = bouton_IA(fenetre);
                changement = choix(ordi_E, partie, fenetre, historique_enchere);
                break;
            case 3 : selection_joueur(fenetre, 'S');
                changement = choix(joueur_S, partie, fenetre, historique_enchere);
                break;
        }
        if (exit == 1)
        {
            return EXIT_FAILURE;
        }
        ordre += 1 ;
        if (ordre == 4)
        {
            ordre -= 4;
        }

        if (changement == 0) //on remet à un si il y a eu une nouvele enchere
        {
            occurence += 1;
        }
        else if (changement == 10) //il a quitte
        {
            return EXIT_FAILURE;
        }
        else //signifie qu'il y a eu une enchere
        {
            occurence = 1;
        }

    rafraichir(fenetre, partie, N_S, E_W, ordi_W, ordi_N, ordi_E, joueur_S);
    affiche_indication_enchere(fenetre, changement, partie->detenteur_contrat[0], historique_enchere);
    } while (occurence < 4);

    return EXIT_SUCCESS;
}

void affiche_indication_enchere(SDL_Surface *fenetre, int changement, char nom, char *historique_enchere)
{
    SDL_Surface *img_indication = NULL;
    SDL_Rect pos_indication;
    if (changement == 0) //pas d'enchere
    {
        img_indication = IMG_Load("Src/Contrats/Passe_indication.png");
        int i = 0;
        while (i < 4 && historique_enchere[i] != '\0')
        {
            switch (historique_enchere[i])
            {
                case 'N' : pos_indication.x = 300;
                    pos_indication.y = 145;
                    break;
                case 'W' : pos_indication.x = 138;
                    pos_indication.y = 422;
                    break;
                case 'S' : pos_indication.x = 300;
                    pos_indication.y = 552;
                    break;
                case 'E' : pos_indication.x = 476;
                    pos_indication.y = 280;
                    break;
            }
            SDL_BlitSurface(img_indication, NULL, fenetre, &pos_indication);
            i += 1;
        }

    }
    else //si il y a eu une nouvelle enchere
    {
        strcpy(historique_enchere, ""); //vide l'historique
    }
    if (nom > 'A' && nom < 'Z')
    {
        img_indication = IMG_Load("Src/Contrats/Enchere_indication.png");
        switch (nom)
        {
            case 'N' : pos_indication.x = 300;
                pos_indication.y = 145;
                break;
            case 'W' : pos_indication.x = 138;
                pos_indication.y = 422;
                break;
            case 'S' : pos_indication.x = 300;
                pos_indication.y = 552;
                break;
            case 'E' : pos_indication.x = 476;
                pos_indication.y = 280;
                break;
        }
    }
    SDL_BlitSurface(img_indication, NULL, fenetre, &pos_indication);

    SDL_Flip(fenetre);
    SDL_FreeSurface(img_indication);

}

int verification_des_contrats(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, Equipe *N_S, Equipe *E_W, SDL_Surface *fenetre)
{
    //verification des scores par rapport aux contrats
    int exit = 0;

    if (strcmp(partie->type_contrat, "GE")==0)
    {
        if (strcmp(partie->equipe_preneuse, "N_S")==0 ) //si c'est l'équie N_S qui détient le contrat
        {
                if (ordi_N->score_perso == 162 || joueur_S->score_perso == 162) // il a reussi
                {
                    N_S->score_manche += N_S->annonce + N_S->belote_rebelote + partie->contrat;

                    N_S->score_total += N_S->score_manche;

                    E_W->score_manche += E_W->annonce + E_W->belote_rebelote;

                    E_W->score_total += E_W->score_manche;
                }
                else //il a rate
                {
                    N_S->score_manche = N_S->belote_rebelote;

                    N_S->score_total += N_S->score_manche;

                    E_W->score_manche = 162 + partie->contrat + E_W->annonce + N_S->annonce + E_W->belote_rebelote;

                    E_W->score_total += E_W->score_manche;
                }
        }
        else //si c'est l'équipe E_W qui détient le contrat
        {
            if (ordi_E->score_perso == 162 || ordi_W->score_perso == 162) // il a reussi
            {
                E_W->score_manche += E_W->annonce + E_W->belote_rebelote + partie->contrat;

                E_W->score_total += E_W->score_manche;

                N_S->score_manche += N_S->annonce + N_S->belote_rebelote;

                N_S->score_total += N_S->score_manche;
            }
            else //il a rate
            {
                E_W->score_manche = E_W->belote_rebelote;

                E_W->score_total += E_W->score_manche;

                N_S->score_manche = 162 + partie->contrat + N_S->annonce + E_W->annonce + N_S->belote_rebelote;

                N_S->score_total += N_S->score_manche;
            }
        }
    }
    else if (strcmp(partie->type_contrat, "GE")==0)
    {
        if (strcmp(partie->equipe_preneuse, "N_S")==0 ) //si c'est l'équie N_S qui détient le contrat
        {
                if (N_S->nb_pli == 8)// il a reussi
                {
                    N_S->score_manche += N_S->annonce + N_S->belote_rebelote + partie->contrat;

                    N_S->score_total += N_S->score_manche;

                    E_W->score_manche += E_W->annonce + E_W->belote_rebelote;

                    E_W->score_total += E_W->score_manche;
                }
                else //il a rate
                {
                    N_S->score_manche = N_S->belote_rebelote;

                    N_S->score_total += N_S->score_manche;

                    E_W->score_manche = 162 + partie->contrat + E_W->annonce + N_S->annonce + E_W->belote_rebelote;

                    E_W->score_total += E_W->score_manche;
                }
        }
        else //si c'est l'équipe E_W qui détient le contrat
        {
            if (E_W->nb_pli == 8) // il a reussi
            {
                E_W->score_manche += E_W->annonce + E_W->belote_rebelote + partie->contrat;

                E_W->score_total += E_W->score_manche;

                N_S->score_manche += N_S->annonce + N_S->belote_rebelote;

                N_S->score_total += N_S->score_manche;
            }
            else //il a rate
            {
                E_W->score_manche = E_W->belote_rebelote;

                E_W->score_total += E_W->score_manche;

                N_S->score_manche = 162 + partie->contrat + N_S->annonce + E_W->annonce + N_S->belote_rebelote;

                N_S->score_total += N_S->score_manche;
            }
        }
    }
    else //contrat classique
    {
        if (strcmp(partie->equipe_preneuse, "N_S")==0 ) //si c'est l'équie N_S qui détient le contrat
        {
            if ( (N_S->score_manche + N_S->annonce + N_S->belote_rebelote >= partie->contrat) && N_S->score_manche >= 80 ) //si le contrat est rempli et 82 sans annonce atteint
            {
                N_S->score_manche += N_S->annonce + N_S->belote_rebelote + partie->contrat;

                N_S->score_total += N_S->score_manche;

                E_W->score_manche += E_W->annonce + E_W->belote_rebelote;

                E_W->score_total += E_W->score_manche;
            }
            else //si le contrat est raté
            {
                N_S->score_manche = N_S->belote_rebelote;

                N_S->score_total += N_S->score_manche;

                E_W->score_manche = 162 + partie->contrat + E_W->annonce + N_S->annonce + E_W->belote_rebelote;

                E_W->score_total += E_W->score_manche;
            }
        }
        else //si c'est l'équipe E_W qui détient le contrat
        {
            if ( (E_W->score_manche + E_W->annonce + E_W->belote_rebelote >= partie->contrat) && E_W->score_manche >= 80) //si le contrat est rempli et 82 sans annonce atteint
            {
                E_W->score_manche += E_W->annonce + E_W->belote_rebelote + partie->contrat;

                E_W->score_total += E_W->score_manche;

                N_S->score_manche += N_S->annonce + N_S->belote_rebelote;

                N_S->score_total += N_S->score_manche;
            }
            else
            {
                E_W->score_manche = E_W->belote_rebelote;

                E_W->score_total += E_W->score_manche;

                N_S->score_manche = 162 + partie->contrat + N_S->annonce + E_W->annonce + N_S->belote_rebelote;

                N_S->score_total += N_S->score_manche;
            }
        }
    }


    //affichage des resultats pour la manche et rappel des scores totaux
    strcpy(partie->evenements_ligne_1, "   * Recapitulatif *");
    char evenement[64] = "";
    sprintf(evenement,"N_S remporte %d pli et %d points sur la manche", N_S->nb_pli, N_S->score_manche);
    affiche_evenements(fenetre,partie,evenement,"");
    sprintf(evenement,"E_W remporte %d pli et %d points sur la manche", E_W->nb_pli, E_W->score_manche);
    affiche_evenements(fenetre,partie,"",evenement);
    exit = bouton_continuer(fenetre);
    return exit;
}

int choix(Joueur *j, Jeu *partie, SDL_Surface *fenetre, char historique_enchere[5]) //gere les différentes possibilite de contrat
{
    int changement = 0;
    int surenchere = 0;

    if (j->nom[0] == 'S') //si le jouer est reel ou ordinateur
        {
            int decision = 1;
            SDL_Surface *carte_contrat = NULL;
            SDL_Rect pos_carte_contrat;
            //on affiche les cartes enchere ou passer
            carte_contrat = IMG_Load("Src/Contrats/Encherir.png");
            pos_carte_contrat.x = 697;
            pos_carte_contrat.y = 64;
            SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
            carte_contrat = IMG_Load("Src/Contrats/Passer.png");
            pos_carte_contrat.x = 912;
            pos_carte_contrat.y = 64;
            SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
            SDL_Flip(fenetre);
            SDL_Event event;

            while (decision == 1)
            {
                SDL_WaitEvent(&event);
                switch(event.type)
                {
                    case SDL_QUIT : return 10;
                        break;
                    case SDL_MOUSEBUTTONDOWN :
                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            if (event.button.x > 697 && event.button.x < 910 && event.button.y >64 && event.button.y < 160) //si il appuie sur le bouton encherir
                            {
                                decision = 2;
                            }
                            else if (event.button.x > 912 && event.button.x <1125 && event.button.y >64 && event.button.y < 160) //si il appuie sur le bouton passer
                            {
                                decision = 3;
                            }
                        }
                        break;
                }
            }

            if (decision == 2)
            {
                rafraichir_possibilitees(fenetre);
                carte_contrat = IMG_Load("Src/Contrats/Classique.png");
                pos_carte_contrat.x = 697;
                pos_carte_contrat.y = 64;
                SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                carte_contrat = IMG_Load("Src/Contrats/Capot.png");
                pos_carte_contrat.x = 841;
                pos_carte_contrat.y = 64;
                SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                carte_contrat = IMG_Load("Src/Contrats/General.png");
                pos_carte_contrat.x = 985;
                pos_carte_contrat.y = 64;
                SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                if (partie->contrat > 79 && partie->surencherir == 0)
                {
                    carte_contrat = IMG_Load("Src/Contrats/Surencherir.png");
                    pos_carte_contrat.x = 1129;
                    pos_carte_contrat.y = 64;
                    SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                }
                SDL_Flip(fenetre);
                SDL_WaitEvent(&event);
                decision = 1; //reinitialise la decision
                while (decision == 1)
                {
                    SDL_WaitEvent(&event);
                    switch(event.type)
                    {
                        case SDL_QUIT : return 10;
                            break;
                        case SDL_MOUSEBUTTONDOWN :
                            if (event.button.button == SDL_BUTTON_LEFT)
                            {
                                if (event.button.x > 697 && event.button.x < 839 && event.button.y >64 && event.button.y < 160) //si il appuie sur le bouton classique
                                {
                                    decision = 0;
                                    strcpy(partie->type_contrat, "CL");
                                }
                                else if (event.button.x > 841 && event.button.x <983 && event.button.y >64 && event.button.y < 160) //si il appuie sur le bouton capot
                                {
                                    decision = 0;
                                    strcpy(partie->type_contrat, "CA");
                                }
                                else if (event.button.x > 985 && event.button.x <1127 && event.button.y >64 && event.button.y < 160) //si il appuie sur le bouton general
                                {
                                    decision = 0;
                                    strcpy(partie->type_contrat, "GE");
                                }
                                else if (event.button.x > 1130 && event.button.x <1274 && event.button.y >64 && event.button.y < 160) //si il appuie sur le bouton surencherir
                                {
                                    decision = 0;
                                    partie->surencherir = 1;
                                    strcpy(partie->detenteur_contrat, j->nom);
                                    partie->contrat += 10;
                                    strcpy(j->contrat_atout, partie->atout);
                                    goto fin;
                                }
                            }
                    }
                }
                //on lui demande la couleur à laquelle il souhaite partir
                rafraichir_possibilitees(fenetre);
                carte_contrat = IMG_Load("Src/Contrats/Atout_T.jpg");
                pos_carte_contrat.x = 697;
                pos_carte_contrat.y = 64;
                SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                carte_contrat = IMG_Load("Src/Contrats/Atout_Ca.jpg");
                pos_carte_contrat.x = 770;
                pos_carte_contrat.y = 64;
                SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                carte_contrat = IMG_Load("Src/Contrats/Atout_P.jpg");
                pos_carte_contrat.x = 843;
                pos_carte_contrat.y = 64;
                SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                carte_contrat = IMG_Load("Src/Contrats/Atout_Co.jpg");
                pos_carte_contrat.x = 916;
                pos_carte_contrat.y = 64;
                SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                carte_contrat = IMG_Load("Src/Contrats/SA.png");
                pos_carte_contrat.x = 989;
                pos_carte_contrat.y = 64;
                SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                carte_contrat = IMG_Load("Src/Contrats/TA.png");
                pos_carte_contrat.x = 1062;
                pos_carte_contrat.y = 64;
                SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                SDL_Flip(fenetre);
                SDL_WaitEvent(&event);
                decision = 1;
                while (decision)
                {
                SDL_WaitEvent(&event);
                switch(event.type)
                    {
                        case SDL_QUIT : return 10;
                            break;
                        case SDL_MOUSEBUTTONDOWN :
                            if (event.button.button == SDL_BUTTON_LEFT)
                            {
                                if (event.button.y >64 && event.button.y < 160)
                                {//on calcul sur quelle case il a clique sachant que la premiere carte commence en 697 et que toutes les cartes sont entre 64 et 160 en y
                                    int numero = (event.button.x-696)/73; //on prend 1 de moins pour prendre l'espacement enrte les cartes de 2 px en compte
                                    switch (numero)
                                    {
                                        case 0 : decision = 0;
                                                strcpy(partie->atout, "T");
                                            break;
                                        case 1 : decision = 0;
                                                strcpy(partie->atout, "Ca");
                                            break;
                                        case 2 : decision = 0;
                                                strcpy(partie->atout, "P");
                                            break;
                                        case 3 : decision = 0;
                                                strcpy(partie->atout, "Co");
                                            break;
                                        case 4 : decision = 0;
                                                strcpy(partie->atout, "SA");
                                            break;
                                        case 5 : decision = 0;
                                                strcpy(partie->atout, "TA");
                                            break;
                                    }
                                }
                            }
                    }
                }
                //on lui demande a combien il souhaite encherir
                if (strcmp(partie->type_contrat, "CL")==0) //si le contrat est classique il choisit cb il veut encherir
                {
                    rafraichir_possibilitees(fenetre);
                    carte_contrat = IMG_Load("Src/Contrats/Enchere.png");
                    pos_carte_contrat.x = 712;
                    pos_carte_contrat.y = 64;
                    SDL_BlitSurface(carte_contrat, NULL, fenetre, &pos_carte_contrat);
                    SDL_Flip(fenetre);
                    SDL_WaitEvent(&event);
                    decision = 1;
                    //initialise le texte du compteur
                    SDL_Surface *texte = NULL;
                    SDL_Rect pos_texte;
                    pos_texte.x = 925;
                    pos_texte.y = 80;
                    TTF_Font *police = TTF_OpenFont("helvetica.ttf", 75);
                    SDL_Color noir = {0,0,0};
                    SDL_Color blanc = {255,255,255};
                    //initialise la valeur du compteur de l'enchère
                    int compteur = 80;
                    while (compteur <= partie->contrat) //on initialise à la première valeur possible
                    {
                        compteur += 10;
                    }
                    while (decision)
                    {
                        char array_compteur[5] = "";
                        sprintf(array_compteur, "%d", compteur);
                        texte = TTF_RenderText_Shaded(police, array_compteur,noir,blanc);
                        SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
                        SDL_Flip(fenetre);

                        SDL_WaitEvent(&event);
                        switch(event.type)
                        {
                            case SDL_QUIT : return 10;
                                break;
                            case SDL_MOUSEBUTTONDOWN :
                                if (event.button.button == SDL_BUTTON_LEFT)
                                {
                                    if (event.button.x > 728 && event.button.x < 892 && event.button.y > 105 && event.button.y < 155) //si il appuie sur le bouton valider
                                    {
                                        decision = 0;
                                        partie->contrat = compteur;
                                    }
                                    else if (event.button.x > 1093 && event.button.x <1173 && event.button.y > 68 && event.button.y < 155) //si il appuie sur le bouton -
                                    {
                                        if (compteur - 10 > partie->contrat) //on vérifie qu'on peut bien soustraire
                                        {
                                            compteur -= 10;
                                            //clean la zone du compteur
                                            SDL_FillRect(texte, NULL,SDL_MapRGB(texte->format, 255, 255, 255));
                                            SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
                                            SDL_Flip(fenetre);

                                        }
                                    }
                                    else if (event.button.x > 1179 && event.button.x <1257 && event.button.y > 68 && event.button.y < 155) //si il appuie sur le bouton +
                                    {
                                        compteur += 10; //on ajoute 10
                                        //clean la zone du compteur
                                        SDL_FillRect(texte, NULL,SDL_MapRGB(texte->format, 255, 255, 255));
                                        SDL_BlitSurface(texte, NULL, fenetre, &pos_texte);
                                        SDL_Flip(fenetre);
                                    }
                                }
                        }
                    }
                    SDL_FreeSurface(texte);
                    TTF_CloseFont(police);

                }
                else if(strcmp(partie->type_contrat, "CA")==0)//si il fait capot c'est 250 points
                {
                    partie->contrat = 250;
                    partie->surencherir = 1; //on ne veut pas que N surencherisse dessus
                }
                else if(strcmp(partie->type_contrat, "GE")==0)//si il fait general c'est 500 points
                {
                    partie->contrat = 500;
                    partie->surencherir = 1; //on ne veut pas que N surencherisse dessus
                }
            fin :
                strcpy(partie->detenteur_contrat, j->nom);//on affecte le nom du detenteur du contrat
                sprintf(historique_enchere, "%s%s", historique_enchere, j->nom); //rajoute le nom apres les autres
                changement = 1;

                SDL_FreeSurface(carte_contrat);
            }
        }
        else //c'est l'ordinateur, il ne sait pas faire capot ougeneral
        {
            surenchere = determination_contrat(j->carte, j, partie);//determination du contrat du joueur avec la fonction définis

            j->contrat = ((j->contrat)/10)*10;

            if (j->contrat >= 80)
            {
                //on verifie les annonce
                calcul_annonce(j->carte, j, partie);
                j->contrat = j->contrat + j->annonce/2;
            }

            changement = demande_enchere_couleur(partie, j->contrat, j);
        }

        if (changement == 1)
        {
            if (surenchere == 0) // si il a encherir grace a ses cartes et non pas grace a la surenchere alors on enleve surenchere
            {
                partie->surencherir = 0;
            }
        }
        else
        {
            sprintf(historique_enchere, "%s%s", historique_enchere, j->nom); //rajoute le nom apres les autres
        }
    return changement;
}


int demande_enchere_couleur (Jeu *partie, int ench_max, Joueur *j) //fait encherir le robot de 10 par 10 juqu'a la somme maximal qu'il veut miser
{
    int ench = 80, changement = 0;
    if (ench_max > partie->contrat)
    {
        while (ench <= partie->contrat && ench + 10 <= ench_max)
        {
            ench += 10;
        }
        changement = 1;
        strcpy(partie->detenteur_contrat, j->nom);
        strcpy(partie->atout, (j->contrat_atout));
        partie->contrat = ench;
        j->contrat = ench;
        strcpy(partie->type_contrat, "CL"); //il n'encherit que de maniere classique
    }

    return changement;
}

int determination_contrat(char paquet[8][5], Joueur *j, Jeu *partie)
{
    int i=0;
    int compte_V=0,compte_A=0, compte_9=0, compte_10=0, compte_R=0;//variables compteur pour les cartes maitresse
    int compte_9P=0, compte_9T=0, compte_9Ca=0, compte_9Co=0;
    int compte_10P=0, compte_10T=0, compte_10Ca=0, compte_10Co=0;
    int valeur_contrat_TA=0,valeur_contrat_SA=0, valeur_contrat_Couleur=0, valeur_contratfinal=0;
    // on regarde si il peut surencherir
    int test_surenchere = 0;

    for (i=0;i<8;i++)
    {
        if (strncmp(paquet[i],"V",1)==0)//compte le nombre de valet dans le jeu
        {
            compte_V++;
        }
        else if (strncmp(paquet[i],"A",1)==0)//compte le nombre de as dans le jeu
        {
            compte_A++;
        }
        else if (strncmp(paquet[i],"9",1)==0)//compte le nombre de 9 dans le jeu
        {
            compte_9++;
        }
        else if (strncmp(paquet[i],"1",1)==0)//compte le nombre de as dans le jeu
        {
            compte_10++;
        }
        else if (strncmp(paquet[i],"R",1)==0)//compte le nombre de as dans le jeu
        {
            compte_R++;
        }
        if (strcmp(paquet[i],"9 P")==0)//compte le nombre de 9 de pique dans le jeu
        {
            compte_9P++;
        }
        else if (strcmp(paquet[i],"9 T")==0)//compte le nombre de 9 de trèfle dans le jeu
        {
            compte_9T++;
        }
        else if (strcmp(paquet[i],"9 Ca")==0)//compte le nombre de 9 de carreau dans le jeu
        {
            compte_9Ca++;
        }
        else if (strcmp(paquet[i],"9 Co")==0)//compte le nombre de 9 de coeur dans le jeu
        {
            compte_9Co++;
        }
        else if (strcmp(paquet[i],"10P")==0)//compte le nombre de 10 de pique dans le jeu
        {
            compte_10P++;
        }
        else if (strcmp(paquet[i],"10T")==0)//compte le nombre de 10 de trèfle dans le jeu
        {
            compte_10T++;
        }
        else if (strcmp(paquet[i],"10Ca")==0)//compte le nombre de 10 de carreau dans le jeu
        {
            compte_10Ca++;
        }
        else if (strcmp(paquet[i],"10Co")==0)//compte le nombre de 10 de coeur dans le jeu
        {
            compte_10Co++;
        }
    }

    valeur_contrat_TA = contrat_TA(paquet,compte_V,compte_9,compte_A,compte_9P,compte_9Ca,compte_9Co,compte_9T); // determine la valeur du contrat possible pour un jeu TA

    valeur_contrat_SA = contrat_SA(paquet,compte_A, compte_10, compte_R,compte_10P,compte_10Ca,compte_10Co,compte_10T); // determine la valeur du contrat possible pour un jeu SA

    char couleur[3] = "";
    valeur_contrat_Couleur = contrat_couleur(paquet, couleur); // determine la valeur du contrat possible pour un jeu couleur

    if (valeur_contrat_TA > valeur_contrat_SA && valeur_contrat_TA > valeur_contrat_Couleur)//determine la valeur du contrat maximum que le joueur peux avoir
    {
        valeur_contratfinal = valeur_contrat_TA;
        strcpy(j->contrat_atout, "TA");
    }
    else if (valeur_contrat_SA > valeur_contrat_TA && valeur_contrat_SA > valeur_contrat_Couleur)
    {
        valeur_contratfinal = valeur_contrat_SA;
        strcpy(j->contrat_atout, "SA");
    }
    else if (valeur_contrat_Couleur > valeur_contrat_SA && valeur_contrat_TA < valeur_contrat_Couleur)
    {
        valeur_contratfinal = valeur_contrat_Couleur;
        strcpy(j->contrat_atout, couleur);
    }

    if (valeur_contratfinal < partie->contrat && partie->surencherir == 0 && partie->contrat >= 80)//si l'encherre ne se fera pas et pas de surenchere de qq'un et il y a deja eu une enchere
    {
        if (strcmp(partie->atout, "TA")==0)
        {
            if ((compte_V + compte_9) >= 3)
            {
                test_surenchere = 1;
            }
        }
        else if (strcmp(partie->atout, "SA")==0)
        {
            if ((compte_A + compte_10) >= 3)
            {
                test_surenchere = 1;
            }
        }
        else
        {
            int nb = 0;
            for (int i = 0; i < 8; i+= 1)
            {
                if (strcmp(&paquet[i][2], partie->atout) == 0)
                {
                    nb += 1; //singnifie que la carte est à l'atout
                }
            }
            if (nb >= 3)
            {
                test_surenchere = 1;
            }
        }

        if (test_surenchere == 1) //maintenant on va verifier si c'est son equipier qui a pose le contrat, si oui on ajoute la surenchère de 10 points
        {
            switch(j->nom[0])
            {
                case 'W' : if(strcmp(partie->detenteur_contrat, "E")==0)
                    {
                        partie->surencherir = 1;
                        strcpy(partie->detenteur_contrat, j->nom);
                        valeur_contratfinal = partie->contrat + 10;
                        strcpy(j->contrat_atout, partie->atout);
                    }
                    break;
                case 'N' : if(strcmp(partie->detenteur_contrat, "S")==0)
                    {
                        partie->surencherir = 1;
                        strcpy(partie->detenteur_contrat, j->nom);
                        valeur_contratfinal = partie->contrat + 10;
                        strcpy(j->contrat_atout, partie->atout);
                    }
                    break;
                case 'E' : if(strcmp(partie->detenteur_contrat, "W")==0)
                    {
                        partie->surencherir = 1;
                        strcpy(partie->detenteur_contrat, j->nom);
                        valeur_contratfinal = partie->contrat + 10;
                        strcpy(j->contrat_atout, partie->atout);
                    }
                    break;
                default : test_surenchere = 0;
            }
        }

    }

    j->contrat = valeur_contratfinal;
    return test_surenchere;
}

int contrat_TA(char paquet[8][5],int compte_V,int compte_9, int compte_A, int compte_9P,int compte_9Ca,int compte_9Co,int compte_9T)
{
    int i=0;
    int compte_P=0,compte_T=0,compte_Ca=0,compte_Co=0;
    int valeur_contrat=0;
    for (i=0;i<8;i++)
    {
        if (strcmp(&paquet[i][2],"P")==0 && strncmp(paquet[i],"V",1)!=0 && strncmp(paquet[i],"9",1)!=0)//compte le nombre de cartes de la famille pique (sauf les cartes maitresse lorsque le joueur joue TA)
        {
            compte_P++;
        }
        else if (strcmp(&paquet[i][2],"T")==0 && strncmp(paquet[i],"V",1)!=0 && strncmp(paquet[i],"9",1)!=0)//compte le nombre de cartes de la famille tr�fles (sauf les cartes maitresse lorsque le joueur joue TA)
        {
            compte_T++;
        }
        else if (strcmp(&paquet[i][2],"Ca")==0 && strncmp(paquet[i],"V",1)!=0 && strncmp(paquet[i],"9",1)!=0)//compte le nombre de cartes de la famille carreau (sauf les cartes maitresse lorsque le joueur joue TA)
        {
            compte_Ca++;
        }
        else if (strcmp(&paquet[i][2],"Co")==0 && strncmp(paquet[i],"V",1)!=0 && strncmp(paquet[i],"9",1)!=0)//compte le nombre de cartes de la famille coeur (sauf les cartes maitresse lorsque le joueur joue TA)
        {
            compte_Co++;
        }
    }

    if ((compte_V>=2 && compte_9 >= 2) || (compte_V>=3 && compte_9 >= 1) || (compte_V>=2 && compte_9 >= 1 && compte_A>=3))//la personne va pour ench�rire si il obtient plus d'un valet, en tout atout
    {
        valeur_contrat = 14 * compte_V;

        if (compte_9P == 1 && compte_P >=1)// si la personne � un 9 et au moins une cartes de la meme famille (pour eviter qu il se fasse prendre son 9 avec un valet), en jouant tous atout, il peux facilement remporter un plie
        {
            valeur_contrat = valeur_contrat + 12;
        }
        if (compte_9T == 1 && compte_T >=1)
        {
            valeur_contrat = valeur_contrat + 12;
        }
        if (compte_9Ca == 1 && compte_Ca >=1)
        {
            valeur_contrat = valeur_contrat + 12;
        }
        if (compte_9Co == 1 && compte_Co >=1)
        {
            valeur_contrat = valeur_contrat + 12;
        }

        valeur_contrat = valeur_contrat + ((compte_9 + compte_V) * 9 + compte_A * 5); //le 9 et le valet valent plus que l'as
    }
    // seul le contrat le plus gros sera retenue parmis les joueurs.
    return valeur_contrat;

}

int contrat_SA(char paquet[8][5],int compte_A, int compte_10, int compte_R,int compte_10P,int compte_10Ca,int compte_10Co,int compte_10T)
{
    int i=0;
    int compte_P=0,compte_T=0,compte_Ca=0,compte_Co=0;
    int valeur_contrat=0;

    for (i=0;i<8;i++)
        {
            if (strcmp(&paquet[i][2],"P")==0 && strncmp(paquet[i],"A",1)!=0 && strncmp(paquet[i],"10",1)!=0)//compte le nombre de cartes de la famille pique (sauf les cartes maitresse lorsque le joueur joue SA)
            {
                compte_P++;
            }
            else if (strcmp(&paquet[i][2],"T")==0 && strncmp(paquet[i],"A",1)!=0 && strncmp(paquet[i],"10",1)!=0)//compte le nombre de cartes de la famille tr�fles (sauf les cartes maitresse lorsque le joueur joue SA)
            {
                compte_T++;
            }
            else if (strcmp(&paquet[i][2],"Ca")==0 && strncmp(paquet[i],"A",1)!=0 && strncmp(paquet[i],"10",1)!=0)//compte le nombre de cartes de la famille carreau (sauf les cartes maitresse lorsque le joueur joue SA)
            {
                compte_Ca++;
            }
            else if (strcmp(&paquet[i][2],"Co")==0 && strncmp(paquet[i],"V",1)!=0 && strncmp(paquet[i],"9",1)!=0)//compte le nombre de cartes de la famille coeur (sauf les cartes maitresse lorsque le joueur joue SA)
            {
                compte_Co++;
            }

        }

    if ((compte_A>=2 && compte_10 >= 2) || (compte_A>=3 && compte_10 >= 1) || (compte_A>=2 && compte_10 >= 1 && compte_R>=3))//la personne va pour ench�rire si il obtient plus d'un valet, en tout atout
    {
        valeur_contrat = 14 * compte_A;

        if (compte_10P == 1 && compte_P >=1)// si la personne � un 9 et au moins une cartes de la meme famille (pour eviter qu il se fasse prendre
                                            //son 9 avec un valet), en jouant tous atout, il peux facilement remporter un plie, il vaut donc plus que les 9 classiques
        {
            valeur_contrat = valeur_contrat + 12;
        }
        if (compte_10T == 1 && compte_T >=1)
        {
            valeur_contrat = valeur_contrat + 12;
        }
        if (compte_10Ca == 1 && compte_Ca >=1)
        {
            valeur_contrat = valeur_contrat + 12;
        }
        if (compte_10Co == 1 && compte_Co >=1)
        {
            valeur_contrat = valeur_contrat + 12;
        }

        valeur_contrat = valeur_contrat + ((compte_10 + compte_A) * 9 + compte_R * 5); //le 10 et l'as comptent plus que le roi
    }
    // seul le contrat le plus gros sera retenue parmis les joueurs.
    return valeur_contrat;

}
int contrat_famille(char temp[8][5])
{
    int compteur_A=0,compteur_V=0,compteur_10=0,compteur_9=0;
    int valeur_contrat=0;
    int i=0;

    while (i < 8 && strcmp(temp[i], "")!=0)
    {

        if(temp[i][0]== '9')
        {
            compteur_9++;
        }
        else if(temp[i][0]== '1')
        {
            compteur_10++;
        }
        else if(temp[i][0]== 'V')
        {
            compteur_V++;
        }
        else if(temp[i][0]== 'A')
        {
            compteur_A++;
        }
        i++;
    }

    if (i>=3 && compteur_V == 1 && compteur_9 == 1)
    {
        valeur_contrat = 20 + 14;
        if (compteur_A == 1)
        {
            valeur_contrat += 11;
            if (compteur_10 == 1)
            {
                valeur_contrat += 10;
            }
        }
        else if (compteur_A ==0 && compteur_10 == 1) //on peut facilement faire tomber l'as
        {
            valeur_contrat += 10;
        }
        valeur_contrat += i * 12;
    }
    else if (i>=4 && compteur_V == 1 && compteur_A == 1) //on peut faire tomber le 9 assez  facilement quand meme
    {
        valeur_contrat = 20 + 12; //mis a 12 pour permttre de faire passer As et V
        if (compteur_10 == 1)
        {
            valeur_contrat += 11;
        }
        valeur_contrat += i * 12;
    }
    return valeur_contrat;
}

int contrat_couleur(char paquet[8][5], char couleur[3])
{
    char temp1[8][5] = {{""},{""}};
    char temp2[8][5] = {{""},{""}};
    char temp3[8][5] = {{""},{""}};
    char temp4[8][5] = {{""},{""}};
    int i=0, j=0, valeur_contrat_P=0,valeur_contrat_Co=0,valeur_contrat_T=0,valeur_contrat_Ca=0,valeur_contratmax=0;

    while (i<8 && strcmp(&paquet[i][2],"T")==0)//copie chaque cartes de la meme famille dans différentes variables temporaires
    {
        strcpy(temp1[j],paquet[i]);
        i++;
        j++;
    }
    j=0;
    while(i<8 && strcmp(&paquet[i][2],"Co")==0)
    {
        strcpy(temp2[j],paquet[i]);
        i++;
        j++;
    }
    j=0;
    while(i<8 && strcmp(&paquet[i][2],"P")==0)
    {
        strcpy(temp3[j],paquet[i]);
        i++;
        j++;
    }
    j=0;
    while(i<8 && strcmp(&paquet[i][2],"Ca")==0)
    {
        strcpy(temp4[j],paquet[i]);
        i++;
        j++;
    }

    valeur_contrat_T = contrat_famille(temp1);//calcul pour chaque famille, le contrat possible
    valeur_contrat_Co = contrat_famille(temp2);
    valeur_contrat_P = contrat_famille(temp3);
    valeur_contrat_Ca = contrat_famille(temp4);

    if (valeur_contrat_P > valeur_contrat_Co && valeur_contrat_P > valeur_contrat_T && valeur_contrat_P > valeur_contrat_Ca)//determine le maximum des contrat parmis toutes les familles
    {
        valeur_contratmax = valeur_contrat_P;
        strcpy(couleur, "P");
    }
    else if (valeur_contrat_P < valeur_contrat_Co && valeur_contrat_Co > valeur_contrat_T && valeur_contrat_Co > valeur_contrat_Ca)
    {
        valeur_contratmax = valeur_contrat_Co;
        strcpy(couleur, "Co");
    }
    else if (valeur_contrat_T > valeur_contrat_Co && valeur_contrat_P < valeur_contrat_T && valeur_contrat_T > valeur_contrat_Ca)
    {
        valeur_contratmax = valeur_contrat_T;
        strcpy(couleur, "T");
    }
    else if (valeur_contrat_Ca > valeur_contrat_Co && valeur_contrat_Ca > valeur_contrat_T && valeur_contrat_P < valeur_contrat_Ca)
    {
        valeur_contratmax = valeur_contrat_Ca;
        strcpy(couleur, "Ca");
    }
    return valeur_contratmax;
}


void attribution_equipe_preneuse(Equipe *N_S, Equipe *E_W, Jeu *partie)
{
    if (strncmp(partie->detenteur_contrat, N_S->nom, 1) == 0 || strcmp(partie->detenteur_contrat, &(N_S->nom[2])) == 0)
    {
        strcpy(partie->equipe_preneuse,N_S->nom); //on dit quelle equipe prend

    } else if (strncmp(partie->detenteur_contrat, E_W->nom, 1) == 0 || strcmp(partie->detenteur_contrat, &(E_W->nom[2])) == 0)
    {
        strcpy(partie->equipe_preneuse,E_W->nom); //on dit quelle equipe prend
    }
}

void detection_belote_rebelote(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, Equipe *N_S, Equipe *E_W)
{
    int comptage = 0; //compte la dame et le roi de l'atout de chaque equipe
    //on commence avec l'equipe N_S
    for (int i=0; i < 8; i += 1)
    {
        if (strcmp(&(ordi_N->carte[i][2]), partie->atout)==0 && (strncmp(ordi_N->carte[i], "R", 1) == 0 || strncmp(ordi_N->carte[i], "Q", 1) == 0))
        {
            comptage += 1;
        }
    }
    for (int i=0; i < 8; i += 1)
    {
        if (strcmp(&(joueur_S->carte[i][2]), partie->atout)==0 && (strncmp(joueur_S->carte[i], "R", 1) == 0 || strncmp(joueur_S->carte[i], "Q", 1) == 0))
        {
            comptage += 1;
        }
    }
    if (comptage == 2)
    {
        N_S->belote_rebelote = 10;
    }
    else
    {
        comptage = 0;
        //on recommence avec l'equipe E_S
        for (int i=0; i < 8; i += 1)
        {
            if (strcmp(&(ordi_E->carte[i][2]), partie->atout)==0 && (strncmp(ordi_E->carte[i], "R", 1) == 0 || strncmp(ordi_E->carte[i], "Q", 1) == 0))
            {
                comptage += 1;
            }
        }
        for (int i=0; i < 8; i += 1)
        {
            if (strcmp(&(ordi_W->carte[i][2]), partie->atout)==0 && (strncmp(ordi_W->carte[i], "R", 1) == 0 || strncmp(ordi_W->carte[i], "Q", 1) == 0))
            {
                comptage += 1;
            }
        }
        if (comptage == 2)
        {
            E_W->belote_rebelote = 10;
        }
    }

}
