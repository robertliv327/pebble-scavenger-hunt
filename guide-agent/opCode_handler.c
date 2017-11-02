/*
 * opCode_handler.c     Eleph     May, 2017
 *
 * Handles the processing of each opCode for Guide Agent. 
 * Called when a message is received by Guide Agent
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
#include "message.h"
#include "hashtable.h"
#include "opCode_handler.h"
#include "GAstats.h"
#include "message_handler.c"

/**************** file−local global variables ****************/ 

/**************** local function prototypes ****************/ 
static void GAME_STATUS(GA_stats_t* g, message_t *message);
static bool validate_GAME_STATUS(message_t *message);
static void GS_AGENT(GA_stats_t* g, message_t *message);
static bool validate_GS_AGENT(message_t *message);
static void GS_CLUE(GA_stats_t* g, message_t *message);
static bool validate_GS_CLUE(message_t *message);
static void GS_CLAIMED(GA_stats_t* g, message_t *message);
static bool validate_GS_CLAIMED(message_t *message);
static void GS_SECRET(GA_stats_t* g, message_t *message);
static bool validate_GS_SECRET(message_t *message);
static void GS_RESPONSE(GA_stats_t* g, message_t *message);
static bool validate_GS_RESPONSE(message_t *message);
static void GAME_OVER(GA_stats_t* g, message_t *message);
static void TEAM_RECORD(GA_stats_t* g, message_t *message);

/**************** function dispatch table ****************/
static const struct {
	const char *opCode;
	void (*func)(GA_stats_t* g, message_t *message);
} dispatch[] = {
	{ "GAME_STATUS", GAME_STATUS },
	{ "GS_AGENT", GS_AGENT },
	{ "GS_CLUE", GS_CLUE },
	{ "GS_CLAIMED", GS_CLAIMED },
	{ "GS_SECRET", GS_SECRET },
	{ "GS_RESPONSE", GS_RESPONSE },
	{ "GAME_OVER", GAME_OVER },
	{ "TEAM_RECORD", TEAM_RECORD },
	{ NULL, NULL }
};

/**************** do_opCode() ****************/
void do_opCode(GA_stats_t* g, message_t *message){
	// get the opCode from the message struct
	char *opCode = get_opCode(message);

	// get function pointer that matches the opCode and call the function
	for (int fn = 0; dispatch[fn].opCode != NULL; fn++) {
		if (strcmp(opCode, dispatch[fn].opCode) == 0) {
			(*dispatch[fn].func)(g, message);
			break;
		}
	}
	// if opCode didn't match, do nothing
}

/**************** GAME_STATUS() ****************/
/*
 *  handles incoming message with opcode "GAME_STATUS"
 * 
 *  A GAME_STATUS message is sent by the Game Server to either a Field Agent 
 *  or Guide Agent upon request for a status update (as indicated by the statusReq field).
 *
 */
static void GAME_STATUS(GA_stats_t* g, message_t *message){
	// validate subset of fields included in message... if correct, proceed; otherwise, do nothing
	if (validate_GAME_STATUS(message)){
	  // update its internal records about the state of the game. (num_claimed, num_krags)
	  GA_set_gameID(g, get_gameId(message));
	  GA_set_teamname(g, get_team(message));
	  GA_set_numClaimed(g, get_numClaimed(message));
          GA_set_numKrags(g, get_numKrags(message));
	}
}

