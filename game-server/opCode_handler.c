/*
 * opCode_handler.c     Eleph     May, 2017
 *
 * Handles the processing of each opCode for Gameserver. 
 * Called when a message is received by Gameserver
 * Does this processing by using a function table to match the incoming message struct to a function.
 * Then calls that funciton to perform correct response to message.
 * 
 * See opCode_handler.h for more information
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "message.h"
#include "gamestats.h"
#include "hashtable.h"
#include "bag.h"
#include "krags.h"
#include "memory.h"
#include "message_handler.h"
#include "opCode_handler.h"
#include <time.h>

/**************** file−local global variables ****************/ 

/**************** local structs/types ****************/ 
// a little structure to carry a message_t and a socket_struct_t
typedef struct message_sock_struct {
  message_t *message;
  socket_struct_t *sock;
  game_stats_t *stats;
} message_sock_struct_t;


// just a 'new' function for the struct above... makes a new struct and returns it or returns NULL if error
static message_sock_struct_t *message_sock_struct_new(message_t *message, socket_struct_t *sock, game_stats_t *stats){
  message_sock_struct_t *new_struct = malloc(sizeof(message_sock_struct_t));
  if (new_struct != NULL){
    new_struct->message = message;
    new_struct->sock = sock;
    new_struct->stats = stats;
    return new_struct;
  }
  return NULL;
}


/**************** local function prototypes ****************/ 
static void FA_LOCATION(message_t *message, game_stats_t *stats, socket_struct_t *sock);
static bool validate_FA_LOCATION(message_t *message, socket_struct_t *sock);
static void FA_CLAIM(message_t *message, game_stats_t *stats, socket_struct_t *sock);
static bool validate_FA_CLAIM(message_t *message, socket_struct_t *sock);
static void FA_LOG(message_t *message, game_stats_t *stats, socket_struct_t *sock);
static bool validate_FA_LOG(message_t *message, socket_struct_t *sock);
static void GA_STATUS(message_t *message, game_stats_t *stats, socket_struct_t *sock);
static bool validate_GA_STATUS(message_t *message, socket_struct_t *sock);
static void GA_HINT(message_t *message, game_stats_t *stats, socket_struct_t *sock);
static bool validate_GA_HINT(message_t *message, socket_struct_t *sock);
static bool validate_gameId_team_player_Id(message_t *message, char *agent, game_stats_t *stats, socket_struct_t *sock);
static void send_GAME_STATUS(message_t *message, unsigned int ID, game_stats_t *stats, socket_struct_t *sock);
static void send_GS_RESPONSE(message_t *message, char *resp_code, unsigned int ID, char *text, socket_struct_t *sock);
static void send_GS_CLAIMED(message_t *message, game_stats_t *stats, socket_struct_t *sock);
static void send_GS_CLUE(message_t *message, game_stats_t *stats, socket_struct_t *sock, bool new_guide); 
static void send_GS_SECRET(message_t *message, game_stats_t *stats, socket_struct_t *sock);
static void send_GS_AGENT(message_t *message, game_stats_t *stats, socket_struct_t *sock);
static void send_GS_AGENT_helper(void *arg, const char *key, void *item);
static void invalid_field(message_t *message, game_stats_t *stats, socket_struct_t *sock);
static void forward_GA_HINT(message_t *message, game_stats_t *stats, socket_struct_t *sock);
static void broadcast_GA_HINT(void *arg, const char *key, void *item);
static bool legit_location(float lat, float lon, unsigned int kragId, game_stats_t *stats);
static bool krag_claimed_by_team(unsigned int kragId, char *team, game_stats_t *stats);
static void claim_krag(unsigned int kragId, char *team, game_stats_t *stats);
static void update_secret(char *team, unsigned int kragId, game_stats_t *stats);
static void send_game_over_to_team(void *arg, const char *key, void *item);
static message_t *assemble_team_record(team_t *team, socket_struct_t *sock, game_stats_t *stats);
static message_t * assemble_game_over(socket_struct_t *sock, game_stats_t *stats);
static void send_game_over_to_player(void * arg, const char *key, void *item);


/**************** function dispatch table ****************/
static const struct {
	const char *opCode;
	void (*func)(message_t *message, game_stats_t *stats, socket_struct_t *sock);
} dispatch[] = {
	{ "FA_LOCATION", FA_LOCATION },
	{ "FA_CLAIM", FA_CLAIM },
	{ "FA_LOG", FA_LOG },
	{ "GA_STATUS", GA_STATUS },
	{ "GA_HINT", GA_HINT },
	{ "INVALID", invalid_field },
	{ NULL, NULL }
};

/**************** do_opCode() ****************/
void do_opCode(message_t *message, game_stats_t *stats, socket_struct_t *sock){
  // get the opCode from the message struct
  char *opCode = get_opCode(message);
  int fn;
  
  // get function pointer that matches the opCode and call the function
  for (fn = 0; dispatch[fn].opCode != NULL; fn++) {
    if (strcmp(opCode, dispatch[fn].opCode) == 0) {
      (*dispatch[fn].func)(message, stats, sock);
      break;
    }
  }
  // if opCode didn't match, error
  if (dispatch[fn].opCode == NULL){
    // send out SH_ERROR_INVALID_OPCODE message
    send_GS_RESPONSE(message, "SH_ERROR_INVALID_OPCODE", '*', "unrecognized opCode", sock);
  }
}

/**************** FA_LOCATION() ****************/
/*
 *  Handles incoming message with opcode "FA_LOCATION"
 *
 *  The primary communication from a Field Agent (FA) to the Game Server (GS) is 
 *	a formatted message that informs the GS of its location and identifiers that allow 
 * 	the GS and Guide Agent (GA) to uniquely identify a particular FA. Generally this 
 *	message is for the benefit of the GS and GA, however, if the FA desires it can explicitly 
 * 	request to receive an update from the GS regarding the status and state of the game.
 *
 */
