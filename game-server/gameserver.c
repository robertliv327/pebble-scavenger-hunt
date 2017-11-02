 /* 
 * gameserver - a server for project KRAG that manages messages, stores data and sends updates for 
 * the guide and field agents using UDP and select().
 *
 * Read messages from a UDP socket and handle them depending on their contents;
 * Listen for a GAME OVER from stdin to end the game (or end the game on team victory).
 * Use select() so we don't block on either stdin or the socket.
 * 
 * usage: ./gameserver gameId=(Game ID) kff=(Krag File) sf=(Secret File)
 * port=(port number)
 *  Type GAME OVER in stdin to exit the game
 *
 * eleph, May 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	      // strlen
#include <strings.h>	      // bcopy, bzero
#include <arpa/inet.h>	      // socket-related calls
#include <sys/select.h>	      // select-related stuff 
#include <ctype.h>
#include "file.h"	      // readlinep
#include "message.h"          
#include "hashtable.h"
#include "message_handler.h"
#include "gamestats.h"
#include "opCode_handler.h"
#include "bag.h"
#include "krags.h"
#include "memory.h"




/* struct that holds the fields for the
 * game server arguments
 *
 */
typedef struct game_server_args {
  unsigned int gameID;
  char * kff;
  char * sf;
  int port;
} gs_args_t;


/* struct that holds the fields necessary                                      
 * to send a datagram                                                       
 *                                                                             
 */
//typedef struct socket_struct {
//int comm_sock;
// hashtable_t * ht;
//} socket_struct_t;



/**************** file-local constants and functions ****************/
static const int BUFSIZE = 1024;     // read/write buffer size
static const int port = 51888;
static const int slot_size = 10;

static int socket_setup();
static void handle_stdin(int comm_sock, char * argv[], game_stats_t *game_stats, socket_struct_t * sock_struct);
static void handle_socket(game_stats_t *game_stats, socket_struct_t * sock_struct);
//static void hashprint(FILE *fp, const char *key, void *item);



void parse_args(int argc ,char* argv[], gs_args_t * gs);

gs_args_t* gs_args_new(void);





//socket_struct_t* socket_struct_new(void);

  
/**************** main() ****************/
/*
 * Moves us through the game
 * waits in the while loop until a datagram
 * is received 
 */

int
main(const int argc, char *argv[])
{

  gs_args_t * gs = gs_args_new();
  parse_args(argc ,argv, gs);
  gs->port = port;

  //Reset the log file
  FILE *fp = fopen("../logs/gameserver.log", "w");
  fclose(fp);


  
  //initialize structs
  //Initialize game_stats_t
  game_stats_t * game_stats = game_stats_new();
    
  //Init ht for playerId, address
  hashtable_t *address_ht = hashtable_new(slot_size);
  
  //read krags
    //hashtable of krag, krag struct pairs
  hashtable_t *krag_table = hashtable_new(50);
  int numKrags = read_krags_from_file(krag_table, gs->kff);

  //  fprintf(stdout, "\nprinting hashtable\n");
  //hashtable_print(krag_table, stdout, hashprint);

  //main bag of clues
  bag_t * clue_bag = bag_new();
  create_bag_of_clues(clue_bag, krag_table);

  //read secret from the file
  FILE * s_file = fopen(gs->sf, "r");
  char * secret= readlinep(s_file);
  fclose(s_file);
  //set the bag, kt, and secret in gamestats
  set_main_hashtable(game_stats, krag_table);
  set_main_bag_of_clues(game_stats, clue_bag);
  set_main_secret(game_stats, secret);

  set_gameId_gs(game_stats, gs->gameID);
  set_num_krags_gs(game_stats, numKrags);

    
  //put clues in a master-bag
  //put krags in a hashtable, where keys are kragIDs and items are krag_t structs

  //set up a socket on which to receive messages
  int comm_sock = socket_setup();

  
  // add to socket struct
  socket_struct_t* sock_struct= socket_struct_new();
  set_comm_socket(sock_struct,comm_sock);
  set_address_ht(sock_struct, address_ht);
  
 
  // if stdin, handle it (it's either a "GAME OVER" or some debugging script);
  // if socket, receive message from the socket, parse it into a message_t struct, and handle it.
  while(true) {
    game_print(game_stats);
    fd_set rfds;
    // stdin is fd 0, and the UDP socket is above it.
    FD_ZERO(&rfds);
    FD_SET(0, &rfds); //stdin
    FD_SET(comm_sock, &rfds); //UDP socket
    int nfds = comm_sock+1; //highest-numbered fd in rfds

    // Wait for input using slect()
    int select_response = select(nfds, &rfds, NULL, NULL, NULL);
    // note: 'rfds' updates

    if (select_response < 0) {
      // some error occurred
      fprintf(stderr, "select()");
      exit(9);
    } else if (select_response == 0) {
      // timeout occurred; this should not happen
    } else if (select_response > 0) {
      //some data is ready on either source, or both

      if (FD_ISSET(0, &rfds)) {
	handle_stdin(comm_sock,  argv, game_stats, sock_struct);
      }
      if (FD_ISSET(comm_sock, &rfds)) {
	handle_socket(game_stats, sock_struct);
      }

      // print a fresh prompt
      printf(": ");
      fflush(stdout);
    }
  }
  //clean up
  close(comm_sock);
  //delete game_stats
  game_stats_delete(game_stats);
}


