#include "initserveur.h"
#include <stdio.h>
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

#define CMD           "serveur"

pthread_mutex_t monmutex= PTHREAD_MUTEX_INITIALIZER;

Salle_de_Jeu salle[NB_THREADS];

int continuer;

void initBall(int *Ball_x,int *Ball_y,int *direct_h, int *direct_v);
void updateBall(int *Ball_x,int *Ball_y, int *P1_y, int *P2_y, int *score_P1, int *score_P2,int *direct_h, int *direct_v);
void initBall(int *Ball_x,int *Ball_y,int *direct_h, int *direct_v)
{
    *Ball_x=WIDTH/2;
    *Ball_y=HEIGHT/2;
    *direct_h = rand()%2;
    *direct_v = rand()%2;
}

void updateBall(int *Ball_x,int *Ball_y, int *P1_y, int *P2_y, int *score_P1, int *score_P2,int *direct_h, int *direct_v)
{
    if (*Ball_x<=0) //Si la balle atteint le but de J1
    {
        *score_P2+=1;//J2 gagne 1 point
        initBall(Ball_x,Ball_y,direct_h,direct_v);//La balle est remise au centre
        printf("Score: P1: %d, P2: %d\n",*score_P1,*score_P2);
    }
    else if (*Ball_x>=WIDTH) //Si la balle atteint le but de J2
    {
        *score_P1+=1;  //J1 gagne 1 point
        initBall(Ball_x,Ball_y,direct_h,direct_v);//La balle est remise au centre
        printf("Score: P1: %d, P2: %d\n",*score_P1,*score_P2);
    }
    //--------------------------------------------------------------------------------------------------------
    if(*Ball_x<POSITION_X_J1+WIDTH_PLAYER && *Ball_x>POSITION_X_J1)//Détection de collision avec le Joueur 1
    {
        if(*Ball_y<*P1_y+HEIGHT_PLAYER && *Ball_y>*P1_y)
        {
            *direct_h=1;           
   
        }
        

    }
    else if(*Ball_x>POSITION_X_J2-WIDTH_PLAYER && *Ball_x<POSITION_X_J2)//Détection de collision avec le Joueur 2
    {
        if(*Ball_y<*P2_y+HEIGHT_PLAYER && *Ball_y>*P2_y)
        {
            *direct_h=0;
        }
        

    }

    //--------------------------------------------------------------------------------------------------------
    if (*Ball_y<=0) //Si la balle atteint le bord en bas
    {
        *direct_v=1;//Elle rebondit vers le haut
    }
    else if (*Ball_y>=HEIGHT) //Si la balle atteint le bord en haut
    {
        *direct_v=0;//Elle rebondit vers le bas
    }

    if (*direct_h)//Update de la position de la balle en horizontal, en fonction de sa trajectoire
    {
        *Ball_x+=VITESSE_BALL;
    }
    else
    {
        *Ball_x-=VITESSE_BALL;
    }
    if (*direct_v)//Update de la position de la balle en vertical, en fonction de sa trajectoire
    {
        *Ball_y+=VITESSE_BALL;
    }
    else
    {
        *Ball_y-=VITESSE_BALL;
    }
}


void *threadSalleJeu(void *arg);
void creerCohorteWorkers(void)
{
    int ret;
    for(int i=0;i<NB_THREADS;i++)
    {   
        sem_init(&salle[i].sem_j1,0,0);
        sem_init(&salle[i].sem_j2,0,0);
        salle[i].socket_j1=-1;
        salle[i].socket_j2=-1;
        salle[i].tid=i;
        ret=pthread_create(&salle[i].idThread,NULL,threadSalleJeu,&salle[i]);
        if (ret!=0)
            erreur_IO("pthread_create");
    }
}

int placementPlayer(void)//Permet de placer un joueur. Retourne l'indice dans le tableau du thread libre.
{
    int cpt=0;
    int player_1_present,player_2_present;

    printf("Looking for a room with a player..\n");

    while(player_1_present==1 && player_2_present==1 && cpt<NB_THREADS)
    {
        sem_getvalue(&salle[cpt].sem_j1,&player_1_present);
        sem_getvalue(&salle[cpt].sem_j2,&player_2_present);
        cpt++;
    }
    printf("The room n°%d accepts the new player\n",cpt);
    if (cpt>=NB_THREADS)
        return -1;
    else
        return cpt;
}

