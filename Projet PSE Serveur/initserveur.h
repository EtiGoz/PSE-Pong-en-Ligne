#include "pse.h"
#include <stdio.h>
#define NB_THREADS 5

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