static void FA_LOCATION(message_t *message, game_stats_t *stats, socket_struct_t *sock){
  // validate subset of fields included in message... if correct, proceed; otherwise, do nothing
  if (validate_FA_LOCATION(message, sock)){
    // get team and pebbleId from message struct
    char *team = get_team(message);
    unsigned int pebbleId = get_pebbleId(message);
    // if FA is just starting
    if (get_gameId(message) == 0){
      // look to see if FA previously joined under different name or team
      int match = player_check_Id(stats, pebbleId, team, get_player(message));
      printf("\nMATCH: %i\n", match);
      // if FA didn't previously join game under different name or team
      if (match == 0){
	// if coming in as new team, register team
	if (stats_get_team(team, stats) == NULL){
	  // allocate memory for team name before copying it over
	  char *team_cpy = malloc(strlen(team) + 1);
	  strcpy(team_cpy, team);
	  // register team
	  register_team(team_cpy, stats);
	  //	  char* currsecret = team_get_secret(get_team(message), stats);
	  
	  
	  //					char* secrett = malloc(strlen(team_get_secret(team, stats))+2);
	  //secrett = team_get_secret(team, stats);
	  //printf("\n\n\nrigh in FA LOCATIOn after registering a team this is theri SECRETTTTTT, should be underscores: %s\n\n\n", secrett);
	  
	}
	// if coming in as new player, register player as member of team
	if (team_get_player(pebbleId, team, stats) == NULL){
	  // if new player name, register player
	  if (!is_player_on_team(get_player(message), team, stats)){
	    // allocate memory for player's name before copying it into player struct
	    char *player_name = malloc(strlen(get_player(message)) + 1);
	    strcpy(player_name, get_player(message));
	    // allocate memory for team name before copying it over
	    char *team_cpy = malloc(strlen(team) + 1);
	    strcpy(team_cpy, team);
	    // register player
	    update_player(pebbleId, team_cpy, player_name, get_latitude(message), \
			  get_longitude(message), stats);
	    
	    // send game status message to FA now that they have joined
	    send_GAME_STATUS(message, pebbleId, stats, sock);
	  }
	  // else, send SH_ERROR_DUPLICATE_PLAYERNAME to FA
	  else {
	    send_GS_RESPONSE(message, "SH_ERROR_DUPLICATE_PLAYERNAME", pebbleId, \
			     "name already in use - pick a new one", sock);
	  }
	}
	// else, FA must have crashed and come back
	else {
	  // send game status message to GA now that they have joined
	  send_GAME_STATUS(message, get_guideId(message), stats, sock);
	}
      } 
      // if FA previously logged in with different team
      else if (match == 1){
	// SH_ERROR_INVALID_TEAMNAME
	send_GS_RESPONSE(message, "SH_ERROR_INVALID_TEAMNAME", '*',	\
			 "Used a different team name in prior communication", sock);
      } 
      // if FA previously logged in with different name (match == 2)
      else if (match == 2) {	
	// SH_ERROR_INVALID_PLAYERNAME
	send_GS_RESPONSE(message, "SH_ERROR_INVALID_PLAYERNAME", '*',	\
			 "Used a different player name in prior communication", sock);
      }
      // else both team and player names were bad...
      else {
	// SH_ERROR_INVALID_PLAYERNAME                                                                                                           
	send_GS_RESPONSE(message, "SH_ERROR_INVALID_PLAYERNAME", '*',	\
			 "Used a different player name in prior communication", sock);
	// SH_ERROR_INVALID_TEAMNAME                                                                                                             
	send_GS_RESPONSE(message, "SH_ERROR_INVALID_TEAMNAME", '*',	\
			 "Used a different team name in prior communication", sock);     
      }
    }
    // if FA is not just starting...
    else {
      // validate the gameId, pebbleId, team, and player
      if (validate_gameId_team_player_Id(message, "FA", stats, sock)){
	// // allocate memory for player's name before copying it into player struct
	// char *player_name = malloc(strlen(get_player(message)) + 1);
	// strcpy(player_name, get_player(message));
	// // free previous name...
	// free(player_get_name(pebbleId, team, stats));
	// update records regarding location and last-contact-time of this Field Agent
	update_player(pebbleId, get_team(message), get_player(message), \
		      get_latitude(message), get_longitude(message), stats);
	// if statusReq=1, send a GAME_STATUS message back to the Field Agent.
	if (get_statusReq(message) == 1){
	  send_GAME_STATUS(message, pebbleId, stats, sock);
	}
      }
    }
  }
}

/**************** validate_FA_LOCATION() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_FA_LOCATION(message_t *message, socket_struct_t *sock){
  // expects: opCode=FA_LOCATION|gameId=|pebbleId=|team=|player=|latitude=|longitude=|statusReq=
  if ((strcmp("FA_LOCATION", get_opCode(message)) == 0) && (get_gameId(message) != 99999999) && \
      (get_pebbleId(message) != 0) && (get_team(message) != NULL) && (get_player(message) != NULL) && \
      (get_latitude(message) != 200.0) && (get_longitude(message) != 200.0) && (get_statusReq(message) != -1)){
    
    // does not expect: respCode, kragId, guideId, lastContact, numPlayers, 
    // 					numClaimed, numKrags, hint, clue, secret, text
    if ((get_respCode(message) == NULL) && (get_kragId(message) == 0) && (get_guideId(message) == 99999999) && \
	(get_lastContact(message) == -1) && (get_numPlayers(message) == -1) && (get_numClaimed(message) == -1) && \
	(get_numKrags(message) == -1) && (get_hint(message) == NULL) && (get_clue(message) == NULL) && \
	(get_secret(message) == NULL) && (get_text(message) == NULL)){
      
      // all fields are correct ; proceed with message handling!
      return true;
      
    } else {
      // if we get more than what we expected, send SH_ERROR_INVALID_FIELD
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_FIELD", get_pebbleId(message), \
		       "received more fields than expected", sock);
    }
  } else {
    // if we did not get what we expected, send SH_ERROR_INVALID_MESSAGE
    send_GS_RESPONSE(message, "SH_ERROR_INVALID_MESSAGE", get_pebbleId(message), "fields do not match opCode", sock);
  }
  return false;
}

/**************** FA_CLAIM() ****************/
/*
 *  handles incoming message with opcode "FA_CLAIM"
 *
 *	To claim a krag the FA must send a message to the GS indicating its current 
 *  location and the unique 4-digit hex code found on the krag at that location.
 *
 */
