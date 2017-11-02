/*                                                                                                                                                               
 * gamestats.h     Eleph     May, 2017                                                                                                                           
 *                                                                                                                                                               
 * Defines a struct that contains the statistics for a given game                                                                                                
 * Has getters for the various fields                                                                                                                            
 *                                                                                                                                                               
 * Functionality:
 * ** this struct created with this funct: game_stats_new();
 *       Statistic x can be retrieved by calling stats_get_x(struct s);
 *                                                                                                                                                               
 */

#ifndef __GAMESTATS_H
#define __GAMESTATS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/set.h"
#include "../libcs50/counters.h"

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
typedef struct team_and_name team_and_name_t;
team_and_name_t* team_and_name_new();
void* team_and_name_delete();
void game_stats_delete(game_stats_t *g);


/**************** getters for GS struct  ****************/
/*                                                                                                                                                                                
 * return the value of a field in the struct if its not NULL
 * return NULL equivalent otherwise
 * 
 *                                                                                                                                                                                
 */
void stats_set_gameID(game_stats_t* s, unsigned int gameID);
unsigned int stats_get_gameID(game_stats_t* s);
int stats_get_numkrags(game_stats_t* s);
int stats_get_numagents(game_stats_t* s);
int stats_get_numteams(game_stats_t* s);
int stats_get_timesincestart(game_stats_t* s);
char* stats_get_secret(game_stats_t* s);
bag_t* stats_get_master_bag(game_stats_t* s);
hashtable_t* stats_get_krag_ht(game_stats_t* s);
team_t* stats_get_team(char* teamname, game_stats_t* s);
set_t* stats_get_teamset(game_stats_t* s);

/**************** setters for GS struct  ****************/
/*                                                                                                                                                                                
 * set a given field in the struct 
 *                                                                                                                                                                                
 */
void set_main_hashtable(game_stats_t *gs, hashtable_t *ht);
void set_main_bag_of_clues(game_stats_t * gs, bag_t* bag);
void set_main_secret(game_stats_t *gs, char* secret);
void set_gameId_gs(game_stats_t *gs, unsigned int gameId);
void set_num_krags_gs(game_stats_t *gs, int num_krags);

/**************** getters and setters for team stats struct  ****************/
/*                                                                                                                                                                                  
 * return the value of a field in the struct if its not NULL                                                                                                                        
 * return NULL equivalent otherwise   
 * OR                                                                                                                                              
 * sets/increments a value of a field if the struct is not NULL                                                                                                                     
 *                                              
 *                                                                                                                                                                                  
 */
void team_set_secret(char* teamname, game_stats_t* s, char* secret);
char * team_get_name(team_t *team);
unsigned int team_get_guideId(char* teamname, game_stats_t* s);
char* team_get_guide_name(char* teamname, game_stats_t* s);
time_t team_get_GA_last_contact(char* teamname, game_stats_t* s);
char** team_get_claimed_krags_array(char* teamname, game_stats_t* s);
int team_get_num_claimed_krags(char* teamname, game_stats_t* s);
char* team_get_secret(char* teamname, game_stats_t* s);
bag_t* team_get_bag_of_clues(char* teamname, game_stats_t* s);
player_t* team_get_player(unsigned int pebbleID, char* teamname, game_stats_t* s);
set_t* team_get_players(char* teamname, game_stats_t* s);
bool is_player_on_team(char* playername, char* teamname, game_stats_t* s);
void team_increment_num_krags(char* teamname, game_stats_t* s);
int player_check_Id(game_stats_t *gs, unsigned int Id, char* team, char* name);
int get_num_players(char * teamname, game_stats_t *s);

/**************** getters for player stats struct  ****************/
/*                                                                                                                                                   
 * return the value of a field in the struct if its not NULL                                                                     
 * return NULL equivalent otherwise                                                                                              
 *                                  
 *                                                                                                                           
 *                                                                                                                        
 */

char* player_get_name(unsigned int pebbleID, char* teamname, game_stats_t* s);
char* player_get_team_name(player_t *player);
float player_get_latitude(unsigned int pebbleID, char* teamname, game_stats_t* s);
float player_get_longitude(unsigned int pebbleID, char* teamname, game_stats_t* s);
time_t player_get_last_contact(int pebbleID, char* teamname, game_stats_t* s);
void player_set_FA_last_contact(int pebbleID, char* teamname, game_stats_t* s);
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
void update_player(unsigned int pebbleID, char* teamname, char* playername, float latitude, float longitude, game_stats_t* s);

/**************** register_guide ****************/
/*                                                                                                                                                                                  
 * update's stats relating to GA in a team's struct
 * updates last contact time 
 */
void register_guide(unsigned int guideId, char* guide_name, char* teamname, game_stats_t* s);

/**************** update_guide ****************/
/*                                                                                                                                                                                  
 * updates last contact time for the GA
 *
 */
void update_guide(char* teamname, game_stats_t* s);

/**************** game_print ******************/
/* 
 * prints all game stats
 *
 */
void game_print(game_stats_t* s);
void team_print(void* arg, const char* key, void* item);
void player_print(void* arg, const char* key, void* item);

#endif //__GAMESTATS_H
