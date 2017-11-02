/*                                                                                                                             
 * guideagent.c
 *                                                                                                                             
 * 
 *                                                                                                                             
 * usage: ./guideagent guideId=... team=... player=... host=... port=...
 *  
 *
 * CS50                                                                                                                            
 * Group 9 Eleph, April 2017
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>           // read, write, close                                                                            
#include <string.h>           // strlen                                                                                        
#include <strings.h>          // bcopy, bzero

#include <netdb.h>	      // socket-related structures
#include <arpa/inet.h>        // socket-related calls                                                                          
#include <sys/select.h>       // select-related stuff                                                                          
#include <ctype.h>
#include <ncurses.h>          // ncurses 
#include <stdlib.h>
#include <unistd.h>

#include "GAstats.h"
#include "file.h"
#include "message.h"
#include "opCode_handler.h"
#include "set.h"

/* struct that holds the fields for the                                                                                        
 * GA arguments                                                                                                       
 *                                                                                                                             
 */
typedef struct GA_args {
  unsigned int guideID;
  char* team;
  char* player;
  char* host;
  int port;
} GA_args_t;

GA_args_t* GA_args_new(void){
  GA_args_t* a = malloc(sizeof(GA_args_t));
  if (a == NULL){
    return NULL;
  }
  a->guideID=0; // NOT SURE IF THIS SHOULD BE 0
  a->team= NULL;
  a->player = NULL;
  a->host = NULL;
  a->port = -99;
  return a;
}





typedef struct playerID {
  unsigned int playerID;
  char* playername;
  GA_stats_t * ga_stats;
} player_ID_t;

player_ID_t* player_ID_new(void){
  player_ID_t* p = malloc(sizeof(player_ID_t));
  if (p == NULL){
    return NULL;
  }
  p->playerID=0; // NOT SURE IF THIS SHOULD BE 0
  p->playername = NULL;
  p->ga_stats = NULL;
  return p;
}

//          set_insert(GA_get_kragId_to_clue(g), kragID, c);



typedef struct location {
  WINDOW *window;
  GA_stats_t * stats;
} loc_t;


loc_t* location_t_new(void){
  loc_t* loc = malloc(sizeof(loc_t));
  if (loc == NULL){
    return NULL;
  }

  loc->window = NULL;
  loc->stats = NULL;
  return loc;
}


static void printunsolved(void* arg, const char* key, void* item){
      clue_bool_t* c = item;
      bag_t* bag = arg;
      if (get_solved_bool(c) == false){
        bag_insert(bag, get_unsolved_clue(c));
      }
    }

typedef struct bag_stats {
  bag_t* bag;
  GA_stats_t* stats;
} bag_stats_t;

bag_stats_t* bag_stats_new(bag_t* bag, GA_stats_t* stats){
  bag_stats_t* b = malloc(sizeof(bag_stats_t));
  if (b !=  NULL){
    b->bag = bag;
    b->stats = stats;
    return b;

  }
  return NULL;
}


static void fillbag(void* arg, const char* key, void* item){
  unsigned long p = strtoul(key,NULL,16);
  bag_stats_t* b = arg;
  char* playername = player_get_name(p, b->stats);
  bag_insert(b->bag, playername);
}


static void simpleiterate(void *arg, const char *key, void *item){
  int *nitems = arg;
  //  if (nitems != NULL && key != NULL  && item != NULL)
    (*nitems)++;
}


static int socket_setup(const int argc, char *argv[], struct sockaddr_in *sAddress, GA_args_t *args);
static void handle_socket(int comm_sock, struct sockaddr_in *addressp, GA_args_t *args, GA_stats_t * stats,  WINDOW *a, WINDOW *b,WINDOW *c, int maxx, int maxy, int quartery);
static int handle_stdin(int comm_sock, struct sockaddr_in *addressp, GA_args_t *args, GA_stats_t * stats, WINDOW *c,int maxx, int maxy, int quartery);
static int send_GA_stats_req(int comm_sock, struct sockaddr_in *addressp, GA_args_t *args, GA_stats_t * stats);
void parse_args(int argc ,char* argv[], GA_args_t* a);
void init_ncurses();
void  find_player_ID(void *arg, const char *key, void *item);
void update_player_locations(GA_stats_t * stats, WINDOW *a);
void find_player_loc(void *arg, const char *key, void *item);
loc_t* location_t_new(void);