static void FA_CLAIM(message_t *message, game_stats_t *stats, socket_struct_t *sock){
  // validate subset of fields included in message... if correct, proceed; otherwise, do nothing
  if (validate_FA_CLAIM(message, sock)){
    if (validate_gameId_team_player_Id(message, "FA", stats, sock)){
      unsigned int kragId = get_kragId(message);
      // get the hashtable of krags in gameserver...
      hashtable_t *krags = stats_get_krag_ht(stats);
      // search for kragId in hashtable of krags (ignore if no match)
      char krag_str[9];
      sprintf(krag_str, "%x", kragId);
      if (hashtable_find(krags, krag_str)){
	// confirm that latitude|longitude is within 10m of the position of the krag
	if (legit_location(get_latitude(message), get_longitude(message), kragId, stats)){
	  // if krag has already been claimed by team, send a SH_CLAIMED_ALREADY response
	  if (krag_claimed_by_team(kragId, get_team(message), stats)){
	    // send SH_CLAIMED_ALREADY response
	    send_GS_RESPONSE(message, "SH_CLAIMED_ALREADY", get_pebbleId(message), "krag already claimed!", sock);
	  }
	  // otherwise, mark krag as claimed and proceed accordingly
	  else {
	    // mark krag as claimed
	    claim_krag(kragId, get_team(message), stats);
	    // send SH_CLAIMED
	    send_GS_RESPONSE(message, "SH_CLAIMED", get_pebbleId(message), "claimed krag!", sock);
	    // send GS_CLAIMED to GA
	    send_GS_CLAIMED(message, stats, sock);
	    // send two clues to GA (GS_CLUE) (false means that it wants two clues, not one)
	    send_GS_CLUE(message, stats, sock, false);
	    char* currsecret = team_get_secret(get_team(message), stats);
	    printf("\n\n\n this is a teams secret after they claim but before update secret is called!! here: %s\n\n\n\n", currsecret);
	    // update this team’s copy of the secret so as to reveal characters of the string                          
	    update_secret(get_team(message), kragId, stats);
	    //					char* currsecret2 = team_get_secret(get_team(message), stats);
	    // send the updated secret, via a GS_SECRET message, to the Guide Agent on the Field Agent’s team.
	    //	printf("HI. TEAMS SECRET AFTER IT IS UPDATES: %s", currsecret2); 
	    send_GS_SECRET(message, stats, sock);
	    int numClaimed = team_get_num_claimed_krags(get_team(message), stats);
	    int numKrags = stats_get_numkrags(stats);
	    if (numClaimed == numKrags) {
	      handle_game_over(stats, sock);
	    }  
      	  }
	}
      }
    }
  }
}

/**************** validate_FA_CLAIM() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_FA_CLAIM(message_t *message, socket_struct_t *sock){
  // expects: opCode=FA_CLAIM|gameId=|pebbleId=|team=|player=|latitude=|longitude=|kragId=
	if ((strcmp("FA_CLAIM", get_opCode(message)) == 0) && (get_gameId(message) != 99999999) && \
	    (get_pebbleId(message) != 0) && (get_team(message) != NULL) && (get_player(message) != NULL) && \
		(get_latitude(message) != 200.0) && (get_longitude(message) != 200.0) && (get_kragId(message) != 0)){
	  
	  // does not expect: respCode, guideId, lastContact, numPlayers, numClaimed, numKrags,
	  //					statusReq, hint, clue, secret, text
	  if ((get_respCode(message) == NULL) && (get_guideId(message) == 99999999) && \
	      (get_lastContact(message) == -1) && (get_numPlayers(message) == -1) && (get_numClaimed(message) == -1) && \
	      (get_numKrags(message) == -1) && (get_statusReq(message) == -1) && (get_hint(message) == NULL) \
	      && (get_clue(message) == NULL) && (get_secret(message) == NULL) && (get_text(message) == NULL)){
	    
	    // all fields are correct ; proceed with message handling!
	    return true;
	    
	  } else {
	    // if we get more than what we expected, send SH_ERROR_INVALID_FIELD
	    send_GS_RESPONSE(message, "SH_ERROR_INVALID_FIELD", get_pebbleId(message), \
			     "received more fields than expected", sock);
	  }
	} else {
	  // if we did not get what we expected, send SH_ERROR_INVALID_MESSAGE
	  send_GS_RESPONSE(message, "SH_ERROR_INVALID_MESSAGE", get_pebbleId(message), "fields do not match opCode", sock);
	}
	return false;
}

/*************** FA_LOG() ***************/
/*
 *  handles incoming message with opcode "FA_LOG"
 *
 *	The Field Agent can send this message to the Game Server at any time. 
 *	It is ignored by the Game Server but is useful for your Field Agent to get information into the log files.
 *
 */
static void FA_LOG(message_t *message, game_stats_t *stats, socket_struct_t *sock){
  // validate subset of fields included in message... if correct, proceed; otherwise, do nothing
  if (validate_FA_LOG(message, sock)){
    // ignore (validate_FA_LOG() will take care of errors)
  }
}

/**************** validate_FA_LOG() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_FA_LOG(message_t *message, socket_struct_t *sock){
  // expects: opCode=FA_LOG|pebbleId=|text=
  if ((strcmp("FA_LOG", get_opCode(message)) == 0) && (get_pebbleId(message) != 0) && (get_text(message) != NULL)){
    
    // does not expect: respCode, kragId, gameId, guideId, lastContact, latitude, longitude,
    //					numPlayers, numClaimed, numKrags, statusReq, player, team, hint, clue, secret
    if ((get_respCode(message) == NULL) && (get_kragId(message) == 0) && (get_gameId(message) == 99999999) && \
	(get_guideId(message) == 99999999) && (get_lastContact(message) == -1) && (get_latitude(message) == 200.0) && \
	(get_longitude(message) == 200.0) && (get_numPlayers(message) == -1) && \
	(get_numClaimed(message) == -1) && (get_numKrags(message) == -1) && (get_statusReq(message) == -1) && \
	(get_team(message) == NULL) && (get_player(message) == NULL) && (get_hint(message) == NULL) && \
	(get_clue(message) == NULL) && (get_secret(message) == NULL)){
      
      // all fields are correct ; proceed with message handling!
      return true;
      
    } else {
      // if we get more than what we expected, send SH_ERROR_INVALID_FIELD
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_FIELD", get_pebbleId(message), \
		       "received more fields than expected", sock);
    }
  } else {
    // if we did not get what we expected, send SH_ERROR_INVALID_MESSAGE
    send_GS_RESPONSE(message, "SH_ERROR_INVALID_MESSAGE", get_pebbleId(message), "fields do not match opCode", sock);
  }
  return false;
}

/**************** GA_STATUS() ****************/
/*
 *  handles incoming message with opcode "GA_STATUS"
 *
 *	The Guide Agent (GA) joins the game by sending its first periodic status message. 
 *  In this status message, it can also request to receive an update from the GS regarding 
 *  the status and state of the game by setting a flag in the fomatted message it sends to the GS.
 * 
 */
