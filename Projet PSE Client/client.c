#include "pse.h"
#include "init.h"

#define CMD   "client"

int line;
int sock;
char  dest[20];
pthread_t receiver_t;
Data data;

void *threadReceiver(void *arg)
{
  int lgLue;
	while(1){
  	lgLue = read(sock, &data, sizeof(Data));
    player1.y=data.P1_y;
    player2.y=data.P2_y;
	  Balle.x=data.ball_x;
    Balle.y=data.ball_y;
    printf("P1: %d P2: %d ",data.P1_y)
    if (lgLue == -1)
    {
      printf("Server disconnected.\n");
      pursue=0;
    }
  }
}

void initialize_video();

int main(int argc, char *argv[]) {
  int ret;
  struct sockaddr_in *adrServ;
  

  signal(SIGPIPE, SIG_IGN);
	
  if (argc != 3)
    erreur("usage: %s machine port\n", argv[0]);

  printf("%s: creating a socket\n", CMD);
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    erreur_IO("socket");

  printf("%s: DNS resolving for %s, port %s\n", CMD, argv[1], argv[2]);
  adrServ = resolv(argv[1], argv[2]);
  if (adrServ == NULL)
    erreur("adresse %s port %s inconnus\n", argv[1], argv[2]);

  printf("%s: adr %s, port %hu\n", CMD,
	       stringIP(ntohl(adrServ->sin_addr.s_addr)),
	       ntohs(adrServ->sin_port));

  printf("%s: connecting the socket\n", CMD);
  ret = connect(sock, (struct sockaddr *)adrServ, sizeof(struct sockaddr_in));
  
  initialize_video();//Crée la fenêtre
    
	player1.w=WIDTH_PLAYER;
  player1.h=HEIGHT_PLAYER;
  player1.x=POSITION_X_J1;
    
  player2.w=WIDTH_PLAYER;
  player2.h=HEIGHT_PLAYER;
  player2.x=POSITION_X_J2;
	Balle.x=WIDTH/2;
  Balle.y=HEIGHT/2;
  Balle.w=WIDTH_BALL;
  Balle.h=HEIGHT_BALL;
  pthread_create(&receiver_t,NULL,threadReceiver,NULL);
	do{

      pursue=listen_event(event);
      line=1;

			ret=write(sock,&line,sizeof(int));
        if (ret<0)
        {
          printf("Server disconnected.\n");
          pursue=0;
        }


      SDL_SetRenderDrawColor(renderer,0,0,0,255);
      SDL_RenderClear(renderer);
      SDL_RenderPresent(renderer);    
	  	SDL_SetRenderDrawColor(renderer,255,255,255,255);
      SDL_RenderFillRect(renderer,&player1);
      SDL_RenderFillRect(renderer,&player2);
      SDL_RenderFillRect(renderer,&Balle);
      SDL_RenderPresent(renderer);
      SDL_Delay(16); 
    }while(pursue==1);
      	/* Impératif pour quitter en sécurité. ----------------------------------------------------------------------------------------------------------*/
		if(NULL != renderer)
        SDL_DestroyRenderer(renderer);
    if(NULL != window)
        SDL_DestroyWindow(window);
    SDL_Quit();
    

    if (close(sock) == -1)
    	erreur_IO("fermeture socket");

		return EXIT_SUCCESS; 
}
  

unsigned char listen_event(SDL_Event event)//Permet de gérer l'appui et le relâchement des touches
{ 
	unsigned char pursue=1; 
	int ret;
	while(SDL_PollEvent(&event)) 
	{ 
        switch(event.type) 
            { 
                case SDL_QUIT: {pursue= 0;break;} 
                case SDL_KEYDOWN:
                {
                	switch(event.key.keysym.sym)
                	{
                		case SDLK_KP_2:{line=2;ret=write(sock,&line,sizeof(int));break;}
                		case SDLK_KP_8:{line=3;ret=write(sock,&line,sizeof(int));break;}

                		default:{ break;}
                	};break;
                }                
                default:{ break;} 
            }            
	} 
    return pursue; 
}



void initialize_video()
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
