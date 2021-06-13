#include "init.h"

void initialiserBalle()
{
    Balle.x=WIDTH/2;
    Balle.y=HEIGHT/2;
    Balle.w=WIDTH_BALL;
    Balle.h= HEIGHT_BALL;
    
    direct_h = rand()%2;
    direct_v = rand()%2;

}

void updateBalle()
{
    if (Balle.x<=0) //Si la balle atteint le but de J1
    {
        score_j2++;//J2 gagne 1 point
        initialiserBalle();//La balle est remise au centre
        printf("Score: J1: %d, J2: %d\n",score_j1,score_j2);
    }
    else if (Balle.x>=WIDTH) //Si la balle atteint le but de J2
    {
        score_j1++;  //J1 gagne 1 point
        initialiserBalle(); //La balle est remise au centre
        printf("Score: J1: %d, J2: %d\n",score_j1,score_j2);
    }

    //--------------------------------------------------------------------------------------------------------
    if(Balle.x<POSITION_X_J1+WIDTH_PLAYER && Balle.x>POSITION_X_J1)//Détection de collision avec le Joueur 1
    {
        if(Balle.y<joueur1.y+HEIGHT_PLAYER && Balle.y>joueur1.y)
        {
            direct_h=1;           
   
        }
        

    }
    else if(Balle.x>POSITION_X_J2-WIDTH_PLAYER && Balle.x<POSITION_X_J2)//Détection de collision avec le Joueur 2
    {
        if(Balle.y<joueur2.y+HEIGHT_PLAYER && Balle.y>joueur2.y)
        {
            direct_h=0;
        }
        

    }

    //--------------------------------------------------------------------------------------------------------
    if (Balle.y<=0) //Si la balle atteint le bord en bas
    {
        direct_v=1;//Elle rebondit vers le haut
    }
    else if (Balle.y>=HEIGHT) //Si la balle atteint le bord en haut
    {
        direct_v=0;//Elle rebondit vers le bas
    }

    if (direct_h)//Update de la position de la balle en horizontal, en fonction de sa trajectoire
    {
        Balle.x+=VITESSE_BALL;
    }
    else
    {
        Balle.x-=VITESSE_BALL;
    }
    if (direct_v)//Update de la position de la balle en vertical, en fonction de sa trajectoire
    {
        Balle.y+=VITESSE_BALL;
    }
    else
    {
        Balle.y-=VITESSE_BALL;
    }



}