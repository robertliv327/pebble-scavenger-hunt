/*                                                                                                                                           
 * message.c     Eleph     May, 2017                                                                                                 
 *                                                                                                                                          
 * Defines a struct that contains the various fields a message could contain 
 * Has getters for the various fields
 *                                                                                                                                          
 * See message.h for more information                                                                                                
 *                                                                                                                                          
 */

#ifdef NOPEBBLE  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#else 
#include <pebble.h>
#endif

#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/set.h"
#include "../libcs50/counters.h"


// definition of a message struct

typedef struct message {
  char* opCode;
  char* respCode;


  #ifdef NOPEBBLE  

  unsigned int gameId;
  unsigned int guideId;
  unsigned int pebbleId;
  unsigned int kragId;

  #else 
  char *gameId;
  char *guideId;
  char *pebbleId;
  char *kragId;

  #endif

  int lastContact;
  float latitude;
  float longitude;
  int numPlayers;
  int numClaimed;
  int numKrags;
  int statusReq;
  char* player;
  char* team;
  char* hint;
  char* clue;
  char* secret;
  char* text;
  
} message_t;


//local functions
static bool add_to_message_struct(message_t *message, char * field_ID, char *field);

/* 
 * message_new()
 * function to initialize a message struct
 */
message_t* message_new(void){
  message_t* m = malloc(sizeof(message_t));
  if (m == NULL){
    return NULL;
  }

  #ifdef NOPEBBLE  

  m->gameId = 99999999;
  m->guideId = 99999999;
  m->pebbleId = 0;
  m->kragId = 0;

  #else 
  m->gameId = NULL;
  m->guideId = NULL;
  m->pebbleId = NULL;
  m->kragId = NULL;
  
  #endif
  
  m->opCode = NULL;
  m->respCode = NULL;
  m->lastContact = -1;
  m->latitude = 200.0;
  m->longitude = 200.0;
  m->numPlayers = -1;
  m->numClaimed = -1;
  m->numKrags = -1;
  m->statusReq = -1; 
  m->player = NULL;
  m->team = NULL;
  m->hint = NULL;
  m->clue = NULL;
  m->secret = NULL;
  m->text = NULL;
  
  return m;
}

/* 
 * message_delete()
 *  function to delete a message struct
 */

void message_delete(message_t* m){
  if (m!= NULL){
    if (m->opCode) free(m->opCode);
    if (m->respCode) free(m->respCode);
    if (m->player) free(m->player);
    if (m->team) free(m->team);
    if (m->hint) free(m->hint);
    if (m->clue) free(m->clue);
    if (m->secret) free(m->secret);
    if (m->text) free(m->text);

    #ifdef NOPEBBLE  


    #else 

    if (m->gameId) free(m->gameId);
    if (m->guideId) free(m->guideId);
    if (m->pebbleId) free(m->pebbleId);
    if (m->kragId) free(m->kragId);
    
    #endif

    free(m);
  }
}

/*************** getters and setters *************/
char* get_opCode(message_t* m){
  if (m != NULL && m->opCode != NULL){
    return m->opCode;
  }
  else {
    return NULL;
  }
}

void set_opCode(message_t *m, char* opCode){
  if (m != NULL) {
    m->opCode = opCode;
  }
}

char* get_respCode(message_t* m){
  if (m != NULL && m->respCode != NULL){
    return m->respCode;
  }
  else {
    return NULL;
  }
}

void set_respCode(message_t *m, char* respCode){
  if (m != NULL) {
    m->respCode = respCode;
  }
}

#ifdef NOPEBBLE  

unsigned int get_ID(message_t* m){
  if (m->guideId != 99999999 || m->pebbleId != 0){
    if (m->guideId == 99999999){
      return m->pebbleId;
    }
    if (m->pebbleId == 0){
      return m->guideId;
    }
    return m->guideId;
  }
  return 0;
}