/**************** validate_GAME_STATUS() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_GAME_STATUS(message_t *message){
	// expects: opCode=GAME_STATUS|gameId=|guideId=|numClaimed=|numKrags=
	if ((strcmp("GAME_STATUS", get_opCode(message)) == 0) && (get_gameId(message) != 99999999) && \
		(get_guideId(message) != 99999999) && (get_numClaimed(message) != -1) && (get_numKrags(message) != -1)){		
		// does not expect: respCode, kragId, lastContact, latitude, team, player, pebbleId, longitude, numPlayers,
		//					statusReq, hint, clue, secret, text
		if ((get_respCode(message) == NULL) && (get_kragId(message) == 0) && \
			(get_lastContact(message) == -1) && (get_latitude(message) == 200.0) && \
			(get_team(message) == NULL) && (get_player(message) == NULL) && (get_pebbleId(message) == 0) && \
			(get_longitude(message) == 200.0) && (get_numPlayers(message) == -1) && \
			(get_statusReq(message) == -1) && (get_hint(message) == NULL) && \
			(get_clue(message) == NULL) && (get_secret(message) == NULL) && (get_text(message) == NULL)){

			// all fields are correct ; proceed with message handling!
			return true;
		}
	}
	return false;
}

/**************** GS_AGENT() ****************/
/*
 *  handles incoming message with opcode "GS_AGENT"
 *
 *	When the GS receives a statusReq from a GA, it shall send a GAME_STATUS message followed 
 *  by one GS_AGENT message to the GA for each FA on the GA’s team.
 *
 */
 static void GS_AGENT(GA_stats_t* g, message_t *message){
	// validate subset of fields included in message... if correct, proceed; otherwise, do nothing
   
   if (validate_GS_AGENT(message)){	 
     // GA_set_teamname(g, get_team(message));
     // update the associated FA’s information
     char *team = get_team(message);
     
     //     char* teamname = malloc(sizeof(get_team(message)+1));
     // strcpy(teamname, get_team(message));
     char *team_cpy = malloc(strlen(team) + 1);
     strcpy(team_cpy, team);
     char *player_name = malloc(strlen(get_player(message)) + 1);
     strcpy(player_name, get_player(message));
     
     GA_set_teamname(g, team_cpy);
     //char* playername = malloc(sizeof(get_player(message)+1));
     //strcpy(playername, get_player(message));
     unsigned int pebbleId = get_pebbleId(message);
	  //update_player(pebbleId, g,  player_name, team_cpy, get_latitude(message), get_longitude(message));
     float lat = get_latitude(message);
     float longi = get_longitude(message);
     lat_long_t *lat_long = lat_long_new(lat, longi);
     fprintf(stdout, "the lat: %f, the long: %f", lat, longi); 
     update_player(pebbleId, g,  player_name, team_cpy, lat_long);
	  // update internal record of when FA last contacted GA     
     player_set_FA_last_contact_time(pebbleId, g);
     free(lat_long);
     //        player_set_last_contact(pebbleId, g);
   }	
   
 }

/**************** validate_GS_AGENT() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_GS_AGENT(message_t *message){
	// expects: opCode=GS_AGENT|gameId=|pebbleId=|team=|player=|latitude=|longitude=|lastContact=
	if ((strcmp("GS_AGENT", get_opCode(message)) == 0) && (get_gameId(message) != 99999999) && \
		(get_pebbleId(message) != 0) && (get_team(message) != NULL) && (get_player(message) != NULL) && \
	    (get_latitude(message) != 200.0) && (get_longitude(message) != 200.0) && (get_lastContact(message) != -1)){		
		// does not expect: respCode, kragId, numPlayers, statusReq, hint, clue, secret, text,
		//					guideId, numClaimed, numKrags
		if ((get_respCode(message) == NULL) && (get_kragId(message) == 0) && (get_numPlayers(message) == -1) && (get_statusReq(message) == -1) && (get_hint(message) == NULL) && \
			(get_clue(message) == NULL) && (get_secret(message) == NULL) && (get_text(message) == NULL) && \
			(get_guideId(message) == 99999999) && (get_numClaimed(message) == -1) && (get_numKrags(message) == -1)){
			// all fields are correct ; proceed with message handling!
			return true;
		}
	}
	return false;
}

/**************** GS_CLUE() ****************/
/*
 *  handles incoming message with opcode "GS_CLUE"
 *
 *	The Game Server may send one or more clues to a Guide Agent after a 
 *  Field Agent successfully locates a krag.
 *
 */
 static void GS_CLUE(GA_stats_t* g, message_t *message){
	// validate subset of fields included in message... if correct, proceed; otherwise, do nothing
   //   char* clueM = message_to_string(message);
   // printf("GS CLUE: %s", clueM);
   
      if (validate_GS_CLUE(message)){

     // update records regarding known clues
	  
	char* kragID = malloc(sizeof(char)*5);

	//  char kragID[5];
          sprintf(kragID, "%x", get_kragId(message));
	  //strcpy(kragID, get_kragId(message));
	  char* clue = malloc(sizeof(get_clue(message)+1));
	  strcpy(clue, get_clue(message));
	  clue_bool_t* c = clue_bool_new(clue, false);
	  set_insert(GA_get_kragId_to_clue(g), kragID, c);
	  bag_insert(GA_get_known_clues(g), clue);
	  
	  	}
	
}