static int BUFSIZE= 1024;
static float ULNG = -72.285254;
static float LLNG= -72.294058;
static float ULAT = 43.70261;
static float LLAT= 43.70707;
static int MMAXX= 114;
static int MMAXY= 40;



int main(const int argc, char *argv[])
{
  
  int maxx, maxy, quarterx, quartery;
  WINDOW *a, *b, *c;
  
  init_ncurses();

  
  getmaxyx(stdscr, maxy, maxx);
  quarterx = maxx >> 2;
  quartery = maxy >> 2;

  //create four windows to fill the screen 
  if( (a = newwin(40,114,0,0))== NULL);//3.4*quartery, 3*quarterx, 0, 0)) == NULL);              
  if (LINES< 50 || COLS<145){
    printw( "PLEASE ADJUST YOUR TERMINAL WINDOW to be >70 rows && >165 columns");
    refresh();
    sleep(5);
    endwin();
    exit(1);
  }

   if( (b = newwin(40, maxx, 0,114))==NULL);//3.4*quartery, maxx, 0, 3*quarterx)) == NULL);      
   if( (c = newwin(maxy-40,maxx,40,0))==NULL); //maxy, maxx, 3.4*quartery, 0)) == NULL); 

  mvwaddstr(a, 0, 0, "Campus map:\n");
  FILE *fp= fopen("map.txt", "r");
  int ch;
  move(0,0);

  while((ch = fgetc(fp)) != EOF)        /* read the file till we reach the end */
    {
      printw("%c", ch);
      refresh();
    }
  fclose(fp);


  
  //wbkgd(a, COLOR_PAIR(1));
  //wrefresh(a);

  // Give some input that will be printed on window b
  //mvwaddstr(b, 0, 0, "Game stats:\n");
  //mvwaddstr(b, 1, 3, "Guide ID is   ");                                                                                    
  wbkgd(b, COLOR_PAIR(2));
  wrefresh(b);

  mvwaddstr(c, 0, 0, "Enter a hint for your field agents or 'status' for a status update:\n");

  
  wbkgd(c, COLOR_PAIR(3));
  wrefresh(c);
  
  //mvwaddstr(c, 2, 0, "2nd line printed to the screen:\n");
  //wbkgd(c, COLOR_PAIR(3));
  //wrefresh(c);
  
  //  fprintf(stdout, "Enter 's' to receive a game status update.\n");
  
  //parse command line                                                                                                     
  GA_args_t *ga_args = GA_args_new();
  parse_args(argc, argv, ga_args);

 
  // the server's address
  struct sockaddr_in sAddress;

  
  // set up a socket on which to communicate
  int comm_sock = socket_setup(argc, argv, &sAddress, ga_args);

  
  //set up ga_stats
  GA_stats_t* ga_stats = GA_stats_new();

  set_team_name(ga_args->team, ga_stats);
  

  
  //socket_struct_t *sock_struct = socket_struct_new();
  //set_address(sock_struct, &sAddress);
  // set_comm_socket_ga(sock_struct, comm_sock);



  //get args in struct from pasrse args

  //create GA stats and fill it in with any args and send this every 15 secs in while loop
  
  send_GA_stats_req(comm_sock, &sAddress, ga_args, ga_stats);

  // read from either the socket or stdin, whichever is ready first;
  // if stdin, read a line and send it to the socket;
  // if socket, receive message from socket and write to stdout.

  while (true) {	      // loop exits on EOF from stdin
    // for use with select()
    fd_set rfds;	      // set of file descriptors we want to read
    
    // Watch stdin (fd 0) and the UDP socket to see when either has input.
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);	      // stdin
    FD_SET(comm_sock, &rfds); // the UDP socket
    int nfds = comm_sock+1;   // highest-numbered fd in rfds

    // Wait for input on either source
    int select_response = select(nfds, &rfds, NULL, NULL, NULL);
    // note: 'rfds' updated
    
    if (select_response < 0) {
      // some error occurred
      perror("select()");
      exit(9);
    } else if (select_response == 0) {
      // timeout occurred; this should not happen
    } else if (select_response > 0) {
      // some data is ready on either source, or both

      
      if (FD_ISSET(0, &rfds)) {
	if (handle_stdin(comm_sock, &sAddress, ga_args, ga_stats,c, maxx, maxy, quartery) == EOF) {
	// fprintf(stdout, "here in stdin EOF\n");
	 break; // exit loop if EOF on stdin
	}
      }
    

      if (FD_ISSET(comm_sock, &rfds)) {
	handle_socket(comm_sock, &sAddress, ga_args, ga_stats, a,b,c, maxx, maxy, quartery);
	
      }

    
  }

  }


  close(comm_sock);

    return 0;
    
}