static void GA_STATUS(message_t *message, game_stats_t *stats, socket_struct_t *sock){
  // validate subset of fields included in message... if correct, proceed; otherwise, do nothing
  if (validate_GA_STATUS(message, sock)){
    // get team struct that matches team name sent by message
    team_t *team_struct = stats_get_team(get_team(message), stats);
    // if GA is just joining game
    if (get_gameId(message) == 0){
      // look to see if GA previously joined under different name or team
      int match = player_check_Id(stats, get_guideId(message), get_team(message), get_player(message));
      // if GA didn't previously join game under different name or team
      if (match == 0){
	// if coming in as new team, register team
	if (team_struct == NULL){
	  printf("\n\n\n\n\n\n\n\n\n entered the if where we register a teammmmmm");
	  // allocate memory for team name before copying it over
	  char *team_cpy = malloc(strlen(get_team(message)) + 1);
	  strcpy(team_cpy, get_team(message));
	  // register team
	  register_team(team_cpy, stats);
	  //stats_set_gameID(stats, get_gameId(message));
	}
	// if coming in as new guideId on team
	if (team_get_guideId(get_team(message), stats) == 0){
	  // allocate memory for guide's name before copying it into team struct
	  char *player_name = malloc(strlen(get_player(message)) + 1);
	  strcpy(player_name, get_player(message));
	  // register guide
	  register_guide(get_guideId(message), player_name, get_team(message), stats);
	  // send game status message to GA now that they have joined
	  send_GAME_STATUS(message, get_guideId(message), stats, sock);
	  // also send a GS_AGENT message to GA for each FA on GA's team
					send_GS_AGENT(message, stats, sock);
					// send one random clue GA (true means one clue, not two)
					send_GS_CLUE(message, stats, sock, true);
					// send blank secret to GA
					//					send_GS_SECRET(message, stats, sock);
	} 
	// else if there's some other GA on team
	else if (team_get_guideId(get_team(message), stats) != get_guideId(message)){
	  // ignore
	}
	// else, GA must have crashed and come back
	else {
	  // send game status message to GA now that they have joined
	  send_GAME_STATUS(message, get_guideId(message), stats, sock);
	}
      } 
      // if GA previously logged in with different team
      else if (match == 1){
	// SH_ERROR_INVALID_TEAMNAME
	send_GS_RESPONSE(message, "SH_ERROR_INVALID_TEAMNAME", get_guideId(message), \
			 "Used a different team name in prior communication", sock);
      } 
      // if GA previously logged in with different name (match == 2)
      else {	
	// SH_ERROR_INVALID_PLAYERNAME
	send_GS_RESPONSE(message, "SH_ERROR_INVALID_PLAYERNAME", get_guideId(message), \
			 "Used a different player name in prior communication", sock);
      }
    }
    // if GA is not just starting
    else {
      // validate the gameId, guideId, team, and player
      if (validate_gameId_team_player_Id(message, "GA", stats, sock)){
	// update records regarding last-contact-time of this Guide Agent.
	update_guide(get_team(message), stats);
	// if statusReq=1, send a GAME_STATUS message back to the Guide Agent.
	if (get_statusReq(message) == 1){
	  send_GAME_STATUS(message, get_guideId(message), stats, sock);
	  // also send a GS_AGENT message to GA for each FA on GA's team
	  send_GS_AGENT(message, stats, sock);
	}
      }		
    }
  }
}

/**************** validate_GA_STATUS() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_GA_STATUS(message_t *message, socket_struct_t *sock){
  // expects: opCode=GA_STATUS|gameId=|guideId=|team=|player=|statusReq=
  if ((strcmp("GA_STATUS", get_opCode(message)) == 0) && (get_gameId(message) != 99999999) && \
      (get_guideId(message) != 99999999) && (get_team(message) != NULL) && (get_player(message) != NULL) && \
      (get_statusReq(message) != -1)){
    
    // does not expect: respCode, kragId, pebbleId, lastContact, latitude, longitude,
    //					numPlayers, numClaimed, numKrags, hint, clue, secret, text
    if ((get_respCode(message) == NULL) && (get_kragId(message) == 0) && (get_pebbleId(message) == 0) && \
	(get_lastContact(message) == -1) && (get_latitude(message) == 200.0) && (get_longitude(message) == 200.0) && \
	(get_numPlayers(message) == -1) && (get_numClaimed(message) == -1) && (get_numKrags(message) == -1) && \
	(get_hint(message) == NULL) && (get_clue(message) == NULL) && (get_secret(message) == NULL) && \
	(get_text(message) == NULL)){
      
      // all fields are correct ; proceed with message handling!
      return true;
      
    } else {
      // if we get more than what we expected, send SH_ERROR_INVALID_FIELD
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_FIELD", get_guideId(message), \
		       "received more fields than expected", sock);
    }
  } else {
    // if we did not get what we expected, send SH_ERROR_INVALID_MESSAGE
    send_GS_RESPONSE(message, "SH_ERROR_INVALID_MESSAGE", get_guideId(message), "fields do not match opCode", sock);
  }
  return false;
}

/**************** GA_HINT() ****************/
/*
 *  handles incoming message with opcode "GA_HINT"
 *
 *	To assist Field Agents in their game play, the GA requires a mechanism that 
 *	enables the Guide to send textual hints to its FAs.
 * 
 */
