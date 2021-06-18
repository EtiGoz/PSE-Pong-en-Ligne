#include "pse.h"

#define NB_THREADS 5
#define WIDTH 1280
#define HEIGHT 720
#define WIDTH_PLAYER 30
#define HEIGHT_PLAYER 150
#define VITESSE_PLAYER 12
#define WIDTH_BALL 30
#define HEIGHT_BALL 30
#define VITESSE_BALL 20

#define POSITION_X_J1 100
#define POSITION_X_J2 1150

#define CMD "serveur"

typedef struct Salle_de_Jeu
{
  pthread_t idThread;               /* identifiant du thread */
  int libre;                  /* indicateur de terminaison */
/* ajouter donnees specifiques après cette ligne */
  int tid;                    /* identifiant logique */
  int socket_j1;                  /* canal de communication joueur1*/
  int socket_j2;                  /*canal de communication joueur2*/
  sem_t sem_j1;                  /* semaphore de reveil joueur1 */
  sem_t sem_j2;                 /*semaphore de reveil joueur2*/
} Salle_de_Jeu;

typedef struct Data
{
  int P1_y,P2_y;
  int score_P1,score_P2;
  int ball_x, ball_y;  
} Data;

void initBall(Data *data,int *direct_h, int *direct_v);
void updateBall(Data *data,int *direct_h, int *direct_v);
void *threadSalleJeu(void *arg);
void updatePlayer(int *player, int *direction);
void sendData(int sock_P1, int sock_P2, Data *data);

Salle_de_Jeu salle[NB_THREADS];

int continuer;