/**************** socket_setup ****************/
/* Parse the arguments and set up our socket.
 * Exit on any error of arguments or socket setup.
 */


 
static int socket_setup(const int argc, char *argv[], struct sockaddr_in *sAddress, GA_args_t* ga_args)
{
  char *program;	      // this program's name
  char *hostname;	      // server hostname
  int port;		      // server port

  // check arguments
  program = argv[0];
  if (argc != 6) {
    fprintf(stderr, "usage: %s hostname port\n", program);
    exit(1);
  } else {
    hostname = ga_args-> host;
    port = ga_args-> port;
  }

  // Look up the hostname specified on command line
  struct hostent *hostp = gethostbyname(hostname);
  if (hostp == NULL) {
    fprintf(stderr, "%s: unknown host '%s'\n", program, hostname);
    exit(3);
  }

  // Initialize fields of the server address
  sAddress->sin_family = AF_INET;
  bcopy(hostp->h_addr_list[0], &sAddress->sin_addr, hostp->h_length);
  sAddress->sin_port = htons(port);

  // Create socket
  int comm_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (comm_sock < 0) {
    perror("opening datagram socket");
    exit(2);
  }

  return comm_sock;
}


/********************* handle_socket **************************/
/* Sends an incoming datagram to parser                                         
 * to parse the message and then sends                                          
 * message to do_Opcode to handle the opCode function                            
 * exits if nothing is received in parser                                       
 */
/**************** handle_socket ****************/
/* Socket has input ready; receive a datagram                                                                                                 
 * 'addressp' should be a valid address representing the expected sender.                                                                     
 * Exit on any socket error.                                                                                                                  
 */

  
