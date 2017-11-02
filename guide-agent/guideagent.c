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
#include "time.h"

//Local functions and declaration
void print_log(char * message, struct sockaddr_in *address, char* tofrom);

// struct to store args after parsing
typedef struct GA_args {
  unsigned int guideID;
  char* team;
  char* player;
  char* host;
  int port;
} GA_args_t;

//initialize args struct
GA_args_t* GA_args_new(void){
  GA_args_t* a = malloc(sizeof(GA_args_t));
  if (a == NULL){
    return NULL;
  }
  a->guideID=0;
  a->team= NULL;
  a->player = NULL;
  a->host = NULL;
  a->port = -99;
  return a;
}

// helper struct
typedef struct playerID {
  unsigned int playerID;
  char* playername;
  GA_stats_t * ga_stats;
} player_ID_t;

// struct intializer
player_ID_t* player_ID_new(void){
  player_ID_t* p = malloc(sizeof(player_ID_t));
  if (p == NULL){
    return NULL;
  }
  p->playerID=0;
  p->playername = NULL;
  p->ga_stats = NULL;
  return p;
}

// iterate helper to print
static void printclue(void* arg, const char* key, void* item){
      clue_bool_t* c = item;
      WINDOW* b = arg;
      if (get_solved_bool(c) == false){
        wprintw(b, "   %s\n", get_unsolved_clue(c));
      }
}

// helper struct
typedef struct stats_w {
  GA_stats_t* stats;
  WINDOW* w;
} stats_w_t;

// struct inializer 
stats_w_t* stats_w_new(WINDOW* w, GA_stats_t* stats){
  stats_w_t* sw = malloc(sizeof(stats_w_t));
  if (sw != NULL){
    sw->w = w;
    sw->stats = stats;
    return sw;
  }
  return NULL;
}

// iterate helper to print all players
static void printppl(void* arg, const char* key, void* item){
  unsigned long p = strtoul(key,NULL,16);
  stats_w_t* sw = arg;
  char* playername = player_get_name(p, sw->stats);
  wprintw(sw->w, "   %s\n", playername);
}
/*
// iterate helper to count number of items
static void simpleiterate(void *arg, const char *key, void *item){
  int *nitems = arg;
  //  if (nitems != NULL && key != NULL  && item != NULL)                                                                                                               
    (*nitems)++;
}
*/

// function headers
//static void simpleiterate(void *arg, const char *key, void *item);
static int socket_setup(const int argc, char *argv[], struct sockaddr_in *sAddress, GA_args_t *args);
static void handle_socket(int comm_sock, struct sockaddr_in *addressp, GA_args_t *args, GA_stats_t * stats,  WINDOW *b);
static int handle_stdin(int comm_sock, struct sockaddr_in *addressp, GA_args_t *args, GA_stats_t * stats, WINDOW *c,int maxx, int maxy, int quartery);
static int send_GA_stats_req(int comm_sock, struct sockaddr_in *addressp, GA_args_t *args, GA_stats_t * stats);
void parse_args(int argc ,char* argv[], GA_args_t* a);
void init_ncurses();
void  find_player_ID(void *arg, const char *key, void *item);


static int BUFSIZE= 1024;

int main(const int argc, char *argv[])
{
  // start logging to log file
  FILE *fp = fopen("../logs/guideagent.log", "w");
  fclose(fp);

  // initialize 3 windows in ncurses
  int maxx, maxy, quarterx, quartery;
  WINDOW *a, *b, *c;
   getmaxyx(stdscr, maxy, maxx);
  quarterx = maxx >> 2;
  quartery = maxy >> 2;

  init_ncurses();
  getmaxyx(stdscr, maxy, maxx);
  quarterx = maxx >> 2;
  quartery = maxy >> 2;

  //create four windows to fill the screen 
  if( (a = newwin(3.4*quartery, 3*quarterx, 0, 0)) == NULL);
  if( (b = newwin(3.4*quartery, maxx, 0, 3*quarterx)) == NULL);
  if( (c = newwin(maxy, maxx, 3.4*quartery, 0)) == NULL);

  mvwaddstr(a, 0, 0, "Campus map:\n");
  wbkgd(a, COLOR_PAIR(1));
  wrefresh(a);

  wbkgd(b, COLOR_PAIR(2));
  wrefresh(b);

  mvwaddstr(c, 1, 0, "Enter a hint for your field agents or 'status' for a status update:\n");

  wresize(c,40, 106);
  wbkgd(c, COLOR_PAIR(3));
  wrefresh(c);
    
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
			if (handle_stdin(comm_sock, &sAddress, ga_args, ga_stats,c, maxx, maxy, quartery ) == EOF) {
	// fprintf(stdout, "here in stdin EOF\n");
	 break; // exit loop if EOF on stdin
	}
      }
      if (FD_ISSET(comm_sock, &rfds)) {
	handle_socket(comm_sock, &sAddress, ga_args, ga_stats, b);
	
      }
    }
  }
  GA_stats_delete(ga_stats);
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