/**************** validate_GS_CLUE() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_GS_CLUE(message_t *message){
	// expects: opCode=GS_CLUE|gameId=|guideId=|clue=
  
	if ((strcmp("GS_CLUE", get_opCode(message)) == 0) && (get_gameId(message) != 99999999) && \
	    (get_guideId(message) != 99999999)/* && (get_kragId(message) != 0)*/ && \
		(get_clue(message) != NULL)){
		// does not expect: respCode, kragId, numPlayers, statusReq, hint, secret, text,
		//					numClaimed, numKrags, pebbleId, team, player, lastContact, lat, long
		if ((get_respCode(message) == NULL) && (get_latitude(message) == 200.0) && \
			(get_numPlayers(message) == -1) && (get_statusReq(message) == -1) && (get_hint(message) == NULL) && \
			(get_secret(message) == NULL) && (get_text(message) == NULL) && (get_longitude(message) == 200.0) && \
			(get_numClaimed(message) == -1) && (get_numKrags(message) == -1) && (get_pebbleId(message) == 0) && \
			(get_team(message) == NULL) && (get_player(message) == NULL) && (get_lastContact(message) == -1)){
			// all fields are correct ; proceed with message handling!
			return true;
		}
	}
	return false; 
}

/**************** GS_CLAIMED() ****************/
/*
 *  handles incoming message with opcode "GS_CLAIMED"
 *
 *	After a Field Agent successfully locates a krag, the Game Server shall send this message 
 *  to that team’s Guide Agent. This message allows a Guide Agent to know that the clue corresponding 
 *  to this kragId has been claimed and thus, for example, could be removed from the list of unsolved clues. 
 *  The message carries the krag’s location, allowing the Guide Agent to display the claimed krag on a map.
 *
 */
 static void GS_CLAIMED(GA_stats_t* g, message_t *message){
	// validate subset of fields included in message... if correct, proceed; otherwise, do nothing
	if (validate_GS_CLAIMED(message)){
	  // take in location of krag to display
	  // update records to note that the clue corresponding to the given krag has been solved and krag claimed.
	  // we need to remove the clue from the data structure but idc ho bc bag extract is randommmm
	  // krags found to draw - bag of lat longs struct
	  // hashtable of kragId to clue

	  float lat = get_latitude(message);
	  float lon = get_longitude(message);
	  lat_long_t* l = lat_long_new(lat, lon);
	  bag_insert(GA_get_krags_found(g), l);

	  // convert kragID to a character
	  unsigned int kragID = get_kragId(message);
	  char kragid[5];
	  sprintf(kragid, "%x", kragID);


	  clue_bool_t* c = set_find(GA_get_kragId_to_clue(g), kragid);
	  set_bool_to_true(c);
	  
	}
 }



/**************** validate_GS_CLAIMED() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */

static bool validate_GS_CLAIMED(message_t *message){
	// expects: opCode=GS_CLAIMED|gameId=|guideId=|pebbleId=|kragId=|latitude=|longitude=
	if ((strcmp("GS_CLAIMED", get_opCode(message)) == 0) && (get_gameId(message) != 99999999) && \
		(get_guideId(message) != 99999999) && (get_pebbleId(message) != 0) && (get_kragId(message) != 0) && \
		(get_latitude(message) != 200.0) && (get_longitude(message) != 200.0)){

		// does not expect: respCode, numPlayers, statusReq, hint, secret, text, clue
		//					numClaimed, numKrags, team, player, lastContact
		if ((get_respCode(message) == NULL) &&  \
			(get_numPlayers(message) == -1) && (get_statusReq(message) == -1) && (get_hint(message) == NULL) && \
			(get_secret(message) == NULL) && (get_text(message) == NULL) && (get_clue(message) == NULL) && \
			(get_numClaimed(message) == -1) && (get_numKrags(message) == -1) && \
			(get_team(message) == NULL) && (get_player(message) == NULL) && (get_lastContact(message) == -1)){
			// all fields are correct ; proceed with message handling!
			return true;
		}
	}
	return false;
}

/**************** GS_SECRET() ****************/
/*
 *  handles incoming message with opcode "GS_SECRET"
 *
 *	The Game Server sends a copy of the hacker’s secret to a Guide Agent, with some 
 *  characters revealed, after a Field Agent successfully locates a krag.
 *
 */
 static void GS_SECRET(GA_stats_t* g, message_t *message){
	// validate subset of fields included in message... if correct, proceed; otherwise, do nothing
   if (validate_GS_SECRET(message)){
		// update its display to present the new secret to the player.
	  char *secret = malloc(sizeof(get_secret(message)+1));
	  strcpy(secret, get_secret(message));
	  GA_set_secret(g, secret);	  
	}
 }
 
