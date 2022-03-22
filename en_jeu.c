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


int lancement_manche(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, Equipe *N_S, Equipe *E_W, SDL_Surface *fenetre)
{
    int exit = 0;
    partie->pli = 0;
    //on range les cartes dans l'odre par rapport à l'atout
    range_croissant(ordi_W->carte, partie->atout, partie); //on classe cette fois on considérant les cartes à l'atout
    range_croissant(ordi_N->carte, partie->atout, partie);
    range_croissant(ordi_E->carte, partie->atout, partie);
    range_croissant(joueur_S->carte, partie->atout, partie);
    detection_belote_rebelote(ordi_W, ordi_N, ordi_E, joueur_S, partie, N_S, E_W); //va detecter les belote rebelote avant le debut de la partie
    calcul_annonce(ordi_W->carte, ordi_W, partie); // on recalcule les annonces de tout le monde pour tenir compte de la couleur de l'atout
    calcul_annonce(ordi_N->carte, ordi_N, partie);
    calcul_annonce(ordi_E->carte, ordi_E, partie);
    calcul_annonce(joueur_S->carte, joueur_S, partie);

    do {
        rafraichir(fenetre, partie, N_S, E_W, ordi_W, ordi_N, ordi_E, joueur_S);

        exit = lancement_pli(ordi_W, ordi_N, ordi_E, joueur_S, partie, N_S, E_W, fenetre);

        partie->pli += 1;

    } while (partie->pli < 8 && exit != 1);

    if(exit == 1) //signifie que l'on a quitte la boucle
    {
        return EXIT_FAILURE;
    }
    char texte[64] = "";
    sprintf(texte,"La manche est terminee - Dix de der : %s",partie->detenteur_dix_de_der);
    affiche_evenements(fenetre, partie, texte,"");
    exit = bouton_continuer(fenetre);
    rafraichir(fenetre, partie, N_S, E_W, ordi_W, ordi_N, ordi_E, joueur_S);

    return exit;
}

int lancement_pli(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Jeu *partie, Equipe *N_S, Equipe *E_W, SDL_Surface *fenetre)
{
    partie->ordre = qui_commence(partie);
    int exit1 = 0, exit2 = 0;
    partie->tour = 0;//compte le nombre de carte pose sur la table

    do {
        switch(partie->ordre) {
            case 0 : selection_joueur(fenetre, 'W');
                exit1 = bouton_IA(fenetre);//met en pause et met un bouton
                exit2 = pose(ordi_W, partie, fenetre, N_S, E_W);
                break;
            case 1 : selection_joueur(fenetre, 'N');
                exit1 = bouton_IA(fenetre);//met en pause et met un bouton
                exit2 = pose(ordi_N, partie, fenetre, N_S, E_W);
                break;
            case 2 : selection_joueur(fenetre, 'E');
                exit1 = bouton_IA(fenetre);//met en pause et met un bouton
                exit2 = pose(ordi_E, partie, fenetre, N_S, E_W);
                break;
            case 3 : selection_joueur(fenetre, 'S');
                exit2 = pose(joueur_S, partie, fenetre, N_S, E_W);
                break;
        }

        if (exit1 == 1 || exit2 == 1)
        {
            return EXIT_FAILURE;
        }

        partie->tour += 1;
        partie->ordre += 1 ;
        if (partie->ordre == 4)
        {
            partie->ordre = 0;
        }
        calcul_des_points(partie); //on calcul les points du pli
        rafraichir(fenetre, partie, N_S, E_W, ordi_W, ordi_N, ordi_E, joueur_S);
        //si le compteur belote rebelote est à deux alors dame et roi a l'atout sont tombé, on vérifie si une équipe avait la belote rebelote auquel cas on l'affiche à l'écran

    } while (partie->tour < 4);

    actualisation_atout(partie); //on actualise le comptage des atouts
    attribution_des_points(ordi_W, ordi_N, ordi_E, joueur_S, N_S, E_W, partie); //on attribue les points au gagnant
    //recap du pli
    char texte[32] = "";
    sprintf(texte, "Le gagnant du pli est : %s", partie->detenteur_carte_maitresse);
    affiche_evenements(fenetre, partie,"",texte);

    if (partie->compteur_belote_rebelote == 2) // sert a afficher s'il y a eu la belote rebelote, on va compter le roi et la dame a l'atout, si c'est tombe on verifie si on a deja attribue la belote rebelote a une equipe, si oui on l'affiche
    {
        if (N_S->belote_rebelote == 10)
        {
            affiche_evenements(fenetre, partie, "Belote-rebelotte N_S      ","");
            partie->compteur_belote_rebelote = 0;
            strcpy(partie->detenteur_belote_rebelote, "N_S");

        }
        else if (E_W->belote_rebelote == 10)
        {
            affiche_evenements(fenetre, partie, "Belote-rebelotte E_W      ","");
            partie->compteur_belote_rebelote = 0;
            strcpy(partie->detenteur_belote_rebelote, "E_W");
        }
        else //ils sont tombés mais n'appartaient pas à la meme équipe
        {
            partie->compteur_belote_rebelote = 0;
        }
    }
    if (partie->pli != 7)// si ce n'est pas le dernier pli
    {
        affiche_evenements(fenetre, partie, "Le pli est fini - Appuyez pour relancer","");
        exit1 = bouton_continuer(fenetre);
        rafraichir(fenetre, partie, N_S, E_W, ordi_W, ordi_N, ordi_E, joueur_S);
    }

    //clean des paramètres du pli et actualisation du jeu
    strcpy(partie->gagnant_precedent, partie->detenteur_carte_maitresse);
    strcpy(partie->carte_maitresse, "");
    strcpy(partie->detenteur_carte_maitresse, "");
    for (int i = 0; i < 8; i += 1)
    {
        strcpy(partie->table[i], "");
    }

    return exit1;
}