unsigned int get_gameId(message_t* m){
  if (m != NULL){
    return m->gameId;
  }
  else {
    return 99999999;
     }
}

unsigned int get_guideId(message_t* m){
  if (m != NULL){
    return m->guideId;
  }
  else {
    return 99999999;
     }
}

unsigned int get_pebbleId(message_t* m){
  if (m != NULL){
    return m->pebbleId;
  }
  else {
    return 0;
     }
}

void set_gameId(message_t *m, unsigned int gameId){
  if (m != NULL) {
    m->gameId = gameId;
  }
}

void set_guideId(message_t *m, unsigned int guideId){
  if (m != NULL) {
    m->guideId = guideId;
  }
}

void set_pebbleId(message_t *m, unsigned int pebbleId){
  if (m != NULL) {
    m->pebbleId = pebbleId;
  }
}

unsigned int get_kragId(message_t* m){
  if (m != NULL){
    return m->kragId;
  }
  else {
    return 0;
     }
}

void set_kragId(message_t *m, unsigned int kragId){
  if (m != NULL) {
    m->kragId = kragId;
  }
}

#else 

char* get_gameId(message_t* m){
  if (m != NULL && m->gameId != NULL){
    return m->gameId;
  }
  else {
    return NULL;
  }
}

char* get_guideId(message_t* m){
  if (m != NULL && m->guideId != NULL){
    return m->guideId;
  }
  else {
    return NULL;
  }
}

char* get_pebbleId(message_t* m){
  if (m != NULL && m->pebbleId != NULL){
    return m->pebbleId;
  }
  else {
    return NULL;
  }
}

char *get_kragId(message_t* m){
  if (m != NULL && m->kragId != NULL){
    return m->kragId;
  }
  else {
    return NULL;
  }
}

void set_gameId(message_t *m, char* gameId){
  if (m != NULL) {
    m->gameId = gameId;
  }
}

void set_guideId(message_t *m, char* guideId){
  if (m != NULL) {
    m->guideId = guideId;
  }
}

void set_pebbleId(message_t *m, char* pebbleId){
  if (m != NULL) {
    m->pebbleId = pebbleId;
  }
}

void set_kragId(message_t *m, char* kragId){
  if (m != NULL) {
    m->kragId = kragId;
  }
}

#endif


int get_lastContact(message_t* m){
  if (m != NULL){
    return m->lastContact;
  }
  else {
    return -1;
     }
}

void set_lastContact(message_t *m, int lastContact){
  if (m != NULL) {
    m->lastContact = lastContact;
  }
}


float get_latitude(message_t* m){
  if (m != NULL){
    return m->latitude;
  }
  else {
    return 200.0;
     }
}

void set_latitude(message_t *m, float latitude){
  if (m != NULL) {
    m->latitude = latitude;
  }
}

float get_longitude(message_t* m){
  if (m != NULL){
    return m->longitude;
  }
  else {
    return 200.0;
     }
}

void set_longitude(message_t *m, float longitude){
  if (m != NULL) {
    m->longitude = longitude;
  }
}

int get_numPlayers(message_t* m){
  if (m != NULL){
    return m->numPlayers;
  }
  else {
    return -1;
     }
}

void set_numPlayers(message_t *m, int numPlayers){
  if (m != NULL) {
    m->numPlayers = numPlayers;
  }
}

int get_numClaimed(message_t* m){
  if (m != NULL){
    return m->numClaimed;
  }
  else {
    return -1;
     }
}

void set_numClaimed(message_t *m, int numClaimed){
  if (m != NULL) {
    m->numClaimed = numClaimed;
  }
}


int get_numKrags(message_t* m){
  if (m != NULL){
    return m->numKrags;
  }
  else {
    return -1;
     }
}

void set_numKrags(message_t *m, int numKrags){
  if (m != NULL) {
    m->numKrags = numKrags;
  }
}

