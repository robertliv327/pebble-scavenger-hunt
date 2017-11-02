/* 
 * opCode_handler.c     Eleph     May, 2017
 *
 * Handles the processing of each opCode for Field Agent. 
 * Called when a message is received by Field Agent
 * Does this processing by using a function table to match the incoming message struct to a function.
 * Then calls that funciton to perform correct response to message.
 * 
 * See opCode_handler.h for more information
 * 
 */

#include <pebble.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "FAstats.h"
#include "../../common/message.h"
#include "opCode_handler.h"
#include "windows/display_text_window.h"
#include "windows/game_over_window.h"
#include "windows/repick_player_name_window.h"
#include "windows/repick_team_name_window.h"

/**************** file−local global variables ****************/ 

/**************** local function prototypes ****************/ 
static void GA_HINT(message_t *message);
static bool validate_GA_HINT(message_t *message);
static void GAME_STATUS(message_t *message);
static bool validate_GAME_STATUS(message_t *message);
static void GS_RESPONSE(message_t *message);
static bool validate_GS_RESPONSE(message_t *message);
static void GAME_OVER(message_t *message);
static void TEAM_RECORD(message_t *message);

/**************** function dispatch table ****************/
static const struct {
	const char *opCode;
	void (*func)(message_t *message);
} dispatch[] = {
	{ "GAME_STATUS", GAME_STATUS },
	{ "GA_HINT", GA_HINT },
	{ "GS_RESPONSE", GS_RESPONSE },
	{ "GAME_OVER", GAME_OVER },
	{ "TEAM_RECORD", TEAM_RECORD },
	{ NULL, NULL }
};

/**************** do_opCode() ****************/
void do_opCode(message_t *message){
	// get the opCode from the message struct
	char *opCode = get_opCode(message);

	// get function pointer that matches the opCode and call the function
	for (int fn = 0; dispatch[fn].opCode != NULL; fn++) {
		if (strcmp(opCode, dispatch[fn].opCode) == 0) {
			(*dispatch[fn].func)(message);
			break;
		}
	}
	// if opCode didn't match, do nothing...
}

/**************** GA_HINT() ****************/
/*
 *  handles incoming message with opcode "GA_HINT"
 *
 *	To assist Field Agents in their game play, the GA requires a mechanism that 
 *	enables the Guide to send textual hints to its FAs.
 * 
 */
static void GA_HINT(message_t *message){
	// validate subset of fields included in message... if correct, proceed; otherwise, do nothing
	if (validate_GA_HINT(message)){
		/*** store the hint in memory for display to the player. ***/
		// first malloc and copy hint into a new string
		char *hint = malloc(strlen(get_hint(message)) + 1);
		strcpy(hint, get_hint(message));
		// then get the array from FAstats
		char **hint_array = FA_get_hints();
		
		// then insert hint into array
		free(hint_array[6]);
		for (int i = 6; i > 0; i--){
			hint_array[i] = hint_array[i-1];
		}
		hint_array[0] = hint;

		APP_LOG(APP_LOG_LEVEL_INFO, "Received hint: %s\n", hint);

		// notify the player about the arrival of a new hint.
		display_text_window_push("You've received a new hint! Go view it from the main menu.");
		
	}
}

