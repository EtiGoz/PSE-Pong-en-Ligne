#ifndef INIT_H 
#define INIT_H 

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <stdlib.h>

#define WIDTH 1280
#define HEIGHT 720
#define WIDTH_PLAYER 30
#define HEIGHT_PLAYER 150
#define VITESSE_PLAYER 12
#define WIDTH_BALL 30
#define HEIGHT_BALL 30
#define VITESSE_BALL 14

#define POSITION_X_J1 100
#define POSITION_X_J2 1150

SDL_Window *window;
SDL_Renderer *renderer;

int pursue;
int touche8,touche2,touche3,touche9;
SDL_Rect player1;
SDL_Rect player2;
SDL_Rect Balle;

int score_j1,score_j2;
SDL_Event event;

typedef struct Data
{
  int P1_y,P2_y;
  int score_P1,score_P2;
  int ball_x, ball_y;  
} Data;

void initialiser_video();
unsigned char listen_event(SDL_Event event) ;
#endif