static void handle_socket(int comm_sock, struct sockaddr_in *sAddress, GA_args_t *args, GA_stats_t * stats, WINDOW *a, WINDOW *b, WINDOW *c, int maxx, int maxy, int quartery)
{
  // fprintf(stdout, "NOW IN HANDLE_SOCKET of guideagent\n");
// socket has input ready
  struct sockaddr_in sender;		 // sender of this message
  struct sockaddr *senderp = (struct sockaddr *) &sender;
  socklen_t senderlen = sizeof(sender);  // must pass address to length
  char buf[BUFSIZE];	      // buffer for reading data from socket
  memset(buf,'\0', BUFSIZE);

  int nbytes = recvfrom(comm_sock, buf, BUFSIZE-1, 0, senderp, &senderlen);

  if (nbytes < 0) {
    perror("receiving from socket");
    exit(1);
  } else {
    buf[nbytes] = '\0';     // null terminate string

    // where was it from?
    if (sender.sin_family != AF_INET) {
      printf("From non-Internet address: Family %d\n", sender.sin_family);
    } else {
      // was it from the expected server?
      if (sender.sin_addr.s_addr == sAddress->sin_addr.s_addr && 
	  sender.sin_port == sAddress->sin_port) {

	/*		printf("[%s@%05d]: %s\n", 
	       inet_ntoa(sender.sin_addr),
	       ntohs(sender.sin_port), 
	       buf);
	*/
	
	
	//create message struct to put parse message into
	message_t *message = message_new();
	message_parse(message,buf);	
	do_opCode(stats, message);
	
      } else {
	printf("[%s@%05d]: INTERLOPER! %s\n",
	       inet_ntoa(sender.sin_addr),
	       ntohs(sender.sin_port),
	       buf);
      }
    }
    //    fflush(stdout);
  }
  //  fprintf(stdout, "NOW LEAVING HANDLE_SOCKET of guideagent\n");

  update_player_locations (stats, a);
   if( (b = newwin(40, maxx, 0,114))==NULL);//3.4*quartery, maxx, 0, 3*quarterx)) == NULL); 
   wbkgd(b, COLOR_PAIR(2));
   wrefresh(b);
   mvwaddstr(b, 0, 3, "Game stats ");
   wprintw(b, "for team %s:\n\n", GA_get_teamname(stats));

  
    wprintw(b, "Game ID: %u \n", GA_get_gameId(stats));
    int nitems = 0;                                                                                       
    set_iterate(GA_get_players(stats), &nitems, simpleiterate);                                           
    wprintw(b, "size of PLAYERS set is %d\n\n", nitems);

    wprintw(b, "Active field agents: \n");


    bag_t* FAs = bag_new();
    bag_stats_t* bs = bag_stats_new(FAs, stats);
    set_iterate(GA_get_players(stats), bs, fillbag);
    char* FA = bag_extract(bs->bag);
    while (FA != NULL){
      wprintw(b, "   %s\n", FA);
      FA = bag_extract(bs->bag);
    }
    
    wprintw(b, "# of krags: %d\n", GA_get_numKrags(stats));
    wprintw(b, "# of krags claimed: %d\n", GA_get_numClaimed(stats));
    wprintw(b, "Unsolved clues:\n");
    
    bag_t* clues_to_print = bag_new();
    
    // int nitems2 = 0;
    //set_iterate(GA_get_kragId_to_clue(stats), &nitems2, simpleiterate);                                   
    //   wprintw(b, "size of clues set is %d", nitems2);                                                       
    set_iterate(GA_get_kragId_to_clue(stats), clues_to_print, printunsolved);
    char* clue = bag_extract(clues_to_print);
    wrefresh(b);

    while(clue != NULL){
      wprintw(b, "   %s\n", clue);
      clue = bag_extract(clues_to_print);
    }
    
    
    wprintw(b, "Decoded secret: %s\n", GA_get_secret(stats));
    wrefresh(b);

    if( (c = newwin(maxy-40,maxx,40,0))==NULL); //maxy, maxx, 3.4*quartery, 0)) == NULL);  
    wbkgd(c, COLOR_PAIR(3));
    mvwaddstr(c, 0, 0,"Enter a hint  or 'status' for a status update:\n");

    wrefresh(c);


  
}
 
 
/**************** handle_stdin ****************/
/* stdin has input ready; read a line and send it to the client.
 * return EOF if EOF was encountered on stdin;
 * return 0 if there is no client to whom we can send;
 * return 1 if message sent successfully.
 * exit on any socket error.
 */

