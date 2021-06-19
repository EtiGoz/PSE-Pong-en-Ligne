#include "pse.h"

#define NB_THREADS 5
#define WIDTH 1280
#define HEIGHT 720
#define WIDTH_PLAYER 30
#define HEIGHT_PLAYER 150
#define PLAYER_SPEED 30
#define WIDTH_BALL 30
#define HEIGHT_BALL 30
#define BALL_SPEED 18

#define POSITION_X_J1 100
#define POSITION_X_J2 1150

#define CMD "serveur"

typedef struct Data
{
  int P1_y,P2_y;
  int score_P1,score_P2;
  int ball_x, ball_y;  
} Data;

typedef struct GamingRoom
{
  pthread_t idRoom;               /* identifiant du thread */
  pthread_t P1_checker; //thread listening to P1 messages. Allows to change P1 coordinates.
  pthread_t P2_checker;//thread listening to P2 messages. Allows to change P1 coordinates.
  int ret;                  /* indicateur de bonne lecture */
  int is_free;
  int new_canal; /*new player*/
/* ajouter donnees specifiques après cette ligne */
  int tid;                    /* identifiant logique */
  int socket_j1;                  /* canal de communication joueur1*/
  int socket_j2;                  /*canal de communication joueur2*/
  sem_t sem_Room;                  /* semaphore de reveil GamingRoom*/
  sem_t sem_P1_checker;
  sem_t sem_P2_checker;
  Data data; //Structure allowing data to be sent to clients. 
} GamingRoom;



void initBall(Data *data,int *direct_h, int *direct_v);
void updateBall(Data *data,int *direct_h, int *direct_v);
void *threadGamingRoom(void *arg);
void updatePlayer(int *player, int *direction);
void sendData(int sock_P1, int sock_P2, Data *data,GamingRoom *my_room);
void createCohorteWorkers();
void *threadP1Checker(void *arg);
void *threadP2Checker(void *arg);

GamingRoom room[NB_THREADS];

int continuer;
