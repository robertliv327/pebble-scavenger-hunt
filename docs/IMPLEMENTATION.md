# CS50 Final Project
### IMPLEMENTATION.md - implementation spec for KRAG 
##### Group Eleph, May 2017
 
### Overall
 
This implementation document specifies our implementation decisions for the KRAG game. It is split into four sections: Common, Game Server, Guide Agent, and Field Agent. These four sections represent the four main directories that exist in our repository.
 
### Common
 
#### Implementation Summary
 
We found that code regarding sending and receiving messages is common to the Game Server, Guide Agent, and Field Agent. Thus, we have a `message.c` file in a common directory. This file contains a message struct (described below under Data Structures as well) that holds a variable for each field of a message. It also has getter and setter functions for each of these fields. In terms of receiving messages, it also has a function `parse_message()` to parse a received message and pack it into a message struct. In terms of sending messages, it has a function `message_to_string()` to convert a message struct into a string to be sent out.
 
#### Function Prototypes and Descriptions
 
**message.c** contains the following functions:
```c
/**************** struct definitions/handlers  ****************/
/*                                                                                                                          	 
 * methods to define and create/delete a message struct to store a message's fields 	 
 *                                                                                                                          	 
 */
typedef struct message message_t;
message_t* message_new(void);
void message_delete(message_t* m);
 
/**************** getters for message struct  ****************/
/*                                                                                                                          	 
 * return the value of a field in the struct if its not NULL                                                                	 
 * return NULL equivalent otherwise                                                                                         	 
 *                                                                                                                          	 
 */
char* get_opCode(message_t* m);
void set_opCode(message_t* m, char*opCode);
char* get_respCode(message_t* m);
void set_respCode(message_t* m, char* respCode);
unsigned int get_kragId(message_t* m);
void set_kragId(message_t* m, unsigned int kragId);
unsigned int get_gameId(message_t* m);
void set_gameId(message_t*m, unsigned int gameId);
unsigned int get_guideId(message_t* m);
void set_guideId(message_t* m, unsigned int guideId);
unsigned int get_pebbleId(message_t* m);
void set_pebbleId(message_t*m, unsigned int pebbleId);
int get_lastContact(message_t* m);
void set_lastContact(message_t*m, int lastContact);
float get_latitude(message_t* m);
void set_latitude(message_t*m, float latitude);
float get_longitude(message_t* m);
void set_longitude(message_t*m, float longitude);
int get_numPlayers(message_t* m);
void set_numPlayers(message_t*m, int numPlayers);
int get_numClaimed(message_t* m);
void set_numClaimed(message_t*m, int numPlayers);
int get_numKrags(message_t* m);
void set_numKrags(message_t* m, int numKrags);
int get_statusReq(message_t* m);
void set_statusReq(message_t* m, int statusReq);
char* get_player(message_t* m);
void set_player(message_t* m, char* player);
char* get_team(message_t* m);
void set_team(message_t* m, char* team);
char* get_hint(message_t* m);
void set_hint(message_t* m, char* hint);
char* get_clue(message_t* m);
void set_clue(message_t* m, char* clue);
char* get_secret(message_t* m);
void set_secret(message_t* m, char* secret);
char* get_text(message_t* m);
void set_text(message_t *m, char* text);
bool get_isBroadcast(message_t *m);
void set_isBroadcast(message_t *m, bool isBroadcast);
unsigned int get_ID(message_t* m);
 
/**************** message_parse  ****************/
/*                                                                                                                          	 
 * takes in a message string and parses it to put its fields into a message struct                                          	 
 * returns the message struct                                                                                               	 
 *                                                                                                                          	 
 */
void message_parse(message_t *parsed_message, char* message_input);
 
/**************** message_to_string  ****************/
/*                                                                                                                          	 
 * takes in a message struct and packs it into a string to be sent away                                                                                          	 
 *                                                                                                                          	 
 */
char * message_to_string(message_t *m);
```
 
#### Description of Data Structures
 
* We have a message_t struct which stores all of the fields that a message could include, for example pebbleID (of a player), num_players (the number of players in the game) or the secret string.
 
 
### Game Server
 
#### Implementation Summary
 