void *threadSalleJeu(void *arg) 
{
    Salle_de_Jeu *my_room=(Salle_de_Jeu*)arg;
    char lineP1[LIGNE_MAX];
    char lineP2[LIGNE_MAX];
    int lP1,lP2;
    int P1_y,P2_y; //Players position
    P1_y=200;
    P2_y=100;
    int Ball_x,Ball_y; //Ball position
    int direct_h; //Si 0: La Balle va vers la gauche. Si 1: vers la droite
    int direct_v; //Si 0: La Balle va vers le bas. Si 1: vers le haut
    int score_P1, score_P2;
    score_P1=0;
    score_P2=0;
    char message[LIGNE_MAX];
    char *mP1_y="player 1 ";
    char *mP2_y="player 2 ";
    char *mBall_x="ballx ";
    char *mBall_y="bally ";
    char valP1_y[LIGNE_MAX];
    char valP2_y[LIGNE_MAX];
    char valBall_x[LIGNE_MAX];
    char valBall_y[LIGNE_MAX];

    printf("Room n°%d : turned on\n",my_room->tid);
    while (1) 
    {
        if (my_room->socket_j1==-1)
        {
            printf("Room n°%d : waiting for Player 1\n",my_room->tid);
            sem_wait(&my_room->sem_j1);
            sem_post(&my_room->sem_j1);
            printf("Room n°%d : Player 1 is here\n",my_room->tid);
        }
        if (my_room->socket_j2==-1)
        {
            printf("Room n°%d : waiting for Player 2\n",my_room->tid);
            sem_wait(&my_room->sem_j2);
            sem_post(&my_room->sem_j2);
            printf("Room n°%d : Player 2 is here\n",my_room->tid);
            printf("Launching Game.\n");
        }


        initBall(&Ball_x,&Ball_y,&direct_h,&direct_v);
        printf("Ballx: %d, Bally: %d, direction h : %d, direction v: %d\n",Ball_x,Ball_y,direct_h,direct_v);

        while(my_room->socket_j1>-1 && my_room->socket_j2>-1)//Tant que les joueurs sont connectés.
        {
            strcpy(message,"");

            lP1 = lireLigne(my_room->socket_j1, lineP1);
            lP2 = lireLigne(my_room->socket_j2, lineP2);

            if (lP1 == -1 || lP2 == -1)
                erreur_IO("wrong lecture");

            if (lP1 == 0 || lP2 == 0) 
            { // connexion fermee  => arret brutal du client
                printf("Room n°%d: A player left. Ending game.\n",my_room->tid);
                my_room->socket_j1=-1;
                my_room->socket_j2=-1;
                sem_wait(&my_room->sem_j1);
                sem_wait(&my_room->sem_j2);
            }
            updateBall(&Ball_x,&Ball_y,&P1_y,&P2_y,&score_P1,&score_P2,&direct_h,&direct_v);
            
            //écriture dans P1
            write(my_room->socket_j1,&P1_y,sizeof(int));
            write(my_room->socket_j1,&P2_y,sizeof(int));
            write(my_room->socket_j1,&Ball_x,sizeof(int));
            write(my_room->socket_j1,&Ball_y,sizeof(int));

            write(my_room->socket_j2,&P1_y,sizeof(int));
            write(my_room->socket_j2,&P2_y,sizeof(int));
            write(my_room->socket_j2,&Ball_x,sizeof(int));
            write(my_room->socket_j2,&Ball_y,sizeof(int));
            //printf("Sent: P1: %d, P2: %d, BallX: %d, Bally: %d\n",P1_y,P2_y,Ball_x,Ball_y);
            sleep(0.01);

        }
    }
}


int main(int argc, char *argv[]) {

    srand(time(NULL));   // Initialisation du timer pour le random. Should only be called once.    
    int valsem_p1,valsem_p2;
    int Salle_libre;
    short port;
    int ecoute, canal, ret;
    struct sockaddr_in adrEcoute, adrClient;
    unsigned int lgAdrClient;

    if (argc != 2)
        erreur("usage: %s port\n", argv[0]);

    port = (short)atoi(argv[1]);

    printf("%s: creating a socket\n", CMD);
    ecoute = socket (AF_INET, SOCK_STREAM, 0);
    if (ecoute < 0)
        erreur_IO("socket");
    
    adrEcoute.sin_family = AF_INET;
    adrEcoute.sin_addr.s_addr = INADDR_ANY;
    adrEcoute.sin_port = htons(port);
    printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
    ret = bind (ecoute,  (struct sockaddr *)&adrEcoute, sizeof(adrEcoute));
    if (ret < 0)
        erreur_IO("bind");
    
    printf("%s: listening to socket\n", CMD);
    ret = listen (ecoute, 5);
    if (ret < 0)
        erreur_IO("listen");
        
    creerCohorteWorkers();
    while (VRAI) 
    {
        printf("%s: accepting a connection\n", CMD);
        lgAdrClient = sizeof(adrClient);

        canal = accept(ecoute, (struct sockaddr *)&adrClient, &lgAdrClient);

        if (canal < 0)
            erreur_IO("accept");
        printf("%s: adr %s, port %hu\n", CMD,stringIP(ntohl(adrClient.sin_addr.s_addr)),ntohs(adrClient.sin_port));
        Salle_libre=placementPlayer();

        while (Salle_libre==-1)//Tant qu"aucune salle est libre
        {
            usleep(1000);
            Salle_libre=placementPlayer();
        }
        sem_getvalue(&salle[Salle_libre].sem_j1,&valsem_p1);
        sem_getvalue(&salle[Salle_libre].sem_j2,&valsem_p2);
        if (valsem_p1==1)
        {
            pthread_mutex_lock(&monmutex);//Ainsi seul le serveur peut modifier le canal à ce moment là
            salle[Salle_libre].socket_j2=canal;
            pthread_mutex_unlock(&monmutex);
            sem_post(&salle[Salle_libre].sem_j2);

        }
        else if(valsem_p2==0)
        {
            pthread_mutex_lock(&monmutex);//Ainsi seul le serveur peut modifier le canal à ce moment là
            salle[Salle_libre].socket_j1=canal;
            pthread_mutex_unlock(&monmutex);
            sem_post(&salle[Salle_libre].sem_j1);
        }
    }

    if (close(ecoute) == -1)
        erreur_IO("fermeture ecoute");  


    exit(EXIT_SUCCESS);
}


//----------------------------------------
