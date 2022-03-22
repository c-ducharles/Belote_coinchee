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

void calcul_annonce(char paquet[8][5], Joueur *j, Jeu *partie)
{
    int i,compte_9=0,compte_10=0,compte_V=0,compte_D=0,compte_R=0,compte_A=0;//variable compteur pour les carrés
    int annonce=0;

    for (i=0;i<8;i++)
    {
        if (strncmp(paquet[i],"V",1)==0)//compte le nombre de valet dans le jeu
        {
            compte_V++;
        }
        else if (strncmp(paquet[i],"D",1)==0)//compte le nombre de dame dans le jeu
        {
            compte_D++;
        }
        else if (strncmp(paquet[i],"R",1)==0)//compte le nombre de roi dans le jeu
        {
            compte_R++;
        }
        else if (strncmp(paquet[i],"A",1)==0)//compte le nombre de as dans le jeu
        {
            compte_A++;
        }
        else if (strncmp(paquet[i],"9",1)==0)//compte le nombre de 9 dans le jeu
        {
            compte_9++;
        }
        else if (strncmp(paquet[i],"10",1)==0)//compte le nombre de 10 dans le jeu
        {
            compte_10++;
        }
    }

    annonce = annonce_suite(paquet, j);//on calcul d abord les annonces des suites de 3,4,5 cartes de la meme famille, et on garde seulement la meilleur

    if (strcmp(partie->atout, "SA")==0)
    {

        if (compte_D==4 || compte_R==4 || compte_V==4)//compte le nombre de carrés et attribut les points
        {
            annonce = 100;
            strcpy(j->quelle_annonce,"Carre de 4 cartes");
        }
        if (compte_10==4)//compte le nombre de 9 et attribut les points
        {
            annonce = 150;
            strcpy(j->quelle_annonce,"Carre de 10");
        }
        if (compte_A==4)//compte le nombre de valet et attribut les points
        {
            annonce = 200;
            strcpy(j->quelle_annonce,"Carre d'As");
        }
    }
    else
    {
        if (compte_D==4 || compte_R==4 || compte_A==4 || compte_10==4)//compte le nombre de carrés et attribut les points
        {
            annonce = 100;
            strcpy(j->quelle_annonce,"Carre de 4 cartes");
        }
        if (compte_9==4)//compte le nombre de 9 et attribut les points
        {
            annonce = 150;
            strcpy(j->quelle_annonce,"Carre de 9");
        }
        if (compte_V==4)//compte le nombre de valet et attribut les points
        {
            annonce = 200;
            strcpy(j->quelle_annonce,"Carre de V");
        }
    }

    j->annonce = annonce;
}

int annonce_famille(char temp[8][5], Joueur *j)//fonction qui va calculer les annonces possibles pour chaque suite de meme famille, pour chaque joueur
{
    int compteur_A=0,compteur_V=0,compteur_10=0,compteur_9=0,compteur_7=0,compteur_8=0,compteur_D=0,compteur_R=0;
    int valeur_annonce=0;
    int i=0;

    while (i < 8 && strcmp(temp[i], "")!= 0)
    {
        if(temp[i][0]=='7')
        {
            compteur_7++;
        }
        else if(temp[i][0]=='8')
        {
            compteur_8++;
        }
        else if(temp[i][0]=='9')
        {
            compteur_9++;
        }
        else if(temp[i][0]=='1')
        {
            compteur_10++;
        }
        else if(temp[i][0]=='V')
        {
            compteur_V++;
        }
        else if(temp[i][0]=='D')
        {
            compteur_D++;
        }
        else if(temp[i][0]=='R')
        {
            compteur_R++;
        }
        else if(temp[i][0]=='A')
        {
            compteur_A++;
        }
        i += 1;
    }

        if (((compteur_7 >= 1 && compteur_8 >= 1 && compteur_9 >= 1)||(compteur_8 >= 1 && compteur_9 >= 1 && compteur_10 >= 1)||(compteur_9 >= 1 && compteur_10 >= 1 && compteur_V >= 1)||(compteur_10 >= 1 && compteur_V >= 1 && compteur_D >= 1)||(compteur_V >= 1 && compteur_D >= 1 && compteur_R >= 1)|| (compteur_D >= 1 && compteur_R >= 1 && compteur_A >= 1))&& valeur_annonce <20)
        {
            valeur_annonce = 20;
            strcpy(j->quelle_annonce,"Suite de 3 cartes" );
        }
        if (((compteur_7 >= 1 && compteur_8 >= 1 && compteur_9 >= 1 && compteur_10 >= 1)||(compteur_8 >= 1 && compteur_9 >= 1 && compteur_10 >= 1 && compteur_V >= 1)||(compteur_9 >= 1 && compteur_10 >= 1 && compteur_V >= 1 && compteur_D >= 1)||(compteur_10 >= 1 && compteur_V >= 1 && compteur_D >= 1 && compteur_R >= 1)||(compteur_V >= 1 && compteur_D >= 1 && compteur_R >= 1 && compteur_A >= 1))&& valeur_annonce < 50)
        {
            valeur_annonce = 50;
            strcpy(j->quelle_annonce,"Suite de 4 cartes" );
        }
        if (((compteur_7 >= 1 && compteur_8 >= 1 && compteur_9 >= 1 && compteur_10 >= 1 && compteur_V >= 1)||(compteur_8 >= 1 && compteur_9 >= 1 && compteur_10 >= 1 && compteur_V >= 1 && compteur_D >= 1)||(compteur_9 >= 1 && compteur_10 >= 1 && compteur_V >= 1 && compteur_D >= 1 && compteur_R >= 1)||(compteur_10 >= 1 && compteur_V >= 1 && compteur_D >= 1 && compteur_R >= 1 && compteur_A >= 1)) && valeur_annonce < 100)
        {
            valeur_annonce = 100;
            strcpy(j->quelle_annonce,"Suite de 5 cartes");
        }

    return valeur_annonce;
}