int pose(Joueur *j, Jeu *partie, SDL_Surface *fenetre, Equipe *N_S, Equipe *E_W)
{
    int exit = 0;

    if (partie->pli == 0)
    {
        if(j->annonce > partie->annonce_max) //on va verifie lors du premier tour qui a la plus grosse annonce et on retient alors laplus grosse
        {
            partie->annonce_max = j->annonce;
            strcpy(partie->detenteur_annonce_max, j->nom);
            strcpy(partie->quelle_annonce_max, j->quelle_annonce);
            char texte[30]="";
            sprintf(texte, "Annonce pour %s : %s",partie->detenteur_annonce_max,partie->quelle_annonce_max);
            affiche_evenements(fenetre, partie, texte, "");
            if (partie->detenteur_annonce_max[0]=='N' && partie->detenteur_annonce_max[0]=='S')
            {
                N_S->annonce = partie->annonce_max;
                E_W->annonce = 0;
            }
            else
            {
                E_W->annonce = partie->annonce_max;
                N_S->annonce = 0;
            }
        }
    }

    if (j->nom[0] == 'S')
    {
        //affichage des possibilités
        affiche_possibilite(j, partie, fenetre);
    }
    else
    {
        choix_possibilite_ordi(j, partie);
    }
    //choix des possibilite + actualisation du paquet
    exit = choisir_possibilite(j, partie);
    //actualisation de la carte maitresse
    actualisation_carte_maitresse(j, partie);

    return exit;
}