/*
static int handle_stdin(int comm_sock, struct sockaddr_in *addressp, GA_args_t *args, GA_stats_t * stats, WINDOW *c, int maxx, int maxy, int quartery)
{
  //fprintf(stdout, "NOW IN HANDLE_STDIN of guideagent\n");

  //char *response = readlinep(stdin);
  //if (response == NULL)

  
  //return EOF;
  
  if( (c = newwin(maxy, maxx, 3.4*quartery, 0)) == NULL);  
  // WINDOW *c;
  char response[141];

   wbkgd(c, COLOR_PAIR(3));
   wgetnstr(c, response, 140);
  
  //convert to lowercase
  //copy string

  char * input = malloc((strlen(response)+1)* sizeof(char));
  
  message_t *message = message_new();
  char * toSend;
  

  
  for(int i = 0; response[i] != '\0'; i++){
    input[i] = tolower(response[i]);
    
  }

  //request update from server
  if (strcmp(input,"status")==0){
    wprintw(c,"status request received\n");
    //wprintw(c,"status request received\n",response);
    wrefresh(c);
    
    send_GA_stats_req(comm_sock,addressp, args, stats);

    sleep(1.5);
    mvwprintw(c,0,0,"\r                                                                                                                                                                                                                                                       ");
    mvwprintw(c,1,0,"\r                                                                                                                                                                                                                                                       ");

    wrefresh(c);
  }
  
  //send hint to all
  else{
    wprintw(c,"hint received\n",response);

    // char n[80];
    //char name[80];
    

    bool found =false;


    wprintw(c,"Enter one player's name to send the hint to or 'all' to broadcast or 'status' for a status update:\n",response);
    
    while (found == false){
      char *n= (char*) calloc(80,sizeof(char));
      char *name=(char*) calloc(80,sizeof(char));

      wgetnstr(c, name, 80);

      // printf("name: %s\n", name);
      
      for(int i = 0; name[i] != '\0'; i++){
	n[i] = tolower(name[i]);

      }

      //printf("lowercase name: %s\n", n);
      if (strcmp(n, "all")==0){
	set_pebbleId(message,'*');
	found =true;
      }

      else {
	player_ID_t* player = player_ID_new();
	player->playername= n;
	player->ga_stats = stats;
	
	set_t *pSet= GA_get_players(stats);
	set_iterate(pSet,player,find_player_ID);
	if (player->playerID != 0){
	  found =true;
	  set_pebbleId(message,player->playerID);
	}

	if (found == false){
	 mvwaddstr(c, 0, 0, "Enter one player's name to send the hint to or 'all' to broadcast" );
	}
	
      }
      sleep(1.5);
      mvwprintw(c,3,0,"\r                                                                                                ");
      mvwprintw(c,1,0,"\r                                                                                                ");
      mvwprintw(c,2,0,"\r                                                                                                ");

      wbkgd(c, COLOR_PAIR(3));

      wrefresh(c);
      free(n);
      free(name);
    }
    
    
    wbkgd(c, COLOR_PAIR(3));
    
    wrefresh(c);
    unsigned int gId= GA_get_gameId(stats);
    

    //pack a message
    set_opCode(message,"GA_HINT");
    set_gameId(message,gId);
    set_guideId(message,args->guideID);
    set_team(message,args->team);
    set_player(message,args->player);
    set_hint(message,response);

    toSend= message_to_string(message);
    //fprintf(stdout, "string to send to server at pointer: %s",toSend);
    sleep(1.5);
    mvwprintw(c,0,0,"\r                                                                                                                                                                                                                                                       ");
    mvwprintw(c,1,0,"\r                                                                                                                                                                                                                                                       ");


     mvwprintw(c,2,0,"\r                                                                                                ");
   
    
  

    if (addressp->sin_family != AF_INET) {
      printf("I am confused: server is not AF_INET.\n");
      fflush(stdout);
      return 0;
    } 
    
     if (sendto(comm_sock, toSend, strlen(toSend), 0, 
    	       (struct sockaddr *) addressp, sizeof((*((struct sockaddr *) addressp)))) < 0) {
    perror("sending in datagram socket");
    exit(6);
     }

    //if (message != NULL){                                                                                                                                                 
    //message_delete(message);                                                                                                                                                
    //}                       
  }

   wbkgd(c, COLOR_PAIR(3));
    mvwaddstr(c, 0, 0,"Enter one player's name to send the hint to or 'all' to broadcast or 'status' for a status update:\n");

    wrefresh(c);

  //free(response);
  //  if (message != NULL){
  //message_delete(message);
  //}

  //fprintf(stdout, "NOW LEAVING HANDLE_STDIN of guideagent\n");
  return 1;
}
  */
  

  static int handle_stdin(int comm_sock, struct sockaddr_in *addressp, GA_args_t *args, GA_stats_t * stats, WINDOW *c, int maxx, int maxy, int quartery)
{
  //fprintf(stdout, "NOW IN HANDLE_STDIN of guideagent\n");                                                                               

  //char *response = readlinep(stdin);                                                                                                    
  //if (response == NULL)                                                                                                                 


  //return EOF;                                                                                                                           

  if( (c = newwin(maxy-40,maxx,40,0))==NULL);
  //if( (c = newwin(maxy, maxx, 3.4*quartery, 0)) == NULL);
  // WINDOW *c;                                                                                                                           
  char response[141];

   wbkgd(c, COLOR_PAIR(3));
   wgetnstr(c, response, 140);

  //convert to lowercase                                                                                                                  
  //copy string                                                                                                                           

  char * input = malloc((strlen(response)+1)* sizeof(char));

  message_t *message = message_new();
  char * toSend;
  strcpy(input, response);
  for(int i = 0; input[i]; i++){
        input[i] = tolower(input[i]);
  }

  

  //request update from server                                                                                                            
  if (strcmp(input,"status")==0){
    wprintw(c,"status request received\n");
    //wprintw(c,"status request received\n",response);                                                                                    
    wrefresh(c);

    send_GA_stats_req(comm_sock,addressp, args, stats);

    sleep(1.5);
    mvwprintw(c,0,0,"\r                                                                                                                  \
                                                                                                                                     ");
    mvwprintw(c,1,0,"\r                                                                                                                  \
                                                                                                                                     ");

    wrefresh(c);
  }

  //send hint               
  else{
    wprintw(c,"hint received\n",response);
    
    // char n[80];                                                                                                                        
    //char name[80];                                                                                                                      
    
    
    bool found =false;
    
    
    wprintw(c,"Enter one player's name to send the hint to or 'all': \n",response);
    
    while (found == false){
      
      char *name=(char*) calloc(80,sizeof(char));
      
      wgetnstr(c, name, 80);

      
      int i;      
      for(i = 0; name[i]; i++){
	
	  name[i] = tolower(name[i]);
	
      }

      
      
      
      //printf("lowercase name: %s\n", n);                                                                                                
      if (strcmp(name, "all")==0){
        set_pebbleId(message,'*');
        found =true;
      }
      
      else {
        player_ID_t* player = player_ID_new();
        player->playername= name;
        player->ga_stats = stats;
	
        set_t *pSet= GA_get_players(stats);
        set_iterate(pSet,player,find_player_ID);
        if (player->playerID != 0){
          found =true;
          set_pebbleId(message,player->playerID);
        }
	
        if (found == false){
	  mvwaddstr(c, 0, 0, "Enter one player's name to send the hint to or 'all' to broadcast" );
        }
	
      }
      sleep(1.5);   
      mvwprintw(c,3,0,"\r                                                                                                                                              ");
      mvwprintw(c,1,0,"\r                                                                                                                                              ");
      mvwprintw(c,2,0,"\r                                                                                                                                              ");
 
      mvwprintw(c,4,0,"\r                                                                                                                                               ");

      
      wbkgd(c, COLOR_PAIR(3));
      
      wrefresh(c);
      
      free(name);
    }


    wbkgd(c, COLOR_PAIR(3));

    wrefresh(c);
    unsigned int gId= GA_get_gameId(stats);


    //pack a message                                                                                                                      
    set_opCode(message,"GA_HINT");
    set_gameId(message,gId);
    set_guideId(message,args->guideID);
    set_team(message,args->team);
    set_player(message,args->player);
    set_hint(message,response);

    toSend= message_to_string(message);
    //fprintf(stdout, "string to send to server at pointer: %s",toSend);                                                                  
    sleep(1.5);
    mvwprintw(c,0,0,"\r                                                                                                                  \
                                                                                                                                     ");
    mvwprintw(c,1,0,"\r                                                                                                                  \
                                                                                                                                     ");


     mvwprintw(c,2,0,"\r                                                                                                ");

    if (addressp->sin_family != AF_INET) {
      printf("I am confused: server is not AF_INET.\n");
      fflush(stdout);
      return 0;
    }

     if (sendto(comm_sock, toSend, strlen(toSend), 0,
               (struct sockaddr *) addressp, sizeof((*((struct sockaddr *) addressp)))) < 0) {
    perror("sending in datagram socket");
    exit(6);
     }

    //if (message != NULL){                                                                                                                                                                                                                                                
    //message_delete(message);                                                                                                                                                                                                                                             
    //}                                                                                                                                   
  }

   wbkgd(c, COLOR_PAIR(3));
    mvwaddstr(c, 0, 0,"Enter one player's name to send the hint to or 'all' to broadcast or 'status' for a status update:\n");

    wrefresh(c);
 //free(response);                                                                                                                       
  //  if (message != NULL){                                                                                                               
  //message_delete(message);                                                                                                              
  //}                                                                                                                                     

  //fprintf(stdout, "NOW LEAVING HANDLE_STDIN of guideagent\n");                                                                          
  return 1;
}