static void GA_HINT(message_t *message, game_stats_t *stats, socket_struct_t *sock){
  // validate subset of fields included in message... if correct, proceed; otherwise, do nothing
  if (validate_GA_HINT(message, sock)){
    // validate gameId, guideId, team, and player
    if (validate_gameId_team_player_Id(message, "GA", stats, sock)){
      // validate that pebbleId is *, or a known player on the GA’s team. (if invalid, ignore this message.)
      if ((team_get_player(get_pebbleId(message),			\
			   get_team(message), stats) != NULL) || (get_pebbleId(message) == strtoul("2a", NULL, 16))){
	// forward message to either all FAs on team, or specific FA
	forward_GA_HINT(message, stats, sock);
	// update records regarding last-contact-time of this Guide Agent.
	update_guide(get_team(message), stats);
      }
    }
  }
}

/**************** validate_GA_HINT() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_GA_HINT(message_t *message, socket_struct_t *sock){
  // expects: opCode=GA_HINT|gameId=|guideId=|team=|player=|pebbleId=|hint=
  if ((strcmp("GA_HINT", get_opCode(message)) == 0) && (get_gameId(message) != 99999999) && \
      (get_guideId(message) != 99999999) && (get_team(message) != NULL) && (get_player(message) != NULL) && \
      (get_pebbleId(message) != 0) && (get_hint(message) != NULL)){
    
    // does not expect: respCode, kragId, lastContact, latitude, longitude, numPlayers,
    //					numClaimed, numKrags, statusReq, clue, secret, text
    if ((get_respCode(message) == NULL) && (get_kragId(message) == 0) && \
	(get_lastContact(message) == -1) && (get_latitude(message) == 200.0) && \
	(get_longitude(message) == 200.0) && (get_numPlayers(message) == -1) && \
	(get_numClaimed(message) == -1) && (get_numKrags(message) == -1) && (get_statusReq(message) == -1) && \
	(get_clue(message) == NULL) && (get_secret(message) == NULL) && (get_text(message) == NULL)){
      
      // all fields are correct ; proceed with message handling!
      return true;
      
    } else {
      // if we get more than what we expected, send SH_ERROR_INVALID_FIELD
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_FIELD", get_guideId(message), \
		       "received more fields than expected", sock);
    }
  } else {
    // if we did not get what we expected, send SH_ERROR_INVALID_MESSAGE
    send_GS_RESPONSE(message, "SH_ERROR_INVALID_MESSAGE", get_guideId(message), "fields do not match opCode", sock);
  }
  return false;
}

/**************** validate_gameId_team_player_Id() ****************/
/*
 *  validates gameId, team name, player name, and an Id
 *  does so by comparing what's in game_stats to what's passed in via the message
 *  agent parameter says whether it was called by a FA or GA (treated differently)
 */
static bool validate_gameId_team_player_Id(message_t *message, char *agent, game_stats_t *stats, socket_struct_t *sock){
  // get team struct from team's name 
  team_t *team_struct = stats_get_team(get_team(message), stats);
  
  // validate team
  if (team_struct == NULL){
    if (strcmp(agent, "FA") == 0){
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_TEAMNAME", get_pebbleId(message), "unrecognized team name", sock);
    } else {
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_TEAMNAME", get_guideId(message), "unrecognized team name", sock);
    }
    return false;
  }
  // validate gameId
  if (stats_get_gameID(stats) != get_gameId(message)){
    if (strcmp(agent, "FA") == 0){
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_GAME_ID", get_pebbleId(message), "unrecognized gameId", sock);
    } else {
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_GAME_ID", get_guideId(message), "unrecognized gameId", sock);
    }
    return false;
  } 
  
  // if called from a FA, we look to validate pebbleId and player name
  if (strcmp(agent, "FA") == 0){
    player_t *player = team_get_player(get_pebbleId(message), get_team(message), stats);
    // validate pebbleId
    if (player == NULL){
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_ID", get_pebbleId(message), "unrecognized pebbleId", sock);
      return false;			
    }
    // validate player
    if (strcmp(player_get_name(get_pebbleId(message), get_team(message), stats), get_player(message)) != 0){
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_PLAYERNAME", get_pebbleId(message), "unrecognized FA player", sock);
      return false;			
    }
  }
  
  // if called from a GA, we look to validate guideId and guide name
  if (strcmp(agent, "GA") == 0){
    // validate guideId
    if (team_get_guideId(get_team(message), stats) != get_guideId(message)){
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_ID", get_guideId(message), "unrecognized guideId", sock);
      return false;
    }
    // validate player
    if ((strcmp(team_get_guide_name(get_team(message), stats), get_player(message)) != 0)){
      send_GS_RESPONSE(message, "SH_ERROR_INVALID_PLAYERNAME", get_guideId(message), "unrecognized GA player", sock);
      return false;
    }
  }
  // everything validated correctly
  return true;
}

/**************** invalid_field() ****************/
/*
 *  Called when opcode of message struct is set to "INVALID" by parser.
 *  This occurs when parser finds an invalid field. To handle this, invalid_field
 *  sends SH_INVALID_FIELD response message.
 */
static void invalid_field(message_t *message, game_stats_t *stats, socket_struct_t *sock){

  send_GS_RESPONSE(message, "SH_INVALID_FIELD", '*', "unrecognized fieldname", sock);
}

/**************** send_GAME_STATUS() ****************/
/*
 *  A GAME_STATUS message is sent by the Game Server to either a Field Agent or Guide Agent 
 *  upon request for a status update (as indicated by the statusReq field).
 *
 *  Whenever the GS sends a GAME_STATUS message to a GA, it immediately sends the GA a 
 *  GS_AGENT message for each FA on the GA’s team.
 */
