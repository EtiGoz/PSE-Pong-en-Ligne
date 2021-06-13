#include "init.h"


int main(int argc, char *argv[])
{
    initialiser_video();//Crée la fenêtre
    srand(time(NULL));   // Initialisation du timer pour le random. Should only be called once.


    SDL_SetRenderDrawColor(renderer,255,255,255,255);//On dessine en blanc
    joueur1.w=WIDTH_PLAYER;
    joueur1.h=HEIGHT_PLAYER;
    joueur1.x=POSITION_X_J1;
    joueur1.y=200;
    touche2=0;
    touche8=0;

    joueur2.w=WIDTH_PLAYER;
    joueur2.h=HEIGHT_PLAYER;
    joueur2.x=POSITION_X_J2;
    joueur2.y=200;
    touche3=0;
    touche9=0;
    SDL_RenderFillRect(renderer,&joueur1);//On dessine J1
    SDL_RenderFillRect(renderer,&joueur2);//On dessine J2

    initialiserBalle();
    SDL_RenderFillRect(renderer,&Balle);//On dessine la balle
    SDL_RenderPresent(renderer);//On affiche notre dessin
    //---Gestion du score:
    score_j1=0;
    score_j2=0;
    continuer=1;
    do 
    {
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        continuer=listen(event);
        if (touche2==1 && joueur1.y+HEIGHT_PLAYER< HEIGHT) 
            joueur1.y+=VITESSE_PLAYER;
        else if (touche8==1 && joueur1.y>0)
            joueur1.y-=VITESSE_PLAYER;
        if (touche3==1 && joueur2.y+HEIGHT_PLAYER< HEIGHT) 
            joueur2.y+=VITESSE_PLAYER;
        else if (touche9==1 && joueur2.y>0)
            joueur2.y-=VITESSE_PLAYER;
        updateBalle();
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderFillRect(renderer,&joueur1);
        SDL_RenderFillRect(renderer,&joueur2);
        SDL_RenderFillRect(renderer,&Balle);
        SDL_RenderPresent(renderer);
        SDL_Delay(16); //une pause de 16 ms (pour faire du 60 fps)
    }while(continuer==1); 


	/* Impératif pour quitter en sécurité. ----------------------------------------------------------------------------------------------------------*/
	if(NULL != renderer)
        SDL_DestroyRenderer(renderer);
    if(NULL != window)
        SDL_DestroyWindow(window);
    SDL_Quit();
	return EXIT_SUCCESS; 
}

unsigned char listen(SDL_Event event)//Permet de gérer l'appui et le relâchement des touches
{ 
	unsigned char continuer=1; 
	while(SDL_PollEvent(&event)) 
	{ 
        switch(event.type) 
            { 
                case SDL_QUIT: {continuer = 0;break;} 
                case SDL_KEYDOWN:
                {
                	switch(event.key.keysym.sym)
                	{
                		case SDLK_KP_2:{touche2=1;break;}
                		case SDLK_KP_8:{touche8=1;break;}

                		case SDLK_KP_3:{touche3=1;break;}
                		case SDLK_KP_9:{touche9=1;break;}
                		default:{ break;}
                	};break;
                }
                case SDL_KEYUP:
                {
                	switch(event.key.keysym.sym)
                	{
                		case SDLK_KP_2:{touche2=0;break;}
                		case SDLK_KP_8:{touche8=0;break;}

                		case SDLK_KP_3:{touche3=0;break;}
                		case SDLK_KP_9:{touche9=0;break;}
                		default:{ break;}
                	};break;}

                default:{ break;} 
            } 
	} 
    return continuer; 
}