/**************** handle_socket ****************/
/* Socket has input ready; receive a datagram                                                                                                 
 * 'addressp' should be a valid address representing the expected sender.                                                                     
 * Exit on any socket error.                                                                                                                  
 */

  
static void handle_socket(int comm_sock, struct sockaddr_in *sAddress, GA_args_t *args, GA_stats_t * stats, WINDOW *b)
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
	//create message struct to put parse message into
	message_t *message = message_new();
	print_log(buf, &sender, "FROM");
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
  mvwaddstr(b, 0, 3, "Game stats ");
  wprintw(b, "for team %s:\n\n\n", GA_get_teamname(stats));
  wprintw(b, "Game ID: %u \n\n", GA_get_gameId(stats));
    
  wprintw(b, "Active field agents: \n");
  
  stats_w_t* sw = stats_w_new(b, stats);
  set_iterate(GA_get_players(stats), sw, printppl);
  
    wprintw(b, "# of krags: %d\n", GA_get_numKrags(stats));
    wprintw(b, "# of krags claimed: %d\n\n", GA_get_numClaimed(stats));
    wprintw(b, "Unsolved clues:\n");
    set_iterate(GA_get_kragId_to_clue(stats), b, printclue);
    
    wprintw(b, "Decoded secret: %s\n", GA_get_secret(stats));
    wrefresh(b);
  
}

 
static int handle_stdin(int comm_sock, struct sockaddr_in *addressp, GA_args_t *args, GA_stats_t * stats, WINDOW *c, int maxx, int maxy,
int quartery)
{

  if( (c = newwin(maxy, maxx, 3.4*quartery, 0)) == NULL);

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
    // show text "hint received" for 1.5 seconds
    sleep(1.5);
    // override the hint with a blank string
    mvwprintw(c,0,0,"\r                                                                                                                  \
                                                                                                                                     ");
    mvwprintw(c,1,0,"\r                                                                                                                  \
                                                                                                                                     ");

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
    print_log(toSend, addressp, "TO");
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

  }

   wbkgd(c, COLOR_PAIR(3));
    mvwaddstr(c, 0, 0,"Enter one player's name to send the hint to or 'all' to broadcast or 'status' for a status update:\n");

    wrefresh(c);

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
    a->guideID = (unsigned int) ret;
    a->team = team;
    a->player = player;
    a->host = host;
    a->port = atoi(port);
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

    fprintf(stdout, "player name from set %s", name);
   
    char * n = malloc(strlen(name) * sizeof(char));
    
    for(int i = 0; name[i] != '\0'; i++){
        n[i] = tolower(name[i]);
      }

    fprintf(stdout, "player name from set %s", n);
    if (strcmp(player->playername, n)==0){

      player->playerID  = p;
    }
    free(n);
      
  }
  
}

/**************** print_log *************/
 /* Prints a message to ../logs/gameserver.log                                                                                                                                                      
  * Appends to the end of the log file -- if you need to                                                                                                                                            
  * reset the file, you must do so separately!                                                                                                                                                      
  */

void print_log(char * message, struct sockaddr_in *address, char* tofrom) {
  //  unsigned long addressnum = address->sin_addr.s_addr;                                                                                                                                        
  //unsigned short addressport = address->sin_port;                                                                                                                         
  char timestamp[27];
  time_t clk = time(NULL);
  sprintf(timestamp, "(%s", ctime(&clk));
  timestamp[25] = ')';
  FILE *fp = fopen("../logs/guideagent.log", "a");
  fprintf(fp, "%s %s %s@%05d: %s\n", timestamp, tofrom, inet_ntoa(address->sin_addr), ntohs(address->sin_port), message);
  fclose(fp);
}