/**************** parse_args ****************/
/*                                                                             
 * parse the command line args                                                  
 * store them into the GA struct                                                                                                                  
 */
void parse_args(int argc ,char* argv[], GA_args_t * a){
  //printf("\n Parsing args and storing into GA args struct:\n");
  char* guideId;
  char* team;
  char* player;
  char* host;
  char* port;

  if (argc != 6){
  exit(1);
  }
    else {
    for (int i=1; i<argc; i++){
      char *ch;
      ch = strtok(argv[i], " = ");
      if (strcmp(ch, "guideId") == 0){
        ch = strtok(NULL, " ,");
        guideId=ch;
      }
      else if (strcmp(ch, "team") == 0){
        ch = strtok(NULL, " ,");
        team=ch;
      }
      else if (strcmp(ch, "player") == 0){
        ch = strtok(NULL, " ,");
        player=ch;
      }
      else if (strcmp(ch, "host") == 0){
        ch = strtok(NULL, " ,");
        host=ch;
      }
      else if (strcmp(ch, "port") == 0){
        ch = strtok(NULL, " ,");
        port=ch;
      }

    }

    //fprintf(stdout, "%s %s %s\n", team, player, host);                                
    char* ptr;
    unsigned long ret = strtoul(guideId, &ptr, 16);
    a->guideID = (unsigned int)ret;
    a->team = team;
    a->player = player;
    a->host = host;
    a->port = atoi(port);
    //fprintf(stdout, "\nguideId: %u\nteam: %s\nplayer: %s\nhost: %s\nport: %d\n", a->gui 
    //deID, a->team, a->player, a->host, a->port);
    //fprintf(stdout, "guideid is %u\n", a->guideID);                                   

    }

  
     

}
  

