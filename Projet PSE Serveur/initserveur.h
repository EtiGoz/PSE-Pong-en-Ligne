#include "pse.h"

#define NB_THREADS 5 //Permet de modifier le nombre de salles de jeu

#define WIDTH 1280 //Taille de la fenêtre de jeu en largeur
#define HEIGHT 720 //Taille de la fenêtre de jeu en hauteur
#define WIDTH_PLAYER 30
#define HEIGHT_PLAYER 150
#define PLAYER_SPEED 30
#define WIDTH_BALL 30
#define HEIGHT_BALL 30
#define BALL_SPEED 18

#define POSITION_X_P1 100
#define POSITION_X_P2 1150

#define CMD "Server"

typedef struct Data//Cette structure sera envoyée aux clients
{
  int P1_y,P2_y;
  int score_P1,score_P2;
  int ball_x, ball_y;  
} Data;

typedef struct GamingRoom//Cette structure permet de gérer les données dans une salle de jeu.
{
  pthread_t idRoom;               /* identifiant du thread */
  pthread_t P1_checker; //thread listening to P1 messages. Allows to change P1 coordinates.
  pthread_t P2_checker;//thread listening to P2 messages. Allows to change P1 coordinates.
  int ret;                  /* indicateur de bonne lecture */
  int is_free;
  int new_channel; /*new player*/
  int tid;                    /* identifiant logique */
  int socket_j1;                  /* canal de communication joueur1*/
  int socket_j2;                  /*canal de communication joueur2*/
  sem_t sem_Room;                  /* semaphore de reveil de salle de jeu*/
  sem_t sem_P1_checker;         /* semaphore de reveil d'écoute du joueur 1*/
  sem_t sem_P2_checker;         /* semaphore de reveil d'écoute du joueur 2*/
  Data data;
} GamingRoom;



void initBall(Data *data,int *direct_h, int *direct_v);//Initialisation de la balle, au centre. La direction est choisie aléatoirement.
void updateBall(Data *data,int *direct_h, int *direct_v);//Calcule la nouvelle position de la balle
void *threadGamingRoom(void *arg);//fonction s'occupant du bon déroulement d'une partie de jeu.
void updatePlayer(int *player, int *direction);//Met à jour la position du joueur en fonction de la touche appuyée.

void sendData(int sock_P1, int sock_P2, Data *data,GamingRoom *my_room);//Permet d'envoyer les scores, positions de joueurs et de balle aux clients.
void createGamingRooms(void);//Création des Salles de Jeu
void *threadP1Checker(void *arg);//"Oreille" de la salle de jeu: permet de recevoir la touche appuyée par le joueur 1
void *threadP2Checker(void *arg);//"Oreille" de la salle de jeu: permet de recevoir la touche appuyée par le joueur 2

GamingRoom room[NB_THREADS];

int continuer;
