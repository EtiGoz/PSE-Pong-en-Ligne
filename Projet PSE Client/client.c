#include "pse.h"
#include "init.h"

#define CMD   "client"

char ligne[LIGNE_MAX];
int sock;
char  dest[20];

void initialiser_video();
char * extrairesouschaine(char*source,int pos,int len);

int main(int argc, char *argv[]) {
	short port;
  int ret, ecoute;
  struct sockaddr_in *adrServ;
  struct sockaddr_in adrEcoute;
  
  int lgLue;

  signal(SIGPIPE, SIG_IGN);


	port = (short)atoi(argv[1]);
	
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
  
  printf("%s: creating a socket\n", CMD);
  ecoute = socket(AF_INET, SOCK_STREAM, 0);
  if (ecoute < 0)
    erreur_IO("socket");
  
  adrEcoute.sin_family = AF_INET;
  adrEcoute.sin_addr.s_addr = INADDR_ANY;
  adrEcoute.sin_port = htons(port);
  printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
  ret = bind(ecoute, (struct sockaddr *)&adrEcoute, sizeof(adrEcoute));
  if (ret < 0)
    erreur_IO("bind");
  
  printf("%s: listening to socket\n", CMD);
  ret = listen(ecoute, 5);
  if (ret < 0)
    erreur_IO("listen");

  if (ret < 0)
    erreur_IO("connect");
  
  printf("%s: accepting a connection\n", CMD);

  initialiser_video();//Crée la fenêtre
    
	joueur1.w=WIDTH_PLAYER;
  joueur1.h=HEIGHT_PLAYER;
  joueur1.x=POSITION_X_J1;
    
  joueur2.w=WIDTH_PLAYER;
  joueur2.h=HEIGHT_PLAYER;
  joueur2.x=POSITION_X_J2;
	Balle.x=WIDTH/2;
  Balle.y=HEIGHT/2;
  Balle.w=WIDTH_BALL;
  Balle.h=HEIGHT_BALL;

	do{

      continuer=listen_event(event);

			strcpy(ligne, "1");

			if (ecrireLigne(sock, ligne) == -1)
       						erreur_IO("ecriture socket"); 
                   
			lgLue = read(sock, &joueur1.y,sizeof(int));
			lgLue = read(sock, &joueur2.y,sizeof(int));
			lgLue = read(sock, &Balle.x,sizeof(int));
			lgLue = read(sock, &Balle.y,sizeof(int));

      SDL_SetRenderDrawColor(renderer,0,0,0,255);
      SDL_RenderClear(renderer);
      SDL_RenderPresent(renderer);    


    	if (lgLue == -1)
      	erreur_IO("lecture canal");


      printf("reçu: P1: %d, P2: %d, ballx: %d, bally: %d.\n",joueur1.y,joueur2.y,Balle.x,Balle.y);
	  	SDL_SetRenderDrawColor(renderer,255,255,255,255);
      SDL_RenderFillRect(renderer,&joueur1);
      SDL_RenderFillRect(renderer,&joueur2);
      SDL_RenderFillRect(renderer,&Balle);
      SDL_RenderPresent(renderer);
      SDL_Delay(20); 
    }while(continuer==1);
      	/* Impératif pour quitter en sécurité. ----------------------------------------------------------------------------------------------------------*/
		if(NULL != renderer)
        SDL_DestroyRenderer(renderer);
    if(NULL != window)
        SDL_DestroyWindow(window);
    SDL_Quit();
    
    if (close(ecoute) == -1)
    	erreur_IO("fermeture ecoute");
    if (close(sock) == -1)
    	erreur_IO("fermeture socket");

		return EXIT_SUCCESS; 
}
  

unsigned char listen_event(SDL_Event event)//Permet de gérer l'appui et le relâchement des touches
{ 
	unsigned char continuer=1; 
	
	while(SDL_PollEvent(&event)) 
	{ 
        switch(event.type) 
            { 
                case SDL_QUIT: {continuer = 0;break;} 
                case SDL_KEYDOWN:
                {
                	switch(event.key.keysym.sym)
                	{
                		case SDLK_KP_2:{strcpy(ligne,"down on");break;}
                		case SDLK_KP_8:{strcpy(ligne,"up on");break;}

                		default:{ break;}
                	};break;
                }
                case SDL_KEYUP:
                {
                	switch(event.key.keysym.sym)
                	{
                		case SDLK_KP_2:{strcpy(ligne,"down off");break;}
                		case SDLK_KP_8:{strcpy(ligne,"up off");break;}
                		
                		default:{ break;}
                	};break;}
                	
								if (ecrireLigne(sock, ligne) == -1)
       						erreur_IO("ecriture socket"); 
       						
                default:{ break;} 
            }            
	} 
    return continuer; 
}



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