void affiche_possibilite(Joueur *j, Jeu *partie, SDL_Surface *fenetre)
{
    int i = 0; //sert pour gérer le nombre de tour de la boucle while
    int k = 0; //permet de stocker le nombre de carte envoyées dans le paquet des possibilitées si différent de i !

    if ((strcmp(partie->carte_maitresse, "")==0)) //si c'est la première carte posee du pli
    {
        while (strcmp(j->carte[i], "")!=0 && i < 8) //tant que la case n'est pas vide et i < 8
        {
            strcpy(j->possibilite_carte[i], j->carte[i]);
            i += 1;
        }
        j->nb_possibilite_carte = i; //affecte le nb de cartes disponible pour la selection
        affich_possibilite_carte(j, fenetre); //affiche les cartes disponibles pour la selection

    }
    else if (partie->tour == 7) //si c'est le dernier tour
    {
        i = 0;
        while (strcmp(j->carte[i], "")!=0 && i < 8) //il se défausse
        {
            strcpy(j->possibilite_carte[i], j->carte[i]);
            i += 1;
        }
        j->nb_possibilite_carte = i;
        affich_possibilite_carte(j, fenetre);
    }
    else
    {
        if (strcmp(partie->atout, "TA")==0)//TA
        {
            int presence_couleur = 0; //nombre de carte a la couleur demandee
            i = 0;
            while (i < 8 && strcmp(j->carte[i], "")!=0) //verifie la presence de la couleur demandée dans les cartes
            {
                if (strcmp(&(partie->carte_maitresse[2]), &(j->carte[i][2]))==0)
                {
                    presence_couleur += 1;
                }
                i += 1;
            }

            if (presence_couleur != 0) //signifie qu'il a des cartes de la bonne couleur
            {
                //on vérifie si il a une carte plus forte à la couleur demandee qui sont toutes atout, donc si il a, alors on lui affiche les cartes plus fortes, sinon on lui affiche ses toutes cartes
                i = 0, k = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8 )
                    {
                        if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0 && (valeur(j->carte[i], partie->atout) > valeur(partie->carte_maitresse, partie->atout))) //si la carte est de la bonne couleur et si elle est plus forte
                        {
                            strcpy(j->possibilite_carte[k], j->carte[i]);
                            k += 1;
                        }
                        i += 1;

                    }

                if (k == 0) //si il n'a pas de carte plus forte
                {
                    i = 0, k = 0;
                    while (strcmp(j->carte[i], "")!=0 && i < 8)
                    {
                        if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0)
                        {
                            strcpy(j->possibilite_carte[k], j->carte[i]);
                            k += 1;
                        }
                        i += 1;

                    }
                }
                j->nb_possibilite_carte = k;
                affich_possibilite_carte(j, fenetre);
            }
            else //il se defausse
            {
                i = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8) //il se défausse
                {
                    strcpy(j->possibilite_carte[i], j->carte[i]);
                    i += 1;
                }
                j->nb_possibilite_carte = i;
                affich_possibilite_carte(j, fenetre);
            }

        }
        else if (strcmp(partie->atout, "SA")==0)//SA
        {
            int presence_couleur = 0; //nombre de carte a la couleur demandee
            i = 0;
            while (i < 8 && strcmp(j->carte[i], "")!=0) //verifie la presence de la couleur demandée dans les cartes
            {
                if (strcmp(&(partie->carte_maitresse[2]), &(j->carte[i][2]))==0)
                {
                    presence_couleur += 1;
                }
                i += 1;
            }

            if (presence_couleur > 0) // si il a la couleur demandee alors il doit la jouer
            {
                i = 0,k = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8)
                {
                    if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0)
                    {
                        strcpy(j->possibilite_carte[k], j->carte[i]);
                        k += 1;
                    }
                    i += 1;
                }
                j->nb_possibilite_carte = k;
                affich_possibilite_carte(j, fenetre);
            }
            else //il se defausse
            {
                i = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8)
                {
                    strcpy(j->possibilite_carte[i], j->carte[i]);
                    i += 1;
                }
                j->nb_possibilite_carte = i;
                affich_possibilite_carte(j, fenetre);
            }
        }
        else //l'atout est une couleur
        {
            //si une carte a deja ete posee
            int presence_atout = 0; //recopere de le nombre de carte a l'atout
            i = 0;
            while (i < 8 && strcmp(j->carte[i], "")!=0) //verifie la presence d'un atout dans les cartes
            {
                if (strcmp(partie->atout, &(j->carte[i][2]))==0)
                {
                    presence_atout += 1;
                }
                i += 1;
            }

            if (strcmp(partie->atout, &(partie->carte_maitresse[2])) == 0 && presence_atout > 0) // si la couleur c'est l'atout, et s'il a de l'atout alors il doit monter à l'atout
            {
                //il doit monter à l'atout si il peut !
                //on vérifie si il a une carte plus forte à l'atout, si il a, alors on lui affiche toutes les cartes plus fortes, sinon on lui affiche ses cartes à l'atout

                i = 0, k = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8 )
                    {
                        if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0 && (valeur(j->carte[i], partie->atout) > valeur(partie->carte_maitresse, partie->atout))) //si la carte est de la bonne couleur et si elle est plus forte
                        {
                            strcpy(j->possibilite_carte[k], j->carte[i]);
                            k += 1;
                        }
                        i += 1;

                    }


                if (k == 0) //si il n'a pas de carte plus forte
                {
                    i = 0, k = 0;
                    while (strcmp(j->carte[i], "")!=0 && i < 8)
                    {
                        if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0)
                        {
                            strcpy(j->possibilite_carte[k], j->carte[i]);
                            k += 1;
                        }
                        i += 1;

                    }
                }


                j->nb_possibilite_carte = k;
                affich_possibilite_carte(j, fenetre);


            }
            else if (strcmp(partie->atout, &(partie->carte_maitresse[2])) == 0 && presence_atout == 0) // si la couleur c'est l'atout, et s'il a pas d'atout alors il défausse
            {
                i = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8) //il se defausse
                {
                    strcpy(j->possibilite_carte[i], j->carte[i]);
                    i += 1;
                }
                    j->nb_possibilite_carte = i;
                    affich_possibilite_carte(j, fenetre);
            }
            else //cela veut dire que la couleur n'est pas l'atout
            {
                int presence_couleur = 0; //nombre de carte a la couleur demandee
                i = 0;
                while (i < 8 && strcmp(j->carte[i], "")!=0) //verifie la presence de la couleur demandée dans les cartes
                {
                    if (strcmp(&(partie->carte_maitresse[2]), &(j->carte[i][2]))==0)
                    {
                        presence_couleur += 1;
                    }
                    i += 1;
                }

                if (presence_couleur > 0) // si il a la couleur demandee alors il doit la jouer
                {
                    i = 0,k = 0;
                    while (strcmp(j->carte[i], "")!=0 && i < 8)
                    {
                        if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0)
                        {
                            strcpy(j->possibilite_carte[k], j->carte[i]);
                            k += 1;
                        }
                        i += 1;
                    }
                    j->nb_possibilite_carte = k;
                    affich_possibilite_carte(j, fenetre);
                }
                else
                {
                    if(presence_atout > 0)
                    {
                        i = 0,k = 0;
                        while (strcmp(j->carte[i], "")!=0 && i < 8) //il doit couper
                        {
                            if (strcmp(&(j->carte[i][2]), partie->atout)==0)
                            {
                                strcpy(j->possibilite_carte[k], j->carte[i]);
                                k += 1;

                            }
                            i += 1;
                        }
                    j->nb_possibilite_carte = k;
                    affich_possibilite_carte(j, fenetre);
                    }
                    else
                    {
                        i = 0;
                        while (strcmp(j->carte[i], "")!=0 && i < 8) //il se défausse
                        {
                            strcpy(j->possibilite_carte[i], j->carte[i]);
                            i += 1;
                        }
                        j->nb_possibilite_carte = i;
                        affich_possibilite_carte(j, fenetre);
                    }
                }
            }
        }
    }

}

