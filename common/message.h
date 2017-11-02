/*                                                                                                                                                            
 * message.h     Eleph     May, 2017                                                                                                                        
 *                                                                                                                                                           
 * Defines a struct that contains the various fields a message could contain                                                                           
 * Has getters for the various fields                                                                                                                         *                                                                                                                                                           
 * Functionality:
 *        A message struct can be created with message_new()
 *        Field x can be retrived by calling get_x(message_t* m);
 */

#ifndef __MESSAGE_H
#define __MESSAGE_H

/* 
 * our message struct - holds all fields associated with a message
 */
typedef struct message message_t;

/* 
 * message_new()
 * function to initialize a message struct
 */
message_t* message_new(void);

/* 
 * message_delete()
 *  function to delete a message struct
 */
void message_delete(message_t* m);


/**************** getters and setters for message struct  ****************/
/*                                                                     
 * GETTERS: return the value of a field in the struct if its not NULL           
 * return NULL equivalent otherwise           
 *
 * SETTERS: add the passed value into the appropriate place in the message
 *
 */

#ifdef NOPEBBLE  

unsigned int get_ID(message_t* m);
unsigned int get_gameId(message_t* m);
unsigned int get_guideId(message_t* m);
unsigned int get_pebbleId(message_t* m);
unsigned int get_kragId(message_t* m);
void set_gameId(message_t *m, unsigned int gameId);
void set_guideId(message_t *m, unsigned int guideId);
void set_pebbleId(message_t *m, unsigned int pebbleId);
void set_kragId(message_t* m, unsigned int kragId);

#else 

char* get_gameId(message_t* m);
char* get_guideId(message_t* m);
char* get_pebbleId(message_t* m);
char* get_kragId(message_t* m);
void set_gameId(message_t *m, char* gameId);
void set_guideId(message_t *m, char* guideId);
void set_pebbleId(message_t *m, char* pebbleId);
void set_kragId(message_t *m, char* kragId);

#endif

char* get_opCode(message_t* m);
void set_opCode(message_t* m, char*opCode);
char* get_respCode(message_t* m);
void set_respCode(message_t* m, char* respCode);
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


/*************** message_to_string **************/
/* 
 * takes in a message struct and reads from all its fields to
 * create a string
 *
 */
char *message_to_string(message_t *m);

/**************** message_parse  ****************/
/*                                                                     
 * takes in a message string and parses it to put its fields into a message struct
 * returns the message struct
 *
 */
void message_parse(message_t *parsed_message, char* message_input);

#endif //__MESSAGE_H