static void send_GAME_STATUS(message_t *message, unsigned int ID, game_stats_t *stats, socket_struct_t *sock){
  // set up new message to send out
  message_t *m = message_new();
  char *op = malloc(12 * sizeof(char*));
  strcpy(op, "GAME_STATUS");
  set_opCode(m, op);
  set_gameId(m, stats_get_gameID(stats));
	if (team_get_guideId(get_team(message), stats) == 99999999){
	  set_guideId(m, 0);
	} else {
	  set_guideId(m, team_get_guideId(get_team(message), stats));
	}
	set_numClaimed(m, team_get_num_claimed_krags(get_team(message), stats));
	set_numKrags(m, stats_get_numkrags(stats));
	
	// send out message
	send_message(m, ID, sock);
}

/**************** send_GS_RESPONSE() ****************/
/*
 * For debugging and convenience the Game Server shall respond to various conditions via this message.
 */
static void send_GS_RESPONSE(message_t *message, char *resp_code, unsigned int ID, char *text, socket_struct_t *sock){
  // set up new message to send out
  message_t *m = message_new();
	char *op = malloc(12 * sizeof(char*));
	strcpy(op, "GS_RESPONSE");
	set_opCode(m, op);
	set_gameId(m, get_gameId(message));
	// plug in appropriate response code
	char *resp = malloc(strlen(resp_code) + 1);						////// MAKE STATS PARAMETER!!!!!!!!!!!!!!!
	strcpy(resp, resp_code);
	set_respCode(m, resp);
	// plug in appropriate text...
	set_text(m, text);
	send_message(m, ID, sock);
} 

/**************** send_GS_CLAIMED() ****************/
/*
 *  After a Field Agent successfully locates a krag, the Game Server shall send this message to that 
 *  team’s Guide Agent. This message allows a Guide Agent to know that the clue corresponding to this 
 *  kragId has been claimed and thus, for example, could be removed from the list of unsolved clues. 
 *  The message carries the krag’s location, allowing the Guide Agent to display the claimed krag on a map.
 */
static void send_GS_CLAIMED(message_t *message, game_stats_t *stats, socket_struct_t *sock){
  // set up new message to send out
  message_t *m = message_new();
  char *op = malloc(12 * sizeof(char*));
  strcpy(op, "GS_CLAIMED");
	set_opCode(m, op);
	set_gameId(m, stats_get_gameID(stats));
	set_guideId(m, team_get_guideId(get_team(message), stats));
	set_pebbleId(m, get_pebbleId(message));
	set_kragId(m, get_kragId(message));
	set_latitude(m, get_latitude(message));
	set_longitude(m, get_longitude(message));
	send_message(m, team_get_guideId(get_team(message), stats), sock);
}

/**************** send_GS_CLUE() ****************/
/*
 *  After a Field Agent successfully locates a krag, the Game Server may send one or 
 *  more clues to that team’s Guide Agent. The Guide Agent shall not show the kragId to its player; 
 *  it is provided here for later matching with a future GS_CLAIMED message.
 */
static void send_GS_CLUE(message_t *message, game_stats_t *stats, socket_struct_t *sock, bool new_guide){
	// get the team's bag of clues to extract from
  bag_t *bag_of_clues = team_get_bag_of_clues(get_team(message), stats);
  // error... just return
  if (bag_of_clues == NULL){
    return;
  }
  // if only want to send one clue
  if (new_guide){
    // extract a clue
    char *clue = bag_extract(bag_of_clues);
		// if bag wasn't empty...
    if (clue != NULL){
      
      message_t *m = message_new();
      char *op = malloc(12 * sizeof(char*));
      strcpy(op, "GS_CLUE");
      set_opCode(m, op);
      set_gameId(m, stats_get_gameID(stats));
		  set_guideId(m, team_get_guideId(get_team(message), stats));
		  set_kragId(m, get_kragId_from_clue(clue, stats_get_krag_ht(stats)));
		  set_clue(m, clue);
		  send_message(m, team_get_guideId(get_team(message), stats), sock);
    }
  }
  // if want to send two clues
  else {
    // do twice (sending max two clues)
    for (int i = 1; i < 3; i++){
			// extract a clue
      char *clue = bag_extract(bag_of_clues);
      // if bag wasn't empty...
      if (clue != NULL){
	message_t *m = message_new();
	char *op = malloc(12 * sizeof(char*));
	strcpy(op, "GS_CLUE");
	set_opCode(m, op);
	set_gameId(m, stats_get_gameID(stats));
	set_guideId(m, team_get_guideId(get_team(message), stats));
				set_kragId(m, get_kragId_from_clue(clue, stats_get_krag_ht(stats)));
				set_clue(m, clue);
				send_message(m, team_get_guideId(get_team(message), stats), sock);
      }
    }
  }
}

/**************** send_GS_SECRET() ****************/
/*
 *  The Game Server sends a copy of the hacker’s secret to a Guide Agent, with some characters revealed, 
 *  after a Field Agent successfully locates a krag.
 */
// message should look like this opCode=GS_SECRET|gameId=FEED|guideId=0707|secret=com_____ _cie____50                                                      
static void send_GS_SECRET(message_t *message, game_stats_t *stats, socket_struct_t *sock){
  message_t *m = message_new();
  char *op = malloc(12 * sizeof(char*));
  strcpy(op, "GS_SECRET");
  set_opCode(m, op);
  set_gameId(m, stats_get_gameID(stats));
  set_guideId(m, team_get_guideId(get_team(message), stats));
  // allocate memory for secret before copying it                                                                                                    
  char *secret = malloc(strlen(team_get_secret(get_team(message), stats)) + 1);
  strcpy(secret, team_get_secret(get_team(message), stats));
  set_secret(m, secret);
  //printf("if its not printing this ..in send_GS_secret, this is the secr.. look at line 782 oc");
  printf("\n\n\n\n\n in send_GS_secret, this is the secret that is being senttttt: %s \n\n\n", secret);
  // send message to guide                                                                                                                           
  send_message(m, team_get_guideId(get_team(message), stats), sock);
  
}


/**************** send_GS_AGENT() ****************/
/*
 *  When the GS receives a statusReq from a GA, it shall send a GAME_STATUS message 
 *  followed by one GS_AGENT message to the GA for each FA on the GA’s team.
 */