void choix_possibilite_ordi(Joueur *j, Jeu *partie)
{
    int i = 0; //sert pour gérer le nombre de tour de la boucle while
    int k = 0; //permet de stocker le nombre de carte envoyées dans le paquet des possibilitées si différent de i !

    if ((strcmp(partie->carte_maitresse, "")==0)) //si c'est la première carte posee du pli l'ordi jour n'importe quoi
    {
        while (strcmp(j->carte[i], "")!=0 && i < 8) //tant que la case n'est pas vide et i < 8
        {
            strcpy(j->possibilite_carte[i], j->carte[i]);
            i += 1;
        }
        j->nb_possibilite_carte = i; //affecte le nb de cartes disponible pour la selection
        jeu_ordi(j, partie);

    }
    else if (partie->tour == 7) //si c'est le dernier tour
    {
        i = 0;
        while (strcmp(j->carte[i], "")!=0 && i < 8)
            {

                strcpy(j->possibilite_carte[i], j->carte[i]);
                i += 1;

            }
            j->nb_possibilite_carte = i;
            jeu_ordi(j, partie);
    }
    else //si une carte a dej ete posee
    {
        if (strcmp(partie->atout, "TA")==0)//TA
        {
            int presence_couleur = 0; //nombre de carte a la couleur demandee
            i = 0;
            while (i < 8 && strcmp(j->carte[i], "")!=0) //verifie la presence de la couleur demandée dans les cartes
            {
                if (strcmp(&(partie->carte_maitresse[2]), &(j->carte[i][2]))==0)
                {
                    presence_couleur += 1;
                }
                i += 1;
            }

            if (presence_couleur != 0) //signifie qu'il a des cartes de la bonne couleur
            {
                //on vérifie si il a une carte plus forte à la couleur demandee qui sont toutes atout, donc si il a, alors on lui affiche les cartes plus fortes, sinon on lui affiche ses toutes cartes
                i = 0, k = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8 )
                    {
                        if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0 && (valeur(j->carte[i], partie->atout) > valeur(partie->carte_maitresse, partie->atout))) //si la carte est de la bonne couleur et si elle est plus forte
                        {
                            strcpy(j->possibilite_carte[k], j->carte[i]);
                            k += 1;
                        }
                        i += 1;

                    }

                if (k == 0) //si il n'a pas de carte plus forte alors il peut mettre la plus faible
                {
                    i = 0, k = 0;
                    while (strcmp(j->carte[i], "")!=0 && i < 8)
                        {
                            if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0)
                            {
                                strcpy(j->possibilite_carte[k], j->carte[i]);
                                k += 1;
                            }
                            i += 1;
                        }
                    j->nb_possibilite_carte = k;
                    jeu_ordi(j, partie);//il doit surcouper avec la plus faible
                }
                else //il doit soucoupper avec la plus faible
                {
                    j->nb_possibilite_carte = k;
                    jeu_ordi(j, partie); //soucoupe avec la plus faible
                }
            }
            else //il se defausse
            {
                i = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8) //il se défausse
                {
                    strcpy(j->possibilite_carte[i], j->carte[i]);
                    i += 1;
                }
                j->nb_possibilite_carte = i;
                jeu_ordi(j, partie); //se defausse avec la plus faible
            }

        }
        else if (strcmp(partie->atout, "SA")==0)//SA
        {
            int presence_couleur = 0; //nombre de carte a la couleur demandee
            i = 0;
            while (i < 8 && strcmp(j->carte[i], "")!=0) //verifie la presence de la couleur demandée dans les cartes
            {
                if (strcmp(&(partie->carte_maitresse[2]), &(j->carte[i][2]))==0)
                {
                    presence_couleur += 1;
                }
                i += 1;
            }

            if (presence_couleur != 0) //signifie qu'il a des cartes de la bonne couleur
            {
                //on vérifie si il a une carte plus forte à la couleur demande, si oui, on lui fait jouer la plus faible mais qui pourait prendre, sinon defausse dans la couleur
                i = 0, k = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8 )
                    {
                        if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0 && (valeur(j->carte[i], partie->atout) > valeur(partie->carte_maitresse, partie->atout))) //si la carte est de la bonne couleur et si elle est plus forte
                        {
                            strcpy(j->possibilite_carte[k], j->carte[i]);
                            k += 1;
                        }
                        i += 1;

                    }

                if (k == 0) //si il n'a pas de carte plus forte alors il peut mettre la plus faible
                {
                    i = 0, k = 0;
                    while (strcmp(j->carte[i], "")!=0 && i < 8)
                        {
                            if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0)
                            {
                                strcpy(j->possibilite_carte[k], j->carte[i]);
                                k += 1;
                            }
                            i += 1;
                        }
                    j->nb_possibilite_carte = k;
                    jeu_ordi(j, partie);//il doit surcouper avec la plus faible
                }
                else //il doit soucoupper avec la plus faible
                {
                    j->nb_possibilite_carte = k;
                    jeu_ordi(j, partie); //soucoupe avec la plus faible
                }
            }
            else //il se defausse
            {
                i = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8) //il se défausse
                {
                    strcpy(j->possibilite_carte[i], j->carte[i]);
                    i += 1;
                }
                j->nb_possibilite_carte = i;
                jeu_ordi(j, partie); //se defausse avec la plus faible
            }
        }
        else // l'atout est une couleur
        {
            int presence_atout = 0; //recopere de le nombre de carte a l'atout
            i = 0;
            while (i < 8 && strcmp(j->carte[i], "")!=0) //verifie la presence d'un atout dans les cartes
            {
                if (strcmp(partie->atout, &(j->carte[i][2]))==0)
                {
                    presence_atout += 1;
                }
                i += 1;
            }

            if (strcmp(partie->atout, &(partie->carte_maitresse[2])) == 0 && presence_atout > 0) // si la couleur c'est l'atout, et s'il a de l'atout alors il doit monter à l'atout
            {
                //il doit monter à l'atout si il peut !
                //on vérifie si il a une carte plus forte à l'atout, si il a, alors on lui affiche toutes les cartes plus fortes, sinon on lui affiche ses cartes à l'atout

                i = 0, k = 0;
                while (strcmp(j->carte[i], "")!=0 && i < 8 )
                    {
                        if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0 && (valeur(j->carte[i], partie->atout) > valeur(partie->carte_maitresse, partie->atout))) //si la carte est de la bonne couleur et si elle est plus forte
                        {
                            strcpy(j->possibilite_carte[k], j->carte[i]);
                            k += 1;
                        }
                        i += 1;
                    }


                if (k == 0) //si il n'a pas de carte plus forte alors il peut mettre la plus faible
                {
                    i = 0, k = 0;
                    while (strcmp(j->carte[i], "")!=0 && i < 8)
                        {
                            if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0)
                            {
                                strcpy(j->possibilite_carte[k], j->carte[i]);
                                k += 1;
                            }
                            i += 1;
                        }
                    j->nb_possibilite_carte = k;
                    jeu_ordi(j, partie);//il doit surcouper avec la plus faible
                }
                else //il doit soucoupper avec la plus faible
                {
                    j->nb_possibilite_carte = k;
                    jeu_ordi(j, partie); //soucoupe avec la plus faible
                }


                }
                else if (strcmp(partie->atout, &(partie->carte_maitresse[2])) == 0 && presence_atout == 0) // si la couleur c'est l'atout, et s'il a pas d'atout alors il défausse
                {
                    i = 0;
                    while (strcmp(j->carte[i], "")!=0 && i < 8) //il se defausse
                    {
                        strcpy(j->possibilite_carte[i], j->carte[i]);
                        i += 1;
                    }
                        j->nb_possibilite_carte = i;
                        jeu_ordi(j, partie); //avec la plus faible
                }
            else //cela veut dire que la couleur n'est pas l'atout
            {
                int presence_couleur = 0; //nombre de carte a la couleur demandee
                i = 0;
                while (i < 8 && strcmp(j->carte[i], "")!=0) //verifie la presence de la couleur demandée dans les cartes
                {
                    if (strcmp(&(partie->carte_maitresse[2]), &(j->carte[i][2]))==0)
                    {
                        presence_couleur += 1;
                    }
                    i += 1;
                }

                if (presence_couleur > 0) // on va faire comme pour l'atout et il doit monter si il a plus fort dans la couleur demandee sinon il se défausse dans cette couleur
                {
                    i = 0, k = 0;
                    while (strcmp(j->carte[i], "")!=0 && i < 8 )
                        {
                            if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0 && (valeur(j->carte[i], partie->atout) > valeur(partie->carte_maitresse, partie->atout))) //si la carte est de la bonne couleur et si elle est plus forte
                            {
                                strcpy(j->possibilite_carte[k], j->carte[i]);
                                k += 1;
                            }
                            i += 1;
                        }

                    if (k == 0) //si il n'a pas de carte plus forte alors il met la plus faible
                    {
                        i = 0, k = 0;
                        while (strcmp(j->carte[i], "")!=0 && i < 8)
                            {
                                if (strcmp(&(j->carte[i][2]), &(partie->carte_maitresse[2]))==0)
                                {
                                    strcpy(j->possibilite_carte[k], j->carte[i]);
                                    k += 1;
                                }
                                i += 1;
                            }
                        j->nb_possibilite_carte = k;
                        jeu_ordi(j, partie);//il doit esaie de prendre le pli avec la plus faible possible
                    }
                    else //il met la plus faible de la couleur
                    {
                        j->nb_possibilite_carte = k;
                        jeu_ordi(j, partie); //la plus faible
                    }
                }
                else
                {
                    if(presence_atout > 0)
                    {
                        i = 0,k = 0;
                        while (strcmp(j->carte[i], "")!=0 && i < 8) //il doit couper
                        {
                            if (strcmp(&(j->carte[i][2]), partie->atout)==0)
                            {
                                strcpy(j->possibilite_carte[k], j->carte[i]);
                                k += 1;

                            }
                            i += 1;
                        }
                        j->nb_possibilite_carte = k;
                        jeu_ordi(j, partie); //avec la plus faible
                    }
                    else
                    {
                        i = 0;
                        while (strcmp(j->carte[i], "")!=0 && i < 8) //il se défausse
                        {
                            strcpy(j->possibilite_carte[i], j->carte[i]);
                            i += 1;
                        }
                        j->nb_possibilite_carte = i;
                        jeu_ordi(j, partie); //avec la plus faible
                    }
                }
            }
        }
    }
}