/**************** validate_GA_HINT() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_GA_HINT(message_t *message){
	// expects: opCode=GA_HINT|gameId=|guideId=|team=|player=|pebbleId=|hint=
	if ((strcmp("GA_HINT", get_opCode(message)) == 0) && (get_gameId(message) != NULL) && \
		(get_guideId(message) != NULL) && (get_team(message) != NULL) && (get_player(message) != NULL) && \
		(get_pebbleId(message) != NULL) && (get_hint(message) != NULL)){
		
		// does not expect: respCode, kragId, lastContact, latitude, longitude, numPlayers,
		//					numClaimed, numKrags, statusReq, clue, secret, text
		if ((get_respCode(message) == NULL) && (get_kragId(message) == 0) && \
			(get_lastContact(message) == -1) && (get_latitude(message) == 200) && \
			(get_longitude(message) == 200) && (get_numPlayers(message) == -1) && \
			(get_numClaimed(message) == -1) && (get_numKrags(message) == -1) && (get_statusReq(message) == -1) && \
			(get_clue(message) == NULL) && (get_secret(message) == NULL) && (get_text(message) == NULL)){

			// all fields are correct ; proceed with message handling!
			return true;
		}
	}
	return false;
}

/**************** GAME_STATUS() ****************/
/*
 *  handles incoming message with opcode "GAME_STATUS"
 * 
 *  A GAME_STATUS message is sent by the Game Server to either a Field Agent 
 *  or Guide Agent upon request for a status update (as indicated by the statusReq field).
 *
 */
static void GAME_STATUS(message_t *message){
	// validate subset of fields included in message... if correct, proceed; otherwise, do nothing
	if (validate_GAME_STATUS(message)){
		// update its internal records about the state of the game. (num_claimed, num_krags, guideId)

		// free old values if they were not null
		if (FA_get_gameId() != NULL){
			free(FA_get_gameId());
		}
		if (FA_get_guideId() != NULL){
			free(FA_get_guideId());
		}

		// set FA_gameID
		char *game_copy = malloc(strlen(get_gameId(message)) + 1);
		strcpy(game_copy, get_gameId(message));
		FA_set_gameId(game_copy);

		// set FA_guideID
		char *guide_copy = malloc(strlen(get_guideId(message)) + 1);
		strcpy(guide_copy, get_guideId(message));
		FA_set_guideId(guide_copy);

		FA_set_numKrags(get_numKrags(message));
		FA_set_numClaimed(get_numClaimed(message));
	}	
}

/**************** validate_GAME_STATUS() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_GAME_STATUS(message_t *message){
	// expects: opCode=GAME_STATUS|gameId=|guideId=|numClaimed=|numKrags=
	if ((strcmp("GAME_STATUS", get_opCode(message)) == 0) && (get_gameId(message) != NULL) && \
		(get_guideId(message) != NULL) && (get_numClaimed(message) != -1) && (get_numKrags(message) != -1)){		
		// does not expect: respCode, kragId, lastContact, latitude, team, player, pebbleId, longitude, numPlayers,
		//					statusReq, hint, clue, secret, text
		if ((get_respCode(message) == NULL) && (get_kragId(message) == 0) && \
			(get_lastContact(message) == -1) && (get_latitude(message) == 200) && \
			(get_team(message) == NULL) && (get_player(message) == NULL) && (get_pebbleId(message) == NULL) && \
			(get_longitude(message) == 200) && (get_numPlayers(message) == -1) && \
			(get_statusReq(message) == -1) && (get_hint(message) == NULL) && \
			(get_clue(message) == NULL) && (get_secret(message) == NULL) && (get_text(message) == NULL)){

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
static void GS_RESPONSE(message_t *message){
	// validate subset of fields included in message... if correct, proceed; otherwise, do nothing
	if (validate_GS_RESPONSE(message)){
		// takes recovery action based on the response code	
		char *resp_code = get_respCode(message);
		if (strcmp(resp_code, "SH_ERROR_INVALID_MESSAGE") == 0){
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Got SH_ERROR_INVALID_MESSAGE...\n");
		}
		// if the player rejoined the game with a team name they didn't previously use
		else if (strcmp(resp_code, "SH_ERROR_INVALID_TEAMNAME") == 0){
			// tell player to re-pick the team - must be team previously used
			free(FA_get_team());
			FA_set_team(NULL);
			FA_set_user_not_joined_game(true);
			FA_set_waiting_for_response(true);
			repick_team_name_window_push("Choose the team you previously used. (Back button)");
		}
		// if the player rejoined the game with a player name they didn't previously use
		else if (strcmp(resp_code, "SH_ERROR_INVALID_PLAYERNAME") == 0){
			// tell player to re-pick the player name - must be player name previously used
			free(FA_get_player());
			FA_set_player(NULL);
			FA_set_user_not_joined_game(true);
			FA_set_waiting_for_response(true);
			repick_player_name_window_push("Choose the name you previously used. (Back button)");
		}
		// if the player joins the game under a name that someone else on the team has taken
		else if (strcmp(resp_code, "SH_ERROR_DUPLICATE_PLAYERNAME") == 0){
			// tell player that the name is already in use on team...
			free(FA_get_player());
			FA_set_player(NULL);
			FA_set_user_not_joined_game(true);
			FA_set_waiting_for_response(true);
			repick_player_name_window_push("That name is already in use. Pick again. (Back button)");
		}
		// if the player successfully claims a krag
		else if (strcmp(resp_code, "SH_CLAIMED") == 0){
			// tell player that they successfully claimed a krag
			int num = FA_get_numClaimed();
			FA_set_numClaimed(num + 1);
			display_text_window_push("Congrats! You've claimed a krag."); 
		}
		// if the user has claimed a krag that the team has already claimed
		else if (strcmp(resp_code, "SH_CLAIMED_ALREADY") == 0){
			// tell player that they already claimed that krag 
			display_text_window_push("You've already claimed this krag! Entry not allowed.");
		}
	}
}

/**************** validate_GS_RESPONSE() ****************/
/*
 *  validate subset of fields included in message (test that expecteds are not disabled, unexpecteds are disabled)
 */