/**************** socket_setup ****************/
/* All the work of preparing the datagram socket;
 * exit program on any error.
 */
static int
socket_setup()
{
  // Create socket on which to listen (file descriptor)
  int comm_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (comm_sock < 0) {
    perror("opening datagram socket");
    exit(1);
  }

  // Name socket using wildcards
  struct sockaddr_in server;  // server address
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  if (bind(comm_sock, (struct sockaddr *) &server, sizeof(server))) {
    perror("binding socket name");
    exit(2);
  }

  // Print our assigned port number
  socklen_t serverlen = sizeof(server);	// length of the server address
  if (getsockname(comm_sock, (struct sockaddr *) &server, &serverlen)) {
    perror("getting socket name");
    exit(3);
  }
  printf("Ready at port %d\n", ntohs(server.sin_port));

  return (comm_sock);
}


/********************* handle_socket **************************/
/* Sends an incoming datagram to parser
 * to parse the message and then sends
 * message to do_Opcode to handle th opCode function
 * exits if nothing is received in parser
 */

static void
handle_socket(game_stats_t * game_stats, socket_struct_t * sock_struct)
{
   fprintf(stdout, "NOW ENTERING HANDLE SOCKET of gameserver\n");
  //initialize variables
  //Figure out who the sender is
  //if the sender is not in our struct, we need to put them in our struct
  //Parse the message and build a message_t struct from it.
  //If the message's gameID is from the wrong game, ignore it and dump thte struct
  //If gameID = 0, add the IP address to hashtable of {name, IP address} pairs
  //If gameID != 0, check hashtable to see if the sender is there. If not, send a SH_ERROR_INVALID_ID
  //Send the struct into the opCode_handler, and Robert does the rest.
 //initialize variables
  //Figure out who the sender is

  // socket has input ready 
   struct sockaddr_in *sender = malloc(sizeof(struct sockaddr_in));
   // sender of this message
  //struct sockaddr_in *senderp = &sender;
  struct sockaddr *senderp = (struct sockaddr *) sender;
  socklen_t senderlen = sizeof(sender);  // must pass address to length
  char buf[BUFSIZE];	      // buffer for reading data from socket
  memset(buf,'\0', BUFSIZE);
  int nbytes = recvfrom(get_comm_socket(sock_struct), buf, BUFSIZE-1, 0, senderp, &senderlen);

  
  if (nbytes < 0) {
    perror("receiving from socket");
    exit(1);
  } else {
    buf[nbytes] = '\0';     // null terminate string
	
	
   // where was it from?
    if (sender->sin_family != AF_INET) {
      printf("From non-Internet address: Family %d\n", sender->sin_family);
    } 

    else{
      //log the message
      print_log(buf, sender, "FROM");
      //Parse the message and build a message_t struct from it.
      message_t *message = message_new();
      message_parse(message,buf);
      
      
      //for testing                                                                                                                                        
      unsigned int gameID= get_gameId(message);
      char * opCode= get_opCode(message);
      
      unsigned int ID;
      if ((get_pebbleId(message)) != 0 ){
	ID=get_pebbleId(message);
	fprintf(stdout, "message out to pebble %u\n", ID);
      }
      else {
	ID= get_guideId(message);
	fprintf(stdout, "message out to guide agent %u\n", ID);
      }
      
      fprintf(stdout, "opcode of received message in socket: %s, %u\n\n", opCode, gameID);
      //end testing                                                                                                                                      
      
      
      //get player ID from the message
      char playerID[9];
      unsigned int iD= get_ID(message);
      //itoa(iD, playerID, 16); nope
      
      sprintf(playerID, "%x", iD);
      //puts(playerID);
      
      fprintf(stdout,"Player id %u in handle_socket %s\n",iD, playerID);
      

      //set a pointer to the last address
      set_last_sender(sock_struct ,sender);

      struct sockaddr_in *hashSender=hashtable_find(get_address_ht(sock_struct), playerID);
      
      if (hashSender == NULL){
	hashtable_insert(get_address_ht(sock_struct), playerID, sender);
	//fprintf(stdout, "we inserted into the hashtable bc key not found");
      }

      

      //printf("printing hashtable of addresses P1: \n");
      //hashtable_print(get_address_ht(sock_struct), stdout, hashprint);
      
      //make sure that name and address added to ht
      if ((hashtable_find(get_address_ht(sock_struct), playerID)) != NULL){

	//hashtable_insert(sock_struct->ht,sender, &sender); 
	// send message to opcode function
	do_opCode(message, game_stats, sock_struct);
	message_delete(message);
      }

    }
  }
  fprintf(stdout, "NOW LEAVING HANDLE SOCKET of gameserver\n");
}