void jeu_ordi(Joueur *j, Jeu *partie)
{
    int i = 0, min = 50, position = -10;
    int attaque = 0;

    if (strcmp(partie->atout, "SA")==0 && (strcmp(&(j->possibilite_carte[0][2]),&(partie->carte_maitresse[2]))==0 || strcmp(partie->carte_maitresse,"")==0)) // si il a des cartes de la bonne couleur ou si c'est le premier tour
    {
        i = 0;
        while(i < j->nb_possibilite_carte && attaque == 0) //on test si il a un As dans son jeu
            {
                if (j->possibilite_carte[i][0]=='A')
                {
                    j->possibilite = i;
                    attaque = 1;
                }
                i += 1;
            }

        i=0;
        while(i < j->nb_possibilite_carte && attaque == 0) //on teste si il a un 10 dans son jeu
            {
                if (j->possibilite_carte[i][0]=='1')
                {
                    int presence_as = 0;
                    for (int k = 0; k < 4; k += 1) //on regarde si son 10 est maitre
                    {
                        if (strcmp(partie->atout_restant[k],&(j->possibilite_carte[i][2]))==0) //si on un as de la meme couleur qui n'est pas tombe
                        {
                            presence_as = 1;
                        }
                    }
                    if (presence_as == 0) //il n'y a pas d'as et donc le 10 est maitre
                    {
                        j->possibilite = i;
                        attaque = 1;
                    }
                }
                i += 1;
            }
    }
    else if (strcmp(partie->atout, "TA")==0 && (strcmp(&(j->possibilite_carte[0][2]),&(partie->carte_maitresse[2]))==0 || strcmp(partie->carte_maitresse,"")==0))
    {
        i = 0;
        while(i < j->nb_possibilite_carte && attaque == 0) //on teste si il a un V dans son jeu
            {
                if (j->possibilite_carte[i][0]=='V')
                {
                    j->possibilite = i;
                    attaque = 1;
                }
                i += 1;
            }

        i=0;
        while(i < j->nb_possibilite_carte && attaque == 0) //on teste si il a un 9 dans son jeu
            {
                if (j->possibilite_carte[i][0]=='9')
                {
                    int presence_v = 0;
                    for (int k = 0; k < 4; k += 1) //on regarde si son 10 est maitre
                    {
                        if (strcmp(partie->atout_restant[k],&(j->possibilite_carte[i][2]))==0) //si on un valet de la meme couleur qui n'est pas tombe
                        {
                            presence_v = 1;
                        }
                    }
                    if (presence_v == 0) //il n'y a pas d'as et donc le 10 est maitre
                    {
                        j->possibilite = i;
                        attaque = 1;
                    }
                }
                i += 1;
            }
    }
    else //signifie que c'est à la couleur
    {
        i = 0;
        while(i < j->nb_possibilite_carte && attaque == 0) //on parcours le paquet
        {
            if (strcmp(&(j->possibilite_carte[i][2]), partie->atout)==0) //si on trouve une carte à l'atout
            {
                if (j->possibilite_carte[i][0]==partie->atout_couleur_restant[0][0])//si sa carte est egale a la premiere carte du paquet qui stocke les cartes a l'atout restantes
                {
                    j->possibilite = i;
                    attaque = 1;
                }
            }
            i += 1;
        }
    }
    i = 0;
    if (attaque == 0)//il n'a rien pu faire donc on le fait jouer de manière classique
    {//il joue la plus faible parmis les cartes proposées

        while (i < j->nb_possibilite_carte) //on cherche quelle est la première carte avec la plus faible
        {
            if (valeur(j->possibilite_carte[i], partie->atout)<min)
            {
                min = valeur(j->possibilite_carte[i], partie->atout);
                position = i;
            }
            i += 1;
        }
        j->possibilite = position;

    }
}