int annonce_suite(char paquet[8][5], Joueur *j)//appelle la fonction annonce_famille, puis determine l annonce maximum qu un joueur peux avoir une une annonce de suite
{
    char temp1[8][5] = {"","","","","","","",""};
    char temp2[8][5] = {"","","","","","","",""};
    char temp3[8][5] = {"","","","","","","",""};
    char temp4[8][5] = {"","","","","","","",""};
    int i=0, k=0, valeur_annonce_T=0,valeur_annonce_Co=0,valeur_annonce_P=0,valeur_annonce_Ca=0,valeur_annoncemax=0;

    while (i<8 && strcmp(&paquet[i][2],"T")==0)//copie chaque cartes de la meme famille dans différentes variables temporaires
    {
        strcpy(temp1[k],paquet[i]);
        i++;
        k++;
    }
    k=0;
    while(i<8 && strcmp(&paquet[i][2],"Co")==0)
    {
        strcpy(temp2[k],paquet[i]);
        i++;
        k++;
    }
    k=0;
    while(i<8 && strcmp(&paquet[i][2],"P")==0)
    {
        strcpy(temp3[k],paquet[i]);
        i++;
        k++;
    }
    k=0;
    while(i<8 && strcmp(&paquet[i][2],"Ca")==0)
    {
        strcpy(temp4[k],paquet[i]);
        i++;
        k++;
    }
    k=0;
    valeur_annonce_T = annonce_famille(temp1, j);//calcul pour chaque famille, l annonce possible
    valeur_annonce_Co = annonce_famille(temp2, j);
    valeur_annonce_P = annonce_famille(temp3, j);
    valeur_annonce_Ca = annonce_famille(temp4, j);

    if (valeur_annonce_T > valeur_annonce_Co && valeur_annonce_T > valeur_annonce_P && valeur_annonce_T > valeur_annonce_Ca)//determine de l annonce des annonce parmis toutes les familles
    {
        valeur_annoncemax = valeur_annonce_T;
    }
    else if (valeur_annonce_T < valeur_annonce_Co && valeur_annonce_Co > valeur_annonce_P && valeur_annonce_Co > valeur_annonce_Ca)
    {
        valeur_annoncemax = valeur_annonce_Co;
    }
    else if (valeur_annonce_P > valeur_annonce_Co && valeur_annonce_T < valeur_annonce_P && valeur_annonce_P > valeur_annonce_Ca)
    {
        valeur_annoncemax = valeur_annonce_P;
    }
    if (valeur_annonce_Ca > valeur_annonce_Co && valeur_annonce_Ca > valeur_annonce_P && valeur_annonce_T < valeur_annonce_Ca)
    {
        valeur_annoncemax = valeur_annonce_Ca;
    }
    return valeur_annoncemax;

}