/**************** gs_args_new ****************/
/* y
 * create new gs_args struct
 * return pointer to struct or
 * NULL if error
 * 
 */
gs_args_t* gs_args_new(void){
  gs_args_t* gs_args = malloc(sizeof(gs_args_t));

  if (gs_args == NULL){
    return NULL;
  }

  gs_args->gameID=0;
  gs_args->kff= NULL;
  gs_args->sf= NULL;
  gs_args->port= -99;

  return gs_args;
}



/**************** handle_stdin ****************/
/* stdin has input ready; 
 * read a line and determine if it 
 * says game over
 * exit on any socket error.
 */
static void
handle_stdin(int comm_sock, char * argv[], game_stats_t *game_stats, socket_struct_t * sock_struct)
{
  fprintf(stdout, "NOW ENTERING HANDLE STDIN of gameserver\n");
  
  char *response = readlinep(stdin);
  if (strcmp(response, "GAME OVER") == 0){ 
    handle_game_over(game_stats, sock_struct);
  }
  else {
    printf("Print GAME OVER to stop the game\n");
  }
  
}




/**************** parse_args ****************/
/*                                                                             
 * parse the command line args
 * store them into the gs struct                                               
 *                                                                             
 *                                                                             
 */
void parse_args(int argc ,char* argv[], gs_args_t * gs)
{
  char *iD; //[IDSIZE];
  char *kff; //[BUFSIZE];
  char *sf; //[BUFSIZE];
  char *port; //[IDSIZE];


  
  if (argc != 5){
  exit(1);
  }

  else {

    

    for (int i=1; i<argc; i++){

      char *ch;
      ch = strtok(argv[i], " = ");
      ch = strtok(NULL, " ,");
      if (i==1){
	iD=ch;
      }

      else if (i==2){
	kff =ch;
      }
      
      else if (i==3){
	sf=ch;
      }

      else{
	port=ch;
      }
      
    }

    
   
    
    char *ptr;
    unsigned long ret= strtoul(iD, &ptr, 16);

    FILE *fp = fopen(kff, "r");
    assertp(fp, "Invalid krag file name");
    fclose(fp);

    FILE *fp2 = fopen(sf, "r");
    assertp(fp2, "Invalid Secret file name");
    fclose(fp2);
    
    gs->gameID = (unsigned int)ret;
    gs->kff = kff;
    gs->sf = sf;
    gs->port = atoi(port);
   

  }
  
}


/*
static void hashprint(FILE *fp, const char *key, void *item) {
  int *num = item;
  if (num == NULL || key == NULL)
    fprintf(fp, "null");
  else
    fprintf(fp, "key: %s item: %p",
	    key, item);
}
*/
    

