void actualisation_atout(Jeu *partie)
{
    for (int carte = 0; carte < 4; carte += 1)
    {
        //on va venir actualiser les paquet du comptage des cartes
        if (strcmp(partie->atout, "SA")==0) //si la partie est a sans atout ou tout atout on va venir modifier le paquet
        {
            if (partie->table[carte][0] == 'A') // si la carte posee est un as alors on le supprime du paquet d'as
            {
                for (int i = 0; i < 4; i += 1)
                {
                    if (strcmp(partie->atout_restant[i], &(partie->table[carte][2]))==0)
                    {
                        strcpy(partie->atout_restant[i],"");
                    }
                }
            }
        }
        else if (strcmp(partie->atout, "TA")==0) //si la partie est a sans atout ou tout atout on va venir modifier le paquet
        {
            if (partie->table[carte][0] == 'V') // si la carte posee est un valet alors on le supprime du paquet d'as
            {
                for (int i = 0; i < 4; i += 1)
                {
                    if (strcmp(partie->atout_restant[i], &(partie->table[carte][2]))==0)
                    {
                        strcpy(partie->atout_restant[i],"");
                    }
                }
            }
        }
        else
        {
            if (strcmp(partie->atout, &(partie->table[carte][2]))==0) // si la carte est a l'atout
            {
                int pos = 0;//on cherche la position de la carte dans le paquet d'atout
                while (partie->table[carte][0]!=partie->atout_couleur_restant[pos][0] && pos < 9)
                {
                    pos += 1;
                }
                //la carte se trouvera à la position : pos
                //on va supprimer cette carte
                strcpy(partie->atout_couleur_restant[pos], "");
                //et décaler tout le paquet pour ne pas laisser de vide au milieu
                for (int i = pos; i+1 < 8; i += 1)
                {
                    strcpy(partie->atout_couleur_restant[i], partie->atout_couleur_restant[i+1]);
                    strcpy(partie->atout_couleur_restant[i+1], "");
                }
            }

        }
    }
}

