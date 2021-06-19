#include "initserveur.h"


pthread_mutex_t mymutex= PTHREAD_MUTEX_INITIALIZER;


void updatePlayer(int *player, int *direction){
	
	if (*direction==2 && *player+HEIGHT_PLAYER< HEIGHT) {
        *player+=PLAYER_SPEED;printf("%i", *direction);}
    if (*direction==3 && *player>0){
        *player-=PLAYER_SPEED;printf("%i", *direction);}
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
        //printf("Score: P1: %d, P2: %d\n",data->score_P1,data->score_P2);
    }
    else if (data->ball_x>=WIDTH) //Si la balle atteint le but de J2
    {
        data->score_P1+=1;  //J1 gagne 1 point
        initBall(data,direct_h,direct_v);//La balle est remise au centre
        //printf("Score: P1: %d, P2: %d\n",data->score_P1,data->score_P2);
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
        data->ball_x+=BALL_SPEED;
    }
    else
    {
        data->ball_x-=BALL_SPEED;
    }
    if (*direct_v)//Update de la position de la balle en vertical, en fonction de sa trajectoire
    {
        data->ball_y+=BALL_SPEED;
    }
    else
    {
        data->ball_y-=BALL_SPEED;
    }
}

void sendData(int sock_P1, int sock_P2, Data *data,GamingRoom *my_room)
{
            int ret;
            //écriture dans P1
            ret=write(sock_P1,data,sizeof(Data));
            if (ret < 0)
            {
                printf("Player 1 left\n");
                my_room->is_free=1;
            }
            //écriture dans P2
            ret=write(sock_P2,data,sizeof(Data));
            if (ret < 0)
            {
                printf("Ici ça bug Player 2 left\n");
                my_room->is_free=1;
            }


}                                

void createCohorteWorkers(void)
{
    int ret;
    for(int i=0;i<NB_THREADS;i++)
    {   
        sem_init(&room[i].sem_Room,0,0);
        sem_init(&room[i].sem_P1_checker,0,0);
        sem_init(&room[i].sem_P2_checker,0,0);
        room[i].socket_j1=-1;
        room[i].socket_j2=-1;
        room[i].tid=i;
        room[i].is_free=1;
        ret=pthread_create(&room[i].idRoom,NULL,threadGamingRoom,&room[i]);
        if (ret!=0)
            erreur_IO("pthread_create");
        pthread_create(&room[i].P1_checker,NULL,threadP1Checker,&room[i]);
        if (ret!=0)
            erreur_IO("pthread_create");
        pthread_create(&room[i].P2_checker,NULL,threadP2Checker,&room[i]); 
        if (ret!=0)
            erreur_IO("pthread_create");
    }
}

int placementPlayer(void)//Permet de placer un joueur. Retourne l'indice dans le tableau du thread free.
{
    int cpt=0;
    int player_1_present;
    sem_getvalue(&room[cpt].sem_Room,&player_1_present);
    printf("Room n°%d: Sem1: %d\n",cpt,player_1_present);
    printf("Looking for a room with a player..\n");

    while(room[cpt].is_free==0 && cpt<NB_THREADS)
    {
        cpt++;
    }
    printf("The room n°%d accepts the new player\n",cpt);
    if (cpt>=NB_THREADS)
        return -1;
    else
        return cpt;
}

void *threadGamingRoom(void *arg) 
{
    GamingRoom *my_room=(GamingRoom*)arg;

    int direct_h; //Si 0: La Balle va vers la gauche. Si 1: vers la droite
    int direct_v; //Si 0: La Balle va vers le bas. Si 1: vers le haut
    my_room->data.P1_y=200;
    my_room->data.P2_y=200;
    my_room->data.score_P1=0;
    my_room->data.score_P2=0;
    printf("Room n°%d : turned on\n",my_room->tid);
    while (1) 
    {
        if (my_room->socket_j1==-1)
        {
            printf("Room n°%d : waiting for Player 1...\n",my_room->tid);
            sem_wait(&my_room->sem_Room);
            printf("Room n°%d : Player 1 is here\n",my_room->tid);
            my_room->socket_j1=my_room->new_canal;
        }
        if (my_room->socket_j2==-1)
        {
            printf("Room n°%d : waiting for Player 2...\n",my_room->tid);
            sem_wait(&my_room->sem_Room);            
            printf("Room n°%d : Player 2 is here. Launching Game.\n",my_room->tid);
            my_room->socket_j2=my_room->new_canal;
        }
        my_room->is_free=0;
        initBall(&my_room->data,&direct_h,&direct_v);
        sem_post(&my_room->sem_P1_checker);
        sem_post(&my_room->sem_P2_checker);

        while(!my_room->is_free)//Tant que les joueurs sont connectés.
        {
            updateBall(&my_room->data,&direct_h,&direct_v);
            sendData(my_room->socket_j1,my_room->socket_j2,&my_room->data,my_room);
            usleep(33000);
        }
        my_room->socket_j1=-1;
        my_room->socket_j2=-1;
        printf("Room n°%d: A player left. Ending game.\n",my_room->tid);
    }
}