static void send_GS_AGENT(message_t *message, game_stats_t *stats, socket_struct_t *sock){
  set_t* players = team_get_players(get_team(message), stats);
  message_sock_struct_t *message_sock = message_sock_struct_new(message,sock,stats);
  set_iterate(players, message_sock, send_GS_AGENT_helper);
  // free up the struct...
  free(message_sock);																		// could core dump?
}

/**************** send_GS_AGENT_helper() ****************/
/*
 *  Helper for send_GS_AGENT() - called on each player of the GA's team to get the player's stats
 */
static void send_GS_AGENT_helper(void *arg, const char *key, void *item){
  // rename args...
  struct message_sock_struct *message_sock = arg;
  message_t *message = message_sock->message;
  socket_struct_t *sock = message_sock->sock;
  game_stats_t *stats = message_sock->stats;
  // cast pebId to unsigned int
  unsigned int pebbleId = strtoul(key, NULL, 16);
  // pack new message to be sent out
  message_t *m = message_new();
  char *op = malloc(12 * sizeof(char*));
  strcpy(op, "GS_AGENT");
  set_opCode(m, op);
  set_gameId(m, stats_get_gameID(stats));
  set_pebbleId(m, pebbleId);
  
  // allocate memory for team name before copying it over
  char *team_cpy = malloc(10);
  strcpy(team_cpy, get_team(message));
  // copy it in
  set_team(m, team_cpy);
  
  // allocate memory for player's name before copying it
  char *player_name = malloc(strlen(player_get_name(pebbleId, get_team(message), stats)) + 1);
  strcpy(player_name, player_get_name(pebbleId, get_team(message), stats));
  // copy it in
  set_player(m, player_name);
  
  set_latitude(m, player_get_latitude(pebbleId, get_team(message), stats));
  set_longitude(m, player_get_longitude(pebbleId, get_team(message), stats));
  
  time_t curr;
  time (&curr);
  int dif = (int) difftime(curr,player_get_last_contact(pebbleId, get_team(message), stats));	
  set_lastContact(m, dif);
  
  // sends message to GA
  send_message(m, team_get_guideId(get_team(message), stats), sock);
}

/**************** forward_GA_HINT() ****************/
/*
 *  If GS receives GA_HINT message, it forwards it to the GA's field agents via this message
 *  Does so according to the following rules:
 *
 *  "if pebbleId is *, send this complete message, unchanged, to all FAs on GA’s team.
 *  otherwise, send this complete message, unchanged, to the named FA."
 */
static void forward_GA_HINT(message_t *message, game_stats_t *stats, socket_struct_t *sock){
  // copy message into new message struct (will be identical)
  message_t *m = message_new();
  char *op = malloc(12 * sizeof(char*));
  strcpy(op, "GA_HINT");
  set_opCode(m, op);
  set_gameId(m, get_gameId(message));
  set_guideId(m, get_guideId(message));
  
  // allocate memory for team name before copying it over
  char *team_cpy = malloc(strlen(get_team(message)) + 1);
  strcpy(team_cpy, get_team(message));
  // copy it in
  set_team(m, team_cpy);
  
  // allocate memory for player's name before copying it
  char *player_name = malloc(strlen(get_player(message)) + 1);
  strcpy(player_name, get_player(message));
  // copy it in
  set_player(m, player_name);
  
  set_pebbleId(m, get_pebbleId(message));
  
  // allocate memory for hint before copying it over
  char *hint_cpy = malloc (strlen(get_hint(message)) + 1);
  strcpy(hint_cpy, get_hint(message));
  // copy it in
  set_hint(m, hint_cpy);
  
  // now do the forwarding....
  unsigned int pebbleId = get_pebbleId(m);
  // if we're broadcasting the hint (* = '2a'), we need to iterate through players
  if (pebbleId == strtoul("2a", NULL, 16)){
    set_t* players = team_get_players(get_team(m), stats);
    message_sock_struct_t *message_sock = message_sock_struct_new(m,sock,stats);
    set_iterate(players, message_sock, broadcast_GA_HINT);
    // free up the struct...
    free(message_sock);																		// could core dump?
  } else {
    send_message(m, pebbleId, sock);
  }
}

/**************** broadcast_GA_HINT() ****************/
/*
 *  Iterator for forward_GA_HINT. Called on each player on GA's team to broadcast the GA_HINT
 */
static void broadcast_GA_HINT(void *arg, const char *key, void *item){
  // rename args
  struct message_sock_struct *message_sock = arg;
  message_t *message = message_sock->message;
  socket_struct_t *sock = message_sock->sock;
  // cast pebId to unsigned int
  unsigned int pebbleId = strtoul(key, NULL, 16);
  // sends message to this player
  send_message(message, pebbleId, sock);
}

/**************** legit_location() ****************/
/*
 *  Returns true if krag is within 10m of the given latitude and longitude; false otherwise
 */
static bool legit_location(float lat, float lon, unsigned int kragId, game_stats_t *stats){
  // get hashtable of krags
  hashtable_t *krags = stats_get_krag_ht(stats);
  char krag_str[9];
  sprintf(krag_str, "%x", kragId);
  // search for kragId in hashtable
  krag_info_t *info = hashtable_find(krags, krag_str);
  float latitude = krag_get_latitude(info);
  float longitude = krag_get_longitude(info);
  // if (absoluteVal(lat-latitude) < 10) and (absoluteVal(long-longitude) < 10), then checks out!
  if ((fabs(lat - latitude) < 10.0) && (fabs(lon - longitude) < 10.0)){
    return true;
  }
  return false;
}

/**************** krag_claimed_by_team() ****************/
/*
 *  Returns true if krag has already been claimed by team; false otherwise
 */
static bool krag_claimed_by_team(unsigned int kragId, char *team, game_stats_t *stats){
  // get the claimed_krags[] array from the team's struct
  char **claimed_krags = team_get_claimed_krags_array(team, stats);
  // get the number of claimed krags from the team's struct
  int num_claimed_krags = team_get_num_claimed_krags(team, stats);
  // search to see if kragId is in the array
  for (int i = 0; i < num_claimed_krags; i++){
    char *krag = claimed_krags[i];
    // convert krag to string
    char krag_str[9];
    sprintf(krag_str, "%x", kragId);
    // see if they match
    if (strcmp(krag, krag_str) == 0){
      return true;
    }
  }
  // if didn't find it, return false
  return false;
}