int choisir_possibilite(Joueur *j, Jeu *partie)//rpogramme qui recupere le choix de la carte, la recherche dans le paquet, la supprime et décale le paquet pour ne pas laisser de vide
{
    if (j->nom[0] == 'S')
    {
        SDL_Event event;
        int decision = 1;
        SDL_WaitEvent(&event);
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
                            if (event.button.y >64 && event.button.y < 160)
                            {//on calcul sur quelle case il a clique sachant que la premiere carte commence en 697 et que toutes les cartes sont entre 64 et 160 en y
                                int numero = (event.button.x-696)/73; //on prend 1 de moins pour prendre l'espacement enrte les cartes de 2 px en compte
                                if (numero < j->nb_possibilite_carte)
                                {
                                    decision = 0;
                                    j->possibilite = numero;
                                }
                            }
                        }
                }
        }

    }

    switch (j->nom[0])
    {
        case 'W' : strcpy(partie->table[0], j->possibilite_carte[j->possibilite]); //rappel chaque emplacement de la table est reservee a un joueur pour l'affichage
            break;
        case 'N' : strcpy(partie->table[1], j->possibilite_carte[j->possibilite]);
            break;
        case 'E' : strcpy(partie->table[2], j->possibilite_carte[j->possibilite]);
            break;
        case 'S' : strcpy(partie->table[3], j->possibilite_carte[j->possibilite]);
            break;
    }

    int pos = 0;//on cherche la position de la carte choisie dans le paquet du joueur
    while (strcmp(j->carte[pos], j->possibilite_carte[j->possibilite])!=0 && pos < 9)
    {
        pos += 1;
    }
    //la carte se trouvera à la pistion i

    //on va supprimer cette carte
    strcpy(j->carte[pos], "");
    //et décaler tout le paquet pour ne pas laisser de vide au milieu
    for (int i = pos; i+1 < 8; i += 1)
    {
        strcpy(j->carte[i], j->carte[i+1]);
        strcpy(j->carte[i+1], "");
    }
    return EXIT_SUCCESS;

}

