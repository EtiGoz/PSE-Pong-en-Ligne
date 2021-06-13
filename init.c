#include "init.h"


void initialiser_video()
{
    /* Initialisation, création de la fenêtre et du renderer. --------------------------------------------------------------*/
    if(0 != SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Erreur SDL_Init : %s", SDL_GetError());
        if(NULL != renderer)
        	SDL_DestroyRenderer(renderer);
    	if(NULL != window)
        	SDL_DestroyWindow(window);
    	SDL_Quit();
    }
    
    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH,HEIGHT, SDL_WINDOW_SHOWN);
    if(NULL == window)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s", SDL_GetError());
        if(NULL != renderer)
        	SDL_DestroyRenderer(renderer);
    	if(NULL != window)
        	SDL_DestroyWindow(window);
    	SDL_Quit();
    }
    
    renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_ACCELERATED);
    
    if(NULL == renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
        if(NULL != renderer)
        	SDL_DestroyRenderer(renderer);
    	if(NULL != window)
        	SDL_DestroyWindow(window);
    	SDL_Quit();
    }
}