int get_statusReq(message_t* m){
  if (m != NULL){
    return m->statusReq;
  }
  else {
    return -1;
     }
}

void set_statusReq(message_t *m, int statusReq){
  if (m != NULL) {
    m->statusReq = statusReq;
  }
}


char* get_player(message_t* m){
  if (m != NULL && m->player != NULL){
    return m->player;
  }
  else {
    return NULL;
     }
}

void set_player(message_t *m, char* player){
  if (m != NULL) {
    m->player = player;
  }
}


char* get_team(message_t* m){
  if (m != NULL && m->team != NULL){
    return m->team;
  }
  else {
    return NULL;
     }
}

void set_team(message_t *m, char* team){
  if (m != NULL) {
    m->team = team;
  }
}

char* get_hint(message_t* m){
  if (m != NULL && m->hint != NULL){
    return m->hint;
  }
  else {
    return NULL;
     }
}

void set_hint(message_t *m, char* hint){
  if (m != NULL) {
    m->hint = hint;
  }
}

char* get_clue(message_t* m){
  if (m != NULL && m->clue != NULL){
    return m->clue;
  }
  else {
    return NULL;
     }
}

void set_clue(message_t *m, char* clue){
  if (m != NULL) {
    m->clue = clue;
  }
}


char* get_secret(message_t* m){
  if (m != NULL && m->secret != NULL){
    return m->secret;
  }
  else {
    return NULL;
  }

}


void set_secret(message_t *m, char* secret){
  if (m != NULL) {
    m->secret = secret;
  }
}


char* get_text(message_t* m){
  if (m != NULL && m->text != NULL){
    return m->text;
  }
  else {
    return NULL;
  }

}


void set_text(message_t *m, char* text){
  if (m != NULL) {
    m->text = text;
  }
}

/* 
 * message_parse()
 * Takes a string and parses it into a message_t struct
 * MEMORY CONTRACT -- does not malloc for a message -- a blank one
 * must be passed in.
 */
void message_parse(message_t *parsed_message, char* message_input) {
  // create a message_t struct
  int message_length = strlen(message_input);
  // Loop through the first string until you see an equals sign.

  char* message_out = malloc(message_length+1);
  strcpy(message_out, message_input);

  for (char *rest = message_out; *rest != '\0'; ) {
    char* field;
    char * field_ID = rest;
    for (rest = field_ID; *rest != '\0' && *rest != '='; rest++) {}
    if (*rest != '\0') {
      *rest++ = '\0';
    }
    field = rest;
    for (rest = field; *rest != '\0' && *rest != '|'; rest++) {}
    if (*rest != '\0') {
      *rest++ = '\0';
    }
    if (!add_to_message_struct(parsed_message, field_ID, field)) {
      free(message_out);
      message_delete(parsed_message);
      parsed_message = message_new();
      char *invalid  = malloc(8);
      strcpy(invalid, "INVALID");
      set_opCode(parsed_message, invalid);
      return;
    }
     
  }
  free(message_out);
}


/*
 * add_to_message_struct
 * Helper method for message_parse
 * given a message, field_ID and field, add the field to the message struct 
 */