void actualisation_carte_maitresse(Joueur *j, Jeu *partie)
{
        //regarde si la dame ou le roi de l'atout a été posé : sert uniquement pour la belote rebelote
    if (strcmp(partie->atout, &(partie->table[partie->ordre][2]))==0 && ((strncmp("Q", partie->table[partie->ordre],1)==0 || strncmp("R", partie->table[partie->ordre],1)==0)))
    {
        partie->compteur_belote_rebelote += 1;
    }

    int presence_atout = 0; //booleen 1 si presence atout
     //verifie si la carte posee est a l'atout
    if (strcmp(partie->atout, &(partie->table[partie->ordre][2]))==0)
    {
        presence_atout = 1;
    }

    if (strcmp(partie->carte_maitresse, "")==0)//si c'est le premier tour
    {
        strcpy(partie->carte_maitresse, partie->table[partie->ordre]);
        strcpy(partie->detenteur_carte_maitresse, j->nom);
    }
    else //sinon on va simplement venir compararer la carte posée avec la carte maitresse
    {
        if (presence_atout == 1)//si la carte posee est à l'atout
        {
            if (strcmp(&(partie->carte_maitresse[2]), partie->atout)==0) //si la carte maitresse est déja à l'atout
            {
                int val_maitress = valeur(partie->carte_maitresse, partie->atout);
                int val_pose = valeur(partie->table[partie->ordre], partie->atout);
                if (val_maitress < val_pose)
                {
                    strcpy(partie->carte_maitresse, partie->table[partie->ordre]);
                    strcpy(partie->detenteur_carte_maitresse, j->nom);
                }

            }
            else
            {
                strcpy(partie->carte_maitresse, partie->table[partie->ordre]);
                strcpy(partie->detenteur_carte_maitresse, j->nom);
            }
        }
        else // si la carte posee n'est pas à l'atout
        {
            if (strcmp(&(partie->carte_maitresse[2]), partie->atout) !=0) //si la carte maitresse n'est pas à l'atout
            {
                if (strcmp(&(partie->carte_maitresse[2]), &(partie->table[partie->ordre][2]))==0) //si la couleur de la carte posee est la meme que celle de la carte maitresse
                {
                    int val_maitress = valeur(partie->carte_maitresse, partie->atout);
                    int val_pose = valeur(partie->table[partie->ordre], partie->atout);
                    if (val_maitress < val_pose)
                    {
                        strcpy(partie->carte_maitresse, partie->table[partie->ordre]);
                        strcpy(partie->detenteur_carte_maitresse, j->nom);
                    }
                }
                //sinon on fait rien
            }
            //sinon on fait rien
        }
    }
}

void calcul_des_points(Jeu *partie)
{
    int total = 0, i = 0;
    while (i < 4 && strcmp(partie->table[i], "")!= 0)
    {
        int val = valeur(partie->table[i], partie->atout); //si la valeur est supérieure à zero on ajoute

        if (val > 0)
        {
            total += val;
        }
        i += 1;
    }
    partie->score_pli = total;

}

void attribution_des_points(Joueur *ordi_W, Joueur *ordi_N, Joueur *ordi_E, Joueur *joueur_S, Equipe *N_S, Equipe *E_W, Jeu *partie)//calcul les points des cartes sur la table
{
    //on attribue les points et le pli à l'équipe gagnante mais aussi au joueur qui a remporté le pli
    if (strcmp(partie->detenteur_carte_maitresse, "N")==0 || strcmp(partie->detenteur_carte_maitresse, "S")==0)
    {
        N_S->nb_pli += 1;
        N_S->score_manche += partie->score_pli;

        if (partie->pli == 7)// si c'est le dernier tour on accorde le dix de der
        {
            N_S->score_manche += 10;
            strcpy(partie->detenteur_dix_de_der, "N_S");
        }

        if (strcmp(partie->detenteur_carte_maitresse, "N")==0)
        {
            ordi_N->score_perso += partie->score_pli;
        }
        else
        {
            joueur_S->score_perso += partie->score_pli;
        }
    }
    else
    {
        E_W->nb_pli += 1;
        E_W->score_manche += partie->score_pli;

        if (partie->pli == 7)// si c'est le dernier tour on accorde le dix de der
        {
            E_W->score_manche += 10;
            strcpy(partie->detenteur_dix_de_der, "E_W");
        }

        if (strcmp(partie->detenteur_carte_maitresse, "E")==0)
        {
            ordi_E->score_perso += partie->score_pli;
        }
        else
        {
            ordi_W->score_perso += partie->score_pli;
        }
    }


}

int qui_commence(Jeu *partie)
{
    int ordre = 0;//en fonction du gagnant precedent, on va savoir quel est le numéro correspondant à l'ordre
    if (partie->tour == 0)
    {
        ordre = partie->ordre_de_jeu;
    }
    else
    {
        switch(partie->gagnant_precedent[0])
        {
                case 'W' : ordre = 0;
                    break;
                case 'N' : ordre = 1;
                    break;
                case 'E' : ordre = 2;
                    break;
                case 'S' : ordre = 3;
                    break;
        }
    }
    return ordre;
}