The Game Server maintains a server for the Project KRAG game, updating all the necessary game information in a game_stats struct and handling messages to and from the Guide and Field Agents. 
 
We start by initializing all of our structs — reading the krags from a file, initializing the game stats struct, and create the DGRAM socket. Then, we begin an infinite while loop that listens at both the socket and stdin until a game-over flag is reached (either from stdin or by a team finishing the game). Incoming datagrams are handled through socket_handler(); parse the message filling up a message_t struct and pass that struct into opCode_handler’s do_opCode function, which performs the appropriate function from the message — updating game_stats and/or sending messages as appropriate. 
 
do_opCode responds to messages that it receives according to the pseudocode laid out in the [Requirements Spec](http://www.cs.dartmouth.edu/~cs50/Labs/Project/REQUIREMENTS.html#server). It receives the message in the form of a message struct that will have each field of the message stored in its variables. It starts off by taking the opCode field from the message struct and then looks for this opCode in its function table. This function table maps each known opCode to a function that responds to the opCode in the appropriate manner. Thus, when the message’s opCode is found in the function table, do_opCode calls to the corresponding function (which is static within opCode_handler.c). After this is done, do_opCode simply returns back to gameserver.c. do_opCode does check to make sure that the opCode received in the message matches a known opCode - if it doesn’t it sends out a SH_ERROR_INVALID_OPCODE message.
 
#### Function Prototypes and Descriptions
 
**gameserver.c** contains the following functions:
```c
/**************** main() ****************/
/*                                                                         	 
 * Moves us through the game                                               	 
 * waits in the while loop until a datagram                                	 
 * is received                                                             	 
 */
int main(const int argc, char *argv[]);
 
/********************* handle_socket **************************/
/* Sends an incoming datagram to parser                                    	 
 * to parse the message and then sends                                     	 
 * message to do_Opcode to handle the opCode function                       	 
 * exits if nothing is received in parser                                  	 
 */
static void handle_socket(game_stats_t * game_stats, socket_struct_t * sock_struct);
 
/**************** handle_stdin ****************/
/* stdin has input ready;                                                  	 
 * read a line and determine if it                                         	 
 * says game over                                                          	 
 * return EOF if EOF was encountered on stdin;                             	 
 * return 0 if there is no client to whom we can send;                     	 
 * return 1 if message sent successfully.                                  	 
 * exit on any socket error.                                               	 
 */
static int handle_stdin(int comm_sock);
 
/**************** socket_setup ****************/
/* All the work of preparing the datagram socket;                     	 
 * exit program on any error.                                              	 
 */
static int socket_setup()
 
/**************** parse_args ****************/
/*                                                                        	 
 * parse the command line args                                             	 
 * store them into the gs struct                                          	                                                                    	 
 */
void parse_args(int argc ,char* argv[], gs_args_t * gs)
```
 
**gamestats.c** contains the following functions:
```c
/**************** struct definitions/initializors  ****************/
/*                                                                                                                                                                              
 * define and create a gs struct to store a game's statistics                 
 * define and create a team struct to store statistics local to a team        
 * define and create a player struct to store statistics local to a player       
 *                            
 */
typedef struct game_stats game_stats_t;
game_stats_t* game_stats_new(void);
typedef struct team team_t;
team_t* team_new(void);
typedef struct player player_t;
player_t* player_new(void);
 
/**************** getters for GS struct  ****************/
/*                                                                     
 * return the value of a field in the struct if its not NULL           
 * return NULL equivalent otherwise           
 *
 */
unsigned int stats_get_gameID(game_stats_t* s);
int stats_get_numkrags(game_stats_t* s);
int stats_get_numagents(game_stats_t* s);
int stats_get_numteams(game_stats_t* s);
time_t stats_get_timesincestart(game_stats_t* s);
char* stats_get_secret(game_stats_t* s);
bag_t* stats_get_master_bag(game_stats_t* s);
hashtable_t* stats_get_krag_ht(game_stats_t* s);
team_t* stats_get_team(char* teamname, game_stats_t* s);
 
/**************** setters for GS struct  ****************/
/*                             
 * set a given field in the struct              
 *                                                          
 */
void set_main_hashtable(game_stats_t *gs, hashtable_t *ht);
void set_main_bag_of_clues(game_stats_t * gs, bag_t* bag);
void set_main_secret(game_stats_t *gs, char* secret);
 
/**************** getters and setters for team stats struct  ****************/
/*                    
 * return the value of a field in the struct if its not NULL    
 * return NULL equivalent otherwise                    
 * OR                                             
 * sets/increments a value of a field if the struct is not NULL                        
 *                                   
 */
unsigned int team_get_guideId(char* teamname, game_stats_t* s);
char* team_get_guide_name(char* teamname, game_stats_t* s);
time_t team_get_GA_last_contact_time(char* teamname, game_stats_t* s);
char** team_get_claimed_krags_array(char* teamname, game_stats_t* s);
int team_get_num_claimed_krags(char* teamname, game_stats_t* s);
char* team_get_secret(char* teamname, game_stats_t* s);
bag_t* team_get_bag_of_clues(char* teamname, game_stats_t* s);
player_t* team_get_player(unsigned int pebbleID, char* teamname, game_stats_t* s);
set_t* team_get_players(char* teamname, game_stats_t* s);
bool is_player_on_team(char* playername, char* teamname, game_stats_t* s);
void team_increment_num_krags(char* teamname, game_stats_t* s);
void team_set_guide_name(char* name, char* teamname, game_stats_t* s);
 
/**************** getters for player stats struct  ****************/
/*                                                                                        
 * return the value of a field in the struct if its not NULL            
 * return NULL equivalent otherwise        
 *                
 */
char* player_get_name(unsigned int pebbleID, char* teamname, game_stats_t* s);
char* player_get_team_name(unsigned int pebbleID, char* teamname, game_stats_t* s);
float player_get_latitude(unsigned int pebbleID, char* teamname, game_stats_t* s);
float player_get_longitude(unsigned int pebbleID, char* teamname, game_stats_t* s);
time_t player_get_FA_last_contact_time(unsigned int pebbleID, char* teamname, game_stats_t* s);
 
/**************** register_team  ****************/
/* 	 
 * if a team with the provided teamname doesn't exist, initializes a new team 	 
 * sets the new teams secret to the correct number of underscores and initializes the claimed_krags array and clue bag                                                          	 
 * adds the new team to the game stats struct's set of teams           
 *                                                                                 
 */
void register_team(char* teamname, game_stats_t* s);
 
/**************** update_player ****************/
/* 	 
 * updates a player's stats if it exists 
 * if it doesn't exist, creates a new player and initializes its fields to the parameters provided	 
 *  
 */
void update_player(unsigned int pebbleID, char* teamname, char* playername, int latitude, int longitude, game_stats_t* s);
 
/**************** register_guide ****************/
/*                       
 * update's stats relating to GA in a team's struct        
 * updates last contact time                  
 */
void register_guide(unsigned int guideId, char* guide_name, char* teamname, game_stats_t* s);
 
/**************** update_guide ****************/
/*                                                                                                                                                                        * updates last contact time for the GA
*                                                                                                                                                                               */
void update_guide(char* teamname, game_stats_t* s);
```
 
**krags.c** contains the following functions:
```c
/**************** struct definitions/handlers  ****************/
/*                                                                                                                          	 
 * define and create or delete  a struct to store krag info                                                                 	 
 *                                                                                                                          	 
 */
typedef struct krag_info krag_info_t;
krag_info_t* krag_info_new(void);
void krag_info_delete(krag_info_t *k);
 
/**************** getters for krags struct  ****************/
/*                                                                                                                          	 
 * return the value of a field in the struct if its not NULL                                                                	 
 * return NULL equivalent otherwise                                                                                         	 
 *                                                                                                                          	 
 */
int krag_get_latitude(krag_info_t* k);
int krag_get_longitude(krag_info_t* k);
char* krag_get_clue(krag_info_t* k);
unsigned int krag_get_kragId(krag_info_t* k);
int krag_get_index(krag_info_t* k);
 
/**************** set up krag data structures ****************/
/*                                                                                                                          	 
 * read_krags_from_file: read krags from a filename and store info in a hashtable with ID key and krag struct item          	 
 * create_bag_of_clues: create a bag of clues from a krag hashtable using iterate helper addtobag                           	 
 *                                                                                                                          	 
 */
hashtable_t* read_krags_from_file(char* filename);
void create_bag_of_clues(bag_t* bag, hashtable_t* krag_ht);
void addtobag(void* arg, const char* key, void* item);
```
 
**message_handler.c** contains the following functions:
```c
/**************** socket_struct_new ****************/
/*                                                                        	 
 * create new socket_struct                                               	 
 * return pointer to struct or                                            	 
 * NULL if error                                                          	 
 *                                                                        	 
 */
socket_struct_t* socket_struct_new(void);
 
//getters and setters for socket_struct_t                                  	 
int get_comm_socket(socket_struct_t *sock_struct);
hashtable_t *get_address_ht(socket_struct_t *sock_struct);
void set_address_ht(socket_struct_t *sock_struct, hashtable_t *ht);
void set_comm_socket(socket_struct_t *sock_struct, int comm_sock);
 
// message is sent out of the socket to the agent(iD) 
void send_message(message_t *message, int iD, socket_struct_t *sock_struct);
```
 
**opCode_handler.c** contains the following functions:
```c
/**************** do_opCode() ****************/
/*
 *  Is called when Gameserver receives a message. 
 *  It takes in a message struct, which will have an opCode field. 
 *  Also takes in a game_stats struct for accessing the server's game stats and 
 *  a socket_struct for sending messages via send_message.
 *  Function matches what's in the message struct's opCode field to a function via the function table.
 *  Then calls the matched function to correctly respond to the message according to the 
 *  pseudocode laid out in the requirements spec.
 */
void do_opCode(message_t *message, game_stats_t *stats, socket_struct_t *sock);
```
 
#### Description of Data Structures
**Structs:**
* We have a `krags_info_t` struct which stores the location, clue and index associated with a given krag
* We have a `game_stats_t` struct which holds all of the server’s stats
* Within the gamestats struct, there is also a `team_t` struct and `player_t` struct for each team and player. These are held in sets in the gamestats struct. 
* The `team_t` struct holds all of the stats for a given team and its Guide Agent, as well as a set of `player_t` structs.
* The `player_t` struct holds all of the stats for a given player (Field Agent).
* We have a `socket_struct` struct which holds the game server’s comm socket and also a hashtable mapping player Ids to their IP addresses.
 
**Data Structures:**
* We have a hashtable, `krag_table`, which maps kragIds to krag_info_t structs
* We have a bag, `clue_bag`, which is just a bag of all the clues. This is used to initialize each team’s bag of unknown clues within the team_t struct of game_stats_t struct.
* Thus, each team struct has its own bag of clues, `bag_of_clues`, that represents the clues that the team has yet to be given. This exists so that the game server can pick two clues out of this bag to send to the team’s Guide Agent when the team finds a krag.
* We have a hashtable, `address_ht`, within gameserver.c that maps pebbleIds and guideIds to their IP addresses. This is used to send out messages to a player - we can find their IP address from their guideId or pebbleId.
* Within the gamestats struct, there is a set of teams, which represents each team that is playing the game. The key for each of these is the team name and the item for each is the team’s `team_t` struct.
* Within each `team_t` struct of the set of teams, there is also a set of players, which represents each FA on the team. The key for each of these is the FA’s pebbleId and the item is the player’s `player_t` struct.
 
 
### Guide Agent
 
#### Implementation Summary
 
The Guide Agent is a client for the Project KRAG game, updating all the necessary game information in a GA_stats struct and handling messages to and from the Game Server.
 
We call to parse_args() to parse the command line arguments. We then set up the ncurses interface before anything else, and we update and refresh after the stdin or socket input are handled. We then initialize the GA stats struct, and create the DGRAM socket. Then, we begin an infinite while loop that listens at both the socket and stdin until the game is over. Incoming datagrams are handled through socket_handler() where ncurses is updated with the the new game stats; parse the message filling up a message_t struct and we pass that struct into opCode_handler’s do_opCode function, which performs the appropriate function from the message — updating GA_stats. Messages are only sent out of the datagram from stdin when the user types in status or hint.  
 
do_opCode responds to messages that it receives according to the pseudocode laid out in the [Requirements Spec](http://www.cs.dartmouth.edu/~cs50/Labs/Project/REQUIREMENTS.html#server). It receives the message in the form of a message struct that will have each field of the message stored in its variables. It starts off by taking the opCode field from the message struct and then looks for this opCode in its function table. This function table maps each known opCode to a function that responds to the opCode in the appropriate manner. Thus, when the message’s opCode is found in the function table, do_opCode calls to the corresponding function (which is static within opCode_handler.c). After this is done, do_opCode simply returns back to guideagent.c or message_handler.c to send a message out of the socket. do_opCode does check to make sure that the opCode received in the message matches a known opCode - if it doesn’t it sends out a SH_INVALID_FIELD message.
 
 
#### Function Prototypes and Descriptions
 
 
**guide_agent.c** contains the following functions:
```c
/**************** main() ****************/
Int main(const int argc, char *argv[]);
 
/**************** socket_setup ****************/
/* Parse the arguments and set up our socket.
 * Exit on any error of arguments or socket setup.
 */
static int socket_setup(const int argc, char *argv[], struct sockaddr_in *address)
 
/**************** handle_socket ****************/
/* Socket has input ready; receive a datagram
 * 'addressp' should be a valid address representing the expected sender.
 * Exit on any socket error.
 */
static void handle_socket(int comm_sock, struct sockaddr_in *addressp)
 
/********************* handle_socket **************************/
/* Sends an incoming datagram to parser                                    	 
 * to parse the message and then sends                                     	 
 * message to do_Opcode to handle the opCode function                       	 
 * exits if nothing is received in parser                                  	 
 */
static void handle_socket(game_stats_t * game_stats, socket_struct_t * sock_struct);
 
/**************** handle_stdin ****************/
/* stdin has input ready; read a line and send it to the client.
 * return EOF if EOF was encountered on stdin;
 * return 0 if there is no client to whom we can send;
 * return 1 if message sent successfully.
 * exit on any socket error.
 */
static int handle_stdin(int comm_sock, struct sockaddr_in *addressp);
 
/**************** parse_args ****************/
/*                                                                        	 
 * parse the command line args                                             	 
 * store them into the GA struct                                                            
 */
void parse_args(int argc, char* argv[], GA_args_t* ga);
```
 
**GA_stats.c** contains the following functions:
```c
 
 
/**************** struct definitions/initializers  ****************/
/*                                                                                                                                                                              
 * define and create a GA struct to store stats local to GA
 * define and create a player struct to store stats for the player that the GA needs  
 *                            
 */
typedef struct GA_stats GA_stats_t;
GA_t* GA_stats_new(void);
typedef struct player player_t;
player_t* player_new(void);
 
/**************** getters and setters for GA struct  ****************/
/*                                                                                                                          	
 * return the value of a field in the struct if it’s not NULL                                                                
 * return NULL equivalent otherwise OR                                             
 * sets value of a field if the struct is not NULL                        
 *                            	 
 */
int GA_get_numClaimed(GA_stats_t* g);
void GA_set_numClaimed(GA_stats_t* g, int numPlayers);
int GA_get_numKrags(GA_stats_t* g);
void GA_set_numKrags(GA_stats_t* g, int numKrags);
bag_t* GA_get_bag(GA_stats_t* g);
void GA_set_bag(GA_stats_t* g, bag_t* bag);
set_t* GA_get_players( GA_stats_t* g);
char* player_get_name(unsigned int pebbleID, GA_stats_t* g);
char* player_get_team_name(unsigned int pebbleID, GA_stats_t* g);
float player_get_latitude(unsigned int pebbleID, GA_stats_t* g);
float player_get_longitude(unsigned int pebbleID, GA_stats_t* g);
time_t player_get_FA_last_contact_time(unsigned int pebbleID, GA_stats_t* g);
 
/**************** update_player ****************/
/* 	 
 * updates a player's stats including its last contact time 
 *  
 */
void update_player(unsigned int pebbleID, GA_stats_t* g, char* playername, int latitude, int longitude);
```
 
**opCode_handler.c** contains the following functions:
```c
/**************** do_opCode() ****************/
/*
 *  Is called when Gameserver receives a message. 
 *  It takes in a message struct, which will have an opCode field. 
 *  Also takes in a GA_stats struct for accessing the GA’s stats and 
 *  a socket_struct for sending messages via send_message.
 *  Function matches what's in the message struct's opCode field to a function via the function table.
 *  Then calls the matched function to correctly respond to the message according to the 
 *  pseudocode laid out in the requirements spec.
 */
void do_opCode(message_t *message, GA_stats_t *stats, socket_struct_t *sock);
```
 
**message_handler.c** contains the following functions:
```c
/**************** socket_struct_new ****************/
/*                                                                        	 
 * create new socket_struct                                               	 
 * return pointer to struct or                                            	 
 * NULL if error                                                          	 
 *                                                                        	 
 */
socket_struct_t* socket_struct_new(void);
 
//getters and setters for socket_struct_t                                  	 
int get_comm_socket(socket_struct_t *sock_struct);
struct sockaddr * get_server_address(socket_struct_t *sock_struct);
void set_server_address(socket_struct_t *sock_struct, struct sockaddr * address);
void set_comm_socket(socket_struct_t *sock_struct, int comm_sock);
 
// message is sent out of the socket to the agent(iD) 
void send_message(message_t *message, GA_stats_t* stats, socket_struct_t *sock_struct.);
```
 
 
#### Description of Data Structures
**Structs:**
* We have a `GA_stats_t` struct which holds all of the stats local to the GA
* Within the GA struct, there is also a set of `player_t` structs.
* The `player_t` struct holds all of the stats for a given player (Field Agent).
* We have a `socket_struct_t` struct which holds the GA’s comm socket and the server’s address 
 
**Data Structures:**
* We have a bag, `clue_bag`, which is just a bag of all the clues known by the team, within the GA_t struct.
* Within each `GA_stats_t` struct, there is also a set of players, which represents each FA on the team. The key for each of these is the FA’s pebbleId and the item is the player’s `player_t` struct.
 
 
### Field Agent
 
#### Implementation Summary
 
Field Agent holds all the code for the Pebble Smartwatch players of the project KRAG game. It maintains and displays a small list of game statistics (like number of krags and a list of hints from the guide agent), and displays an interactive UI that enables players to view game statistics, view hints, and input krags (after initializing player information and connecting to the proxy). In response to events (15-second intervals, user KRAG claims, messages from the server), the Field Agent code will update its stats and send messages back to the server through the proxy server (a converter between the pebble’s TCP protocol and the server’s UDP protocol) as appropriate.
 
When the user starts the game on their watch, init sets up the app and the timer is started. The player is first prompted for their name and team. After the user inputs this information, the program adds the information to the FA stats.
 
From there, all code is essentially event-driven. The pebble will react to different triggers based on time, user input or incoming messages and handle each trigger appropriately until it receives a message from the server declaring the end of the game.
 
#### Function Descriptions
 
 
**field_agent.c** contains functions to set up the app (init, main) and also functions to display the main menu and receive callbacks from this main menu.
 
**FA_stats.c** contains getters and setters for each field in `FA_stats_t` struct, i.e. num_claimed, num_krags, etc.
 
**opCode_handler.c** contains the following functions:
```c

/**************** do_opCode() ****************/
/*
 *  Is called when Field Agent receives a message. 
 *  It takes in a message struct, which will have an opCode field. 
 *  Function matches what's in the message struct's opCode field to a function via the function table.
 *  Then calls the matched function to correctly respond to the message according to the 
 *  pseudocode laid out in the requirements spec.
 */
void do_opCode(message_t *message);

```
 
**message_handler.c** contains the following functions:
```c

// tick handler function
void tick_handler(struct tm *tick_time, TimeUnits units_changed);

// AppMessage functions
void inbox_received_callback(DictionaryIterator *iterator, void *context);
void outbox_sent_callback(DictionaryIterator *iterator, void *context);
void inbox_dropped_callback(AppMessageResult reason, void *context);
void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context);

// my message functions
void request_pebbleId();
void request_location();

// functions to pack and send messages to send_message()
void build_FA_LOCATION();
void build_FA_CLAIM();
void build_FA_LOG(char *text);
 
```
#### Description of Data Structures
 
**Data Structures:**
* We will store the 7 most recent hints from the GA in an array. 
