/* FAstats.c  
 *
 * Eleph    May 2017
 *
 * Defines a struct that contains the statistics for a given guide agent
 * Has getters and setters for the various fields
 *
 * See FAstats.h for more information
 */

#include <pebble.h>
#include "FAstats.h"

 
// all of our variables
char* gameId;
char* guideId;
char* pebbleId;
char* team;
char* player;
int numClaimed;
int numKrags;
int numPlayers;
char *hints[] = {"empty", "empty", "empty", "empty", "empty", "empty", "empty"};
char *secret;
bool user_not_joined_game;
int statusReq;
bool krag_claim;
char *krag_to_send;
bool waiting_for_response;

// function to initialize all of the variables
void FA_stats_new(){
    gameId = NULL;
    pebbleId = NULL;
    guideId = NULL;
    team = NULL;
    player = NULL;
    numClaimed = -1;
    numKrags = -1;
    numPlayers = -1;
    user_not_joined_game = true;
    statusReq = 0;
    waiting_for_response = true;
}

// function to free all of the pointers at end of program
void FA_stats_delete() {
    if (hints!=NULL) {
      for (int i = 0; i < 7; i++){
        free(hints[i]);
      }
    }
    if (team!=NULL) {
        free(team);
    }
    if (player!=NULL) {
        free(player);
    }
    if (secret!=NULL) {
        free(secret);
    }
    if (gameId != NULL){
        free(gameId);
    }
    if (pebbleId != NULL){
        free(pebbleId);
    }
    if (guideId != NULL){
        free(guideId);
    }
    if (krag_to_send != NULL){
        free(krag_to_send);
    }
}

/************ GETTERS AND SETTERS (see FAstats.h for more info on variable purposes) ********/

bool FA_get_waiting_for_response(){
    return waiting_for_response;
}

void FA_set_waiting_for_response(bool b){
    waiting_for_response = b;
}

void FA_set_krag_to_send(char *krag){
    krag_to_send = krag;
}

char *FA_get_krag_to_send(){
    return krag_to_send;
}

void FA_set_krag_claim(bool b){
    krag_claim = b;
}

bool FA_get_krag_claim(){
    return krag_claim;
}

int FA_get_statusReq(){
    return statusReq;
}

void FA_set_statusReq(int req){
    statusReq = req;
}

bool FA_get_user_not_joined_game(){
    return user_not_joined_game;
}

void FA_set_user_not_joined_game(bool b){
    user_not_joined_game = b;
}

char *FA_get_gameId() {
    return gameId;
}

void FA_set_gameId(char *new_gameId) {
    gameId = new_gameId;
}


char *FA_get_pebbleId() {
    return pebbleId;
}

void FA_set_pebbleId(char *new_pebbleId) {
    pebbleId = new_pebbleId;
}


char *FA_get_guideId() {
    return guideId;
}

void FA_set_guideId(char *new_guideId) {
    guideId = new_guideId;
}


char * FA_get_team() {
    return team;
}

void FA_set_team(char * new_team) {
    team = new_team;
}

char * FA_get_player() {
    return player;
}

void FA_set_player(char * new_player) {
    player = new_player;
}

int FA_get_numKrags() {
    return numKrags;
}

void FA_set_numKrags(int new_numKrags) {
    numKrags = new_numKrags;
}

int FA_get_numClaimed() {
    return numClaimed;
}

void FA_set_numClaimed(int new_numClaimed) {
    numClaimed = new_numClaimed;
}

int FA_get_numPlayers(){
    return numPlayers;
}

void FA_set_numPlayers(int new_numPlayers){
    numPlayers = new_numPlayers;
}

char ** FA_get_hints(){
    return hints;
}

void FA_set_hints(){
    for (int i = 0; i < 7; i++){
        hints[i] = malloc(140*sizeof(char) + 1);
        strcpy(hints[i], "N/A");
    }
}

char * FA_get_secret() {
    return secret;
}

void FA_set_secret(char * new_secret) {
    secret = new_secret;
}
