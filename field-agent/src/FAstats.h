/*   
 *	GA_stats.h      Eleph     May, 2017
 *	
 *	stores all of the variables that we will need to access for the pebble
 *  each of the variables has a getter and setter method
 *
 */
 
#ifndef __FASTATS_H
#define __FASTATS_H

#include <pebble.h>

/******** List of Variables ********

char* gameId;					holds gameID
char* guideId;					holds guideID of guide agent
char* pebbleId;					holds pebbleId of field agent
char* team;			   			holds team's name
char* player;					holds player's name
int numClaimed;					holds number of claimed krags
int numKrags;					holds total number of krags
int numPlayers;					holds number of players on the team (for end of game)
char **hints[];					holds array of hints
char *secret;					holds the completed secret (for end of game)
int statusReq;					holds the statusReq
char *krag_to_send;				holds the string form of the krag we are about to send in a FA_CLAIM
bool krag_claim;				a flag to determine whether or not we are sending an FA_LOCATION prior to a krag claim
bool user_not_joined_game;		a flag to determine whether or not the player has joined the game yet
bool waiting_for_response;		a flag to determine whether or not we are waiting for a response to our request location

***********************************/

// initializer and deleter (frees the pointers)
void FA_stats_new();
void FA_stats_delete();

// getters
char ** FA_get_hints();
char * FA_get_gameId();
char * FA_get_pebbleId();
char * FA_get_guideId();
char * FA_get_team();
char * FA_get_player();
char * FA_get_secret();
char *FA_get_krag_to_send();
int FA_get_numKrags();
int FA_get_numClaimed();
int FA_get_numPlayers();
int FA_get_statusReq();
bool FA_get_krag_claim();
bool FA_get_waiting_for_response();
bool FA_get_user_not_joined_game();

// setters
void FA_set_gameId(char *new_gameId);
void FA_set_pebbleId(char *new_pebbleId);
void FA_set_guideId(char *new_guideId);
void FA_set_team(char * new_team);
void FA_set_player(char * new_player);
void FA_set_numKrags(int new_numKrags);
void FA_set_numClaimed(int new_numClaimed);
void FA_set_numPlayers(int new_numPlayers);
void FA_set_hints();
void FA_set_secret(char * new_secret);
void FA_set_user_not_joined_game(bool b);
void FA_set_statusReq(int req);
void FA_set_krag_to_send(char *krag);
void FA_set_waiting_for_response(bool b);
void FA_set_krag_claim(bool b);


#endif
