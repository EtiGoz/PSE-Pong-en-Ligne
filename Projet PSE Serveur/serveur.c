#include "initserveur.h"


pthread_mutex_t monmutex= PTHREAD_MUTEX_INITIALIZER;


void updatePlayer(int *player, int *direction){
	
	if (*direction==2 && *player+HEIGHT_PLAYER< HEIGHT) {
        *player+=VITESSE_PLAYER;printf("%i", *direction);}
    if (*direction==3 && *player>0){
        *player-=VITESSE_PLAYER;printf("%i", *direction);}
}

void initBall(Data *data,int *direct_h, int *direct_v)
{
    data->ball_x=WIDTH/2;
    data->ball_y=HEIGHT/2;
    *direct_h = rand()%2;
    *direct_v = rand()%2;
}

void updateBall(Data *data,int *direct_h, int *direct_v)
{
    if (data->ball_x <=0) //Si la balle atteint le but de J1
    {
        data->score_P2+=1;//J2 gagne 1 point
        initBall(data,direct_h,direct_v);//La balle est remise au centre
        printf("Score: P1: %d, P2: %d\n",data->score_P1,data->score_P2);
    }
    else if (data->ball_x>=WIDTH) //Si la balle atteint le but de J2
    {
        data->score_P1+=1;  //J1 gagne 1 point
        initBall(data,direct_h,direct_v);//La balle est remise au centre
        printf("Score: P1: %d, P2: %d\n",data->score_P1,data->score_P2);
    }
    //--------------------------------------------------------------------------------------------------------
    if(data->ball_x<POSITION_X_J1+WIDTH_PLAYER && data->ball_x>POSITION_X_J1)//Détection de collision avec le Joueur 1
    {
        if(data->ball_y<data->P1_y+HEIGHT_PLAYER && data->ball_y>data->P1_y)
        {
            *direct_h=1;           
   
        }
        
    }
    else if(data->ball_x>POSITION_X_J2-WIDTH_PLAYER && data->ball_x<POSITION_X_J2)//Détection de collision avec le Joueur 2
    {
        if(data->ball_y<data->P2_y+HEIGHT_PLAYER && data->ball_y>data->P2_y)
        {
            *direct_h=0;
        }
        
    }
    //--------------------------------------------------------------------------------------------------------
    if (data->ball_y<=0) //Si la balle atteint le bord en bas
    {
        *direct_v=1;//Elle rebondit vers le haut
    }
    else if (data->ball_y>=HEIGHT) //Si la balle atteint le bord en haut
    {
        *direct_v=0;//Elle rebondit vers le bas
    }

    if (*direct_h)//Update de la position de la balle en horizontal, en fonction de sa trajectoire
    {
        data->ball_x+=VITESSE_BALL;
    }
    else
    {
        data->ball_x-=VITESSE_BALL;
    }
    if (*direct_v)//Update de la position de la balle en vertical, en fonction de sa trajectoire
    {
        data->ball_y+=VITESSE_BALL;
    }
    else
    {
        data->ball_y-=VITESSE_BALL;
    }
}

void sendData(int sock_P1, int sock_P2, Data *data)
{
            int ret;
            //écriture dans P1
            ret=write(sock_P1,data,sizeof(Data));
            if (ret < 0)
                printf("Player 1 left\n");
            //écriture dans P2
            ret=write(sock_P2,data,sizeof(Data));
            if (ret < 0)
                printf("Player 2 left\n");

}                                

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
    sem_getvalue(&salle[cpt].sem_j1,&player_1_present);
    sem_getvalue(&salle[cpt].sem_j2,&player_2_present);
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
    int lP1,lP2;
    Data data; //Structure allowing data to be sent to clients. 
    int direct_h; //Si 0: La Balle va vers la gauche. Si 1: vers la droite
    int direct_v; //Si 0: La Balle va vers le bas. Si 1: vers le haut
    data.P1_y=200;
    data.P2_y=200;
    data.score_P1=0;
    data.score_P2=0;
    printf("Room n°%d : turned on\n",my_room->tid);
    while (1) 
    {
        if (my_room->socket_j1==-1)
        {
            printf("Room n°%d : waiting for Player 1...\n",my_room->tid);
            sem_wait(&my_room->sem_j1);
            sem_post(&my_room->sem_j1);
            printf("Room n°%d : Player 1 is here\n",my_room->tid);
        }
        if (my_room->socket_j2==-1)
        {
            printf("Room n°%d : waiting for Player 2...\n",my_room->tid);
            sem_wait(&my_room->sem_j2);            
            sem_post(&my_room->sem_j2);
            printf("Room n°%d : Player 2 is here. Launching Game.\n",my_room->tid);
        }
        initBall(&data,&direct_h,&direct_v);
        printf("Ballx: %d, Bally: %d, direction h : %d, direction v: %d\n",data.ball_x,data.ball_y,direct_h,direct_v);

        while(my_room->socket_j1>-1 && my_room->socket_j2>-1)//Tant que les joueurs sont connectés.
        {
            read(my_room->socket_j1,&lP1,sizeof(int));
            read(my_room->socket_j2,&lP2,sizeof(int));
            
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

			updatePlayer(&data.P1_y, &lP1);
			updatePlayer(&data.P2_y, &lP2);
            updateBall(&data,&direct_h,&direct_v);
            sendData(my_room->socket_j1,my_room->socket_j2,&data);

            //printf("Sent: P1: %d, P2: %d, BallX: %d, Bally: %d\n",P1_y,P2_y,Ball_x,Ball_y);
            sleep(0.016);

        }
    }
}


int main(int argc, char *argv[]) 
{

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