static int send_GA_stats_req(int comm_sock, struct sockaddr_in *addressp, GA_args_t *args, GA_stats_t * stats)
{
  
  //fprintf(stdout, "NOW ENTERING SEND GA STATS of guideagent\n");
    message_t *message = message_new();
    unsigned int gId= GA_get_gameId(stats);
  
    set_opCode(message,"GA_STATUS");
    set_gameId(message,gId);
    set_guideId(message,args->guideID);
    set_team(message,args->team);
    set_player(message,args->player);
    set_statusReq(message, 1);

    char * toSend= message_to_string(message);

    //fprintf(stdout, "message to send: %s\n", toSend);
    
    //fprintf(stdout, "ready to send");
    if (addressp->sin_family != AF_INET) {
      printf("I am confused: server is not AF_INET.\n");
      fflush(stdout);
      return 0;
    }

    else{
      if (sendto(comm_sock, toSend, strlen(toSend), 0,
		 (struct sockaddr *) addressp, sizeof((*((struct sockaddr *) addressp)))) < 0) {
	perror("sending in datagram socket");
	exit(6);
      }
    }
    
    //if (message != NULL){
   
    //message_delete(message);
   
    //}

    //fprintf(stdout, "NOW LEAVING SEND GA STATS of guideagent\n");
  return 1;
}

  
 void init_ncurses(WINDOW *a, WINDOW *b, WINDOW *c)
{
  // WINDOW *a, *b, *c;
  
  //int maxx, maxy, quarterx, quartery;

  initscr();
  raw();
  refresh();
  start_color();
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  init_pair(2, COLOR_BLACK, COLOR_CYAN);
  init_pair(3, COLOR_WHITE, COLOR_MAGENTA);
}
 