static bool add_to_message_struct(message_t *message, char * field_ID, char* field) {

  if (strcmp(field_ID,"opCode") == 0 && !get_opCode(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->opCode = field_copy;
  } else if (strcmp(field_ID, "respCode") == 0 && !get_respCode(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->respCode = field_copy;
  } else if (strcmp(field_ID,"lastContact") == 0 && get_lastContact(message) == -1) {
    message->lastContact = atoi(field);
  }

#ifdef NOPEBBLE 

   else if (strcmp(field_ID, "latitude") == 0 && get_latitude(message) == 200.0) {
    message->latitude = atof(field);
  } else if (strcmp(field_ID, "longitude") == 0 && get_longitude(message) == 200.0) {
    message->longitude = atof(field);

  } else if (strcmp(field_ID, "gameId") == 0 && get_gameId(message) == 99999999) {
    message->gameId = strtoul(field, NULL, 16);
  } else if (strcmp(field_ID, "guideId") == 0 && get_guideId(message) == 99999999) {
    message->guideId = strtoul(field, NULL, 16);
  } else if (strcmp(field_ID, "pebbleId") == 0 && get_pebbleId(message) == 0) {
    message->pebbleId = strtoul(field, NULL, 16);
  } else if (strcmp(field_ID, "kragId") == 0 && !get_kragId(message)) {
    message->kragId = strtoul(field, NULL, 16);
  }

#else

   else if (strcmp(field_ID, "gameId") == 0 && !get_gameId(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->gameId = field_copy;
  } else if (strcmp(field_ID, "guideId") == 0 && !get_guideId(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->guideId = field_copy;
  } else if (strcmp(field_ID, "pebbleId") == 0 && !get_pebbleId(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->pebbleId = field_copy;
  } else if (strcmp(field_ID, "kragId") == 0 && !get_kragId(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->kragId = field_copy;
  }
  
#endif

   else if (strcmp(field_ID, "numPlayers")==0 && get_numPlayers(message) == -1) {
    message->numPlayers = atoi(field); 
  } else if (strcmp(field_ID, "numClaimed") == 0 && get_numClaimed(message) == -1) {
    message->numClaimed = atoi(field);
  } else if (strcmp(field_ID, "numKrags") == 0 && get_numKrags(message) == -1) {
    message->numKrags = atoi(field);
  } else if (strcmp(field_ID, "statusReq") == 0 && get_statusReq(message) == -1) {
    message->statusReq = atoi(field);
  } else if (strcmp(field_ID, "player") == 0 && !get_player(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->player = field_copy;
  } else if (strcmp(field_ID, "team") == 0 && !get_team(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->team = field_copy;
  } else if (strcmp(field_ID, "hint") == 0 && !get_hint(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->hint = field_copy;
  } else if (strcmp(field_ID,"clue") == 0 && !get_clue(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->clue = field_copy;
  } else if (strcmp(field_ID,"secret") == 0 && !get_secret(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->secret = field_copy;
  } else if (strcmp(field_ID,"text") == 0 && !get_text(message)) {
    char *field_copy = malloc(strlen(field)+1);
    strcpy(field_copy, field); 
    message->text = field_copy;
  } else { // if the field_ID matches nothing, return the message with a flag
    
    return false;
  }
  return true;
}

/* 
 * message_to_string()
 * Take a message and return a string representation of the message
 * 
 */
char * message_to_string(message_t *m) {
  char * m_out = malloc(700);
  char *field;
  if (m->opCode!=NULL) {
    field = "opCode=";
    strcpy(m_out, field);
    strcat(m_out, m->opCode);
  }
  if (m->respCode!=NULL) {
    field = "|respCode=";
    strcat(m_out, field);
    strcat(m_out, m->respCode);
  }

  #ifdef NOPEBBLE 

  if (m->kragId!=0){
    field = "|kragId=";
    strcat(m_out, field);
    char krag_str[9];
    snprintf(krag_str, sizeof(krag_str), "%x", m->kragId);
    strcat(m_out, krag_str);
  }

  if (m->gameId!=99999999){
    field = "|gameId=";
    strcat(m_out, field);
    char game_str[9];
    snprintf(game_str, sizeof(game_str), "%x", m->gameId);
    strcat(m_out, game_str);
  }

  if (m->guideId!=99999999){
    field = "|guideId=";
    strcat(m_out, field);
    char guide_str[9];
    snprintf(guide_str, sizeof(guide_str), "%x", m->guideId);
    strcat(m_out, guide_str);
  }

  if (m->pebbleId!=0){
    field = "|pebbleId=";
    strcat(m_out, field);
    char peb_str[9];
    snprintf(peb_str, sizeof(peb_str), "%x", m->pebbleId);
    strcat(m_out, peb_str);
  }

  #else

  if (m->kragId!=NULL){
    field = "|kragId=";
    strcat(m_out, field);
    char krag_str[9];
    snprintf(krag_str, sizeof(krag_str), "%s", m->kragId);
    strcat(m_out, krag_str);
  }

  if (m->gameId!=NULL){
    field = "|gameId=";
    strcat(m_out, field);
    char game_str[9];
    snprintf(game_str, sizeof(game_str), "%s", m->gameId);
    strcat(m_out, game_str);
  }

  if (m->guideId!=NULL){
    field = "|guideId=";
    strcat(m_out, field);
    char guide_str[9];
    snprintf(guide_str, sizeof(guide_str), "%s", m->guideId);
    strcat(m_out, guide_str);
  }

  if (m->pebbleId!=NULL){
    field = "|pebbleId=";
    strcat(m_out, field);
    char peb_str[9];
    snprintf(peb_str, sizeof(peb_str), "%s", m->pebbleId);
    strcat(m_out, peb_str);
  }

  #endif

  if (m->lastContact!=-1){
    field = "|lastContact=";
    strcat(m_out, field);
    char last_contact_str[10];
    snprintf(last_contact_str, sizeof(last_contact_str), "%d", m->lastContact);
    strcat(m_out, last_contact_str);
  }

  if (m->latitude!=200.0){
    field = "|latitude=";
    strcat(m_out, field);
    char lat_str[11];
    snprintf(lat_str, sizeof(lat_str), "%f", m->latitude);
    strcat(m_out, lat_str);
  }

  if (m->longitude!=200.0){
    field = "|longitude=";
    strcat(m_out, field);
    char long_str[11];
    snprintf(long_str, sizeof(long_str), "%f", m->longitude);
    strcat(m_out, long_str);
  }

  if (m->numPlayers!=-1){
    field = "|numPlayers=";
    strcat(m_out, field);
    char num_players_str[4];
    snprintf(num_players_str, sizeof(num_players_str), "%d", m->numPlayers);
    strcat(m_out, num_players_str);
  }

  if (m->numClaimed!=-1){
    field = "|numClaimed=";
    strcat(m_out, field);
    char num_claimed_str[4];
    snprintf(num_claimed_str, sizeof(num_claimed_str), "%d", m->numClaimed);
    strcat(m_out, num_claimed_str);
  }

  if (m->numKrags!=-1){
    field = "|numKrags=";
    strcat(m_out, field);
    char num_krags_str[4];
    snprintf(num_krags_str, sizeof(num_krags_str), "%d", m->numKrags);
    strcat(m_out, num_krags_str);
  }

  if (m->statusReq!=-1){
    field = "|statusReq=";
    strcat(m_out, field);
    char stat_req_str[2];
    snprintf(stat_req_str, sizeof(stat_req_str), "%d", m->statusReq);
    strcat(m_out, stat_req_str);
  }

  if (m->player!=NULL) {
    field = "|player=";
    strcat(m_out, field);
    strcat(m_out, m->player);
  }
  
  if (m->team!=NULL) {
    field = "|team=";
    strcat(m_out, field);
    strcat(m_out, m->team);
  }
  
  if (m->hint!=NULL) {
    field = "|hint=";
    strcat(m_out, field);
    strcat(m_out, m->hint);
  }
  
  if (m->clue!=NULL) {
    field = "|clue=";
    strcat(m_out, field);
    strcat(m_out, m->clue);
  }
  
  if (m->secret!=NULL) {
    field = "|secret=";
    strcat(m_out, field);
    strcat(m_out, m->secret);
  }
  if (m->text!=NULL) {
    field = "|text=";
    strcat(m_out, field);
    strcat(m_out, m->text);
  }
  
  
    
  return m_out;
}