/**************** claim_krag() ****************/
/*
 *  Adds krag to team's list of claimed krags.
 */
static void claim_krag(unsigned int kragId, char *team, game_stats_t *stats){
  
  
  printf("in claim_krag\n");
  
  // get the claimed_krags[] array from the team's struct
  char **claimed_krags = team_get_claimed_krags_array(team, stats);
  

  if (claimed_krags == NULL){
    printf("array was null\n");
  }
  
  
  
  
	// get the number of claimed krags from the team's struct
  int num_claimed_krags = team_get_num_claimed_krags(team, stats);
  
  
  // add the krag to the array at index num_claimed_krags
  char *krag_str = malloc(sizeof(char*) * 9);
  sprintf(krag_str, "%x", kragId);
  
  
  printf("about to insert krag %s into array\n", krag_str);
  
  claimed_krags[num_claimed_krags] = krag_str;

  printf("inserted krag %s into array\n", krag_str);
  
  // increment num_krags_claimed
  team_increment_num_krags(team, stats);
}

/**************** update_secret() ****************/
/*
 *  Updates the team's secret string to display letters revealed by kragId
 */
static void update_secret(char *team, unsigned int kragId, game_stats_t *stats){
  
  // get the secret string from gamestats                                                                                                                    
  char *secret = stats_get_secret(stats);
  // get the number of krags from gamestats                                                                                                          
  int numkrags = stats_get_numkrags(stats);
  printf("this i snum krags %d", numkrags);
  // put length of string into int length                                                                                                            
  int length = strlen(secret);
  // get the team's secret string                                                                                                                    
  char* team_secret = malloc(strlen(team_get_secret(team, stats))+1);
  strcpy(team_secret, team_get_secret(team, stats));
  free(team_get_secret(team, stats));
  
  
  // get the krag's index from its "info" struct                                                                                                     
  hashtable_t *krags = stats_get_krag_ht(stats);
  char krag_str[9];
  sprintf(krag_str, "%x", kragId);
  // search for kragId in hashtable                                                                                                                  
  krag_info_t *info = hashtable_find(krags, krag_str);
  int i = krag_get_index(info);
  
  // loop through team's copy of secret, updating from underscores when appropriate
  // loop through team's copy of secret, updating from underscores when appropriate
  
  //When a team claims krag number i, reveal characters at all positions j where j % k == i.
  // k is number of kragsssss
  printf("\n i:%d   numkrags:%d", i, numkrags);
  
  for (int j = 0; j < length && team_secret[j] != '\0'; j++){ // does this need to be length - 1 ?????
    if (j % numkrags == i){
      printf("this is secret[j]: %s\n\n", secret);
      printf("this is team_secret[j]: %s\n\n", team_secret);
      printf("this is j: %d\n", j);
      // update this underscore from underscore to respective character                                                                  
      team_secret[j] = secret[j];
    }
  }
  
  team_set_secret(team, stats, team_secret);
}

/* 
 * handle_game_over()
 * handles game over by sending a TEAM_RECORD and GAME_OVER message to every
 * player in the game
 */
void handle_game_over(game_stats_t *gs, socket_struct_t *sock_struct) {
  message_sock_struct_t *mss = message_sock_struct_new(NULL, sock_struct, gs);
  set_t *teamset = stats_get_teamset(gs);
  // Send messages to every team
  set_iterate(teamset,mss, send_game_over_to_team);
  exit(0);
}

/* 
 * send_game_over_to_team()
 * sends the game over and team record messages to everyone in the team
 */
static void send_game_over_to_team(void *arg, const char *key, void *item) {
  
  message_sock_struct_t *mss = arg;
  socket_struct_t *sock = mss->sock;
  game_stats_t *stats = mss->stats;
  team_t *team = item;
  message_t *tr = assemble_team_record(team, sock, stats);
  char *teamname = team_get_name(team);
  send_message(tr, team_get_guideId(teamname, stats), sock);
  message_t *go = assemble_game_over(sock, stats);
  send_message(go, team_get_guideId(teamname, stats), sock);
  set_iterate(team_get_players(teamname, stats), mss, send_game_over_to_player);
}

/* 
 * assemble_team_record()
 * assembles a TEAM_RECORD message for a given team
 *
 */
static message_t *assemble_team_record(team_t *team, socket_struct_t *sock, game_stats_t *stats) {
  message_t *m = message_new();
  char *op = malloc(12 * sizeof(char));
  op = "TEAM_RECORD";
  set_opCode(m, op);
  set_gameId(m, stats_get_gameID(stats));
  char* teamname = malloc(10);
  strcpy(teamname, team_get_name(team));
  set_team(m, teamname);;
  set_numPlayers(m, get_num_players(teamname, stats));
  set_numClaimed(m, team_get_num_claimed_krags(teamname, stats));
  return m;
}

/* 
 * assemble_game_over
 * assembles a GAME_OVER message for a given game
 *
 */
static message_t * assemble_game_over(socket_struct_t *sock, game_stats_t *stats) {
  message_t *m = message_new();
  char *op = malloc(10 *sizeof(char));
  op = "GAME_OVER";
  set_opCode(m, op);
  set_gameId(m, stats_get_gameID(stats));
  char* secret = malloc(141 * sizeof(char));
  secret = stats_get_secret(stats);
  set_secret(m, secret);
  return m;
}


/* 
 * Send_game_over_to_player
 * sends a GAME_OVER and TEAM_RECORD message to a player
 * 
 */ 
static void send_game_over_to_player(void * arg, const char *key, void *item) {
  message_sock_struct_t *mss = arg;
  socket_struct_t *sock = mss->sock;
  game_stats_t *stats = mss->stats;
  player_t *player= item;
  char*teamname;
  teamname = player_get_team_name(player);
  team_t *team = stats_get_team(teamname, stats);
  message_t *tr = assemble_team_record(team, sock, stats);
  unsigned int playerId = strtoul(key, NULL, 16);
  send_message(tr, playerId, sock);
  message_t *go = assemble_game_over(sock, stats);
  send_message(go, playerId, sock);
}