//find the ID that matches the player name in the set of players
void find_player_ID(void *arg, const char *key, void *item)
{
  player_ID_t* player;
  player = arg;

  //  player_t* play = item;

  
  if (arg!= NULL && item != NULL && key != NULL){

    unsigned long p = strtoul(key,NULL,16);

    char * name = player_get_name(p, player->ga_stats);

    fprintf(stdout, "player name from set %s\n", name);
   
    char * n = malloc(strlen(name) * sizeof(char));
    
    for(int i = 0; name[i] != '\0'; i++){
        n[i] = tolower(name[i]);
      }

    fprintf(stdout, "player name from set %s\n", n);
    if (strcmp(player->playername, n)==0){

      player->playerID  = p;
    }
    free(n);
      
  }
  
}


void update_player_locations (GA_stats_t * stats, WINDOW *a)
{
  printf("here in the update_player location\n");
  int ch;
  FILE *fp= fopen("map.txt", "r");

  if( (a = newwin(40,114,0,0))== NULL);
  move(0,0);

  while((ch = fgetc(fp)) != EOF)        /* read the file till we reach the end */
  {
    printw("%c", ch);
    refresh();
  }
  fclose(fp);

  move(0,0);



   set_t * players = GA_get_players(stats);
   // make struct that holds GA_stats and everything needed to draw players on window a          

   if (players != NULL){
     loc_t *loc = location_t_new();
     loc->stats= stats;
     loc->window= a;
     //printf("about to iterate over set of players\n");
     set_iterate(players, loc, find_player_loc);
     //     printf("done iterating over the set of players\n");
   }
}





void find_player_loc(void *arg, const char *key, void *item)
{
  //printf("in the iterator\n");
  loc_t* loc;
  loc = arg;

  
  if (arg!= NULL && item != NULL && key != NULL){

    //  printf("in the if statement in the iterator\n");
    unsigned long p = strtoul(key,NULL,16);
    //printf("player id: %lu\n", p);
    
    float lat= player_get_latitude(p, loc->stats);
    float lng= player_get_longitude(p, loc->stats);

    printw("lat: %f long: %f\n", lat, lng);
    //printf("lat: %f lng: %f\n", lat, lng);
    float y= lng-ULNG;
    float divisor= ((ULNG-LLNG)/MMAXX);
    float spaces= MMAXX + (y/divisor);
    int sp = (int)spaces;
 
    
    float x= lat - ULAT;
    
    divisor = ((ULAT-LLAT)/MMAXY);

    printw("the x: %f the divisor: %f\n", x, divisor);
    float num_lines = MMAXY + (x/divisor);
    int nl = (int)num_lines;
    //fprintf(stdout, "num of lines: %f spaces: %f\n", num_lines, spaces);
    printw("num of lines: %f spaces: %f\n", num_lines, spaces);
    //wmove(loc->window, nl,sp);
    //printw( "@");
    mvaddch(nl,sp,'@');
    addch(key[0]);
    //mvprintw(
    refresh();
    //move(0,0);
  }

}