void *threadP1Checker(void *arg)
{
    GamingRoom *my_room=(GamingRoom*)arg;
    int dP1;
    int ret;
    int semP1;
    while(1)
    {
        sem_wait(&my_room->sem_P1_checker);
        sem_getvalue(&my_room->sem_P1_checker,&semP1);
        printf("Sem P1 checker: %d\n",semP1);
        while(ret!=0 || ret!=-1)
        {
            ret=read(my_room->socket_j1,&dP1,sizeof(int));
            updatePlayer(&my_room->data.P1_y, &dP1);
        }
        printf("Room n°%d: Player 1 left.\n",my_room->tid);
        my_room->is_free=1;
    }
}

void *threadP2Checker(void *arg)
{
    GamingRoom *my_room=(GamingRoom*)arg;
    int dP2;
    int ret;
    int semP2;
    while(1)
    {
        sem_wait(&my_room->sem_P2_checker);
        sem_getvalue(&my_room->sem_P2_checker,&semP2);
        printf("Sem P2 checker: %d\n",semP2);
        while(ret!=0 || ret!=-1)
        {
            ret=read(my_room->socket_j2,&dP2,sizeof(int));
            updatePlayer(&my_room->data.P2_y, &dP2);
        }
        printf("Room n°%d: Player 2 left.\n",my_room->tid);
        my_room->is_free=1;
    }
}

int main(int argc, char *argv[]) 
{
    srand(time(NULL));   // Initialisation du timer pour le random. Should only be called once.    
    int room_free;
    short port;
    int listener, canal, ret;
    struct sockaddr_in adrecoute, adrClient;
    unsigned int lgAdrClient;
    
    signal(SIGPIPE, SIG_IGN);

    if (argc != 2)
        erreur("usage: %s port\n", argv[0]);

    port = (short)atoi(argv[1]);

    printf("%s: creating a socket\n", CMD);
    listener = socket (AF_INET, SOCK_STREAM, 0);
    if (listener < 0)
        erreur_IO("socket");
    
    adrecoute.sin_family = AF_INET;
    adrecoute.sin_addr.s_addr = INADDR_ANY;
    adrecoute.sin_port = htons(port);
    printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
    ret = bind (listener,  (struct sockaddr *)&adrecoute, sizeof(adrecoute));
    if (ret < 0)
        erreur_IO("bind");
    
    printf("%s: listening to socket\n", CMD);
    ret = listen (listener, 5);
    if (ret < 0)
        erreur_IO("listen");
        
    createCohorteWorkers();
    while (VRAI) 
    {
        printf("%s: accepting a connection\n", CMD);
        lgAdrClient = sizeof(adrClient);

        canal = accept(listener, (struct sockaddr *)&adrClient, &lgAdrClient);

        if (canal < 0)
            erreur_IO("accept");
        printf("%s: adr %s, port %hu\n", CMD,stringIP(ntohl(adrClient.sin_addr.s_addr)),ntohs(adrClient.sin_port));
        room_free=placementPlayer();

        while (room_free==-1)//while no room is free
        {
            usleep(1000);
            room_free=placementPlayer();
        }
        pthread_mutex_lock(&mymutex);//Ainsi seul le serveur peut modifier le canal à ce moment là
        room[room_free].new_canal=canal;
        pthread_mutex_unlock(&mymutex);
        sem_post(&room[room_free].sem_Room);
    }

    if (close(listener) == -1)
        erreur_IO("fermeture listen");  

    exit(EXIT_SUCCESS);
}


//----------------------------------------