/**************** validate_GS_SECRET() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_GS_SECRET(message_t *message){
	// expects: opCode=GS_SECRET|gameId=|guideId=|secret=
	if ((strcmp("GS_SECRET", get_opCode(message)) == 0) && (get_gameId(message) != 99999999) && \
		(get_guideId(message) != 99999999) && (get_secret(message) != NULL)){
		// does not expect: respCode, kragId, numPlayers, statusReq, hint, text, latitude, longitude,
		//					clue, numClaimed, numKrags, pebbleId, team, player, lastContact
		if ((get_respCode(message) == NULL) && (get_kragId(message) == 0) && \
			(get_numPlayers(message) == -1) && (get_statusReq(message) == -1) && (get_hint(message) == NULL) && \
			(get_text(message) == NULL) && (get_latitude(message) == 200.0) && \
			(get_longitude(message) == 200.0) && (get_clue(message) == NULL) && \
			(get_numClaimed(message) == -1) && (get_numKrags(message) == -1) && (get_pebbleId(message) == 0) && \
			(get_team(message) == NULL) && (get_player(message) == NULL) && (get_lastContact(message) == -1)){
			// all fields are correct ; proceed with message handling!
			return true;
		}
	}
	return false;
}

/**************** GS_RESPONSE() ****************/
/*
 *  handles incoming message with opcode "GS_RESPONSE"
 *
 *	For debugging and convenience the Game Server shall respond to various conditions via this message.
 *
 */
static void GS_RESPONSE(GA_stats_t* g, message_t *message){
	// validate subset of fields included in message... if correct, proceed; otherwise, do nothing
	if (validate_GS_RESPONSE(message)){
		// optionally takes some recovery action based on the response code.
	}
}

/**************** validate_GS_RESPONSE() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_GS_RESPONSE(message_t *message){
	// expects: opCode=GS_RESPONSE|gameId=|respCode=|text= 
	if ((strcmp("GS_RESPONSE", get_opCode(message)) == 0) && (get_gameId(message) != 99999999) && 
		(get_respCode(message) != NULL) && (get_text(message) != NULL)){
		// does not expect: kragId, guideId, secret, numPlayers, statusReq, hint, latitude, longitude,
		//					clue, numClaimed, numKrags, pebbleId, team, player, lastContact
		if ((get_kragId(message) == 0) && (get_guideId(message) == 99999999) && (get_secret(message) == NULL) && \
			(get_numPlayers(message) == -1) && (get_statusReq(message) == -1) && (get_hint(message) == NULL) && \
			(get_latitude(message) == 200.0) && (get_longitude(message) == 200.0) && (get_clue(message) == NULL) && \
			(get_numClaimed(message) == -1) && (get_numKrags(message) == -1) && (get_pebbleId(message) == 0) && \
			(get_team(message) == NULL) && (get_player(message) == NULL) && (get_lastContact(message) == -1)){
			// all fields are correct ; proceed with message handling!
			return true;
		}
	}
	return false;
}

/**************** GAME_OVER() ****************/
/*
 *  handles incoming message with opcode "GAME_OVER"
 *
 *	When the game ends, the GS should first send all players (all FAs and all GAs) a TEAM_RECORD 
 *  message for each team known to the GS. Afterwards, the GS should send a GAME_OVER message.
 *
 */
static void GAME_OVER(GA_stats_t* g, message_t *message){

	char *secret = malloc(sizeof(get_secret(message)+1));
	strcpy(secret, get_secret(message));
	GA_set_secret(g, secret);

	// do nothing here - printing is handled and ncurses is closed in guideagent.c

}

/**************** TEAM_RECORD() ****************/
/*
 *  handles incoming message with opcode "TEAM_RECORD"
 *
 *	When the game ends, the GS should send one TEAM_RECORD message to every player 
 *  (all FAs and all GAs) for each team known to the GS
 *
 */
 static void TEAM_RECORD(GA_stats_t* g, message_t *message){
	// Upon receipt of this message, the Agent should store the information contained in this message, 
	// and prepare to display it upon receiving a GAME_OVER message.
	int gameId = get_gameId(message);
	int numClaimed = get_numClaimed(message);
	int numPlayers = get_numPlayers(message);

	char *team = malloc(sizeof(get_team(message)+1));
	strcpy(team, get_team(message));

	// store these stats somewhere in preparation for GAME_OVER...

	update_GA(g, gameId, team, numClaimed, numPlayers);
}