static bool validate_GS_RESPONSE(message_t *message){
	// expects: opCode=GS_RESPONSE|gameId=|respCode=|text= 
	if ((strcmp("GS_RESPONSE", get_opCode(message)) == 0) && (get_gameId(message) != NULL) && 
		(get_respCode(message) != NULL) && (get_text(message) != NULL)){
		// does not expect: kragId, guideId, secret, numPlayers, statusReq, hint, latitude, longitude,
		//					clue, numClaimed, numKrags, pebbleId, team, player, lastContact
		if ((get_kragId(message) == 0) && (get_guideId(message) == NULL) && (get_secret(message) == NULL) && \
			(get_numPlayers(message) == -1) && (get_statusReq(message) == -1) && (get_hint(message) == NULL) && \
			(get_latitude(message) == 200) && (get_longitude(message) == 200) && (get_clue(message) == NULL) && \
			(get_numClaimed(message) == -1) && (get_numKrags(message) == -1) && (get_pebbleId(message) == NULL) && \
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
static void GAME_OVER(message_t *message){
	// Upon receipt of this message, the Agent notifies its player that the game is over and reports 
	// (in some suitable format) the overall game results (stored by TEAM_RECORD), and the fully revealed secret. 
	APP_LOG(APP_LOG_LEVEL_INFO, "Received GAME_OVER!\n");
	FA_set_secret(get_secret(message));
	window_stack_pop_all(false);
	game_over_window_push();
}

/**************** TEAM_RECORD() ****************/
/*
 *  handles incoming message with opcode "TEAM_RECORD"
 *
 *	When the game ends, the GS should send one TEAM_RECORD message to every player 
 *  (all FAs and all GAs) for each team known to the GS
 *
 */
static void TEAM_RECORD(message_t *message){
	// Upon receipt of this message, the Agent should store the information contained in this message, 
	// and prepare to display it upon receiving a GAME_OVER message.
	char *game_copy = malloc(strlen(get_gameId(message)) + 1);
	strcpy(game_copy, get_gameId(message));
	FA_set_gameId(game_copy);
	FA_set_numClaimed(get_numClaimed(message));
	FA_set_numPlayers(get_numPlayers(message));
	// not doing anything with message's team field b/c will already have it stored...
}