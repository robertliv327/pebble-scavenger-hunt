/**************** struct definitions/initializers  ****************/
/*                                                                                                                                                                              
 * define and create a GA struct to store stats local to GA
 * define and create a player struct to store stats for the player that the GA needs  
 *                            
 */

#include "../libcs50/bag.h"
#include "../libcs50/set.h"

typedef struct lat_long lat_long_t;
lat_long_t* lat_long_new(float lat, float lon);
typedef struct GA_stats GA_stats_t;
GA_stats_t* GA_stats_new(void);
void GA_stats_delete(GA_stats_t* s);
typedef struct player player_t;
player_t* player_new(void);
void player_delete(player_t* p);
typedef struct clue_bool clue_bool_t;
clue_bool_t* clue_bool_new(char* clue, bool solved);
void set_bool_to_true(clue_bool_t* c);
bool get_solved_bool(clue_bool_t* c);
char* get_unsolved_clue(clue_bool_t* c);

/**************** getters and setters for GA struct  ****************/
/*                                                                                                                               
 * return the value of a field in the struct if it’s not NULL                                                                     
 * return NULL equivalent otherwise                                                                                              
 OR                                             
 * sets value of a field if the struct is not NULL                        
 *                                                                                                                                                               
 */

int GA_get_numPlayers(GA_stats_t* g);
unsigned int GA_get_gameId(GA_stats_t* g);
void GA_set_gameID(GA_stats_t* g, unsigned int gameID);
char* GA_get_teamname(GA_stats_t* g);
void GA_set_teamname(GA_stats_t* g, char* teamname);
int GA_get_numClaimed(GA_stats_t* g);
void GA_set_numClaimed(GA_stats_t* g, int numClaimed);
int GA_get_numKrags(GA_stats_t* g);
void GA_set_numKrags(GA_stats_t* g, int numKrags);
bag_t* GA_get_known_clues(GA_stats_t* g);
bag_t* GA_get_krags_found(GA_stats_t* g);
set_t* GA_get_kragId_to_clue(GA_stats_t* g);
void GA_set_known_clues(GA_stats_t* g, bag_t* bag);

char* GA_get_secret(GA_stats_t* g);
void GA_set_secret(GA_stats_t* g, char* secret);

set_t* GA_get_players( GA_stats_t* g);
char* player_get_name(unsigned int pebbleID, GA_stats_t* g);
char* player_get_team_name(unsigned int pebbleID, GA_stats_t* g);
void set_team_name(char * team_name, GA_stats_t* g);
float player_get_latitude(unsigned int pebbleID, GA_stats_t* g);
float player_get_longitude(unsigned int pebbleID, GA_stats_t* g);

#ifdef NOPEBBLE 
time_t player_get_FA_last_contact_time(unsigned int pebbleID, GA_stats_t* g);
#endif

void player_set_FA_last_contact_time(unsigned int pebbleID, GA_stats_t* g);
  
/**************** update_player ****************/
/*      
 * updates a player's stats including its last contact time 
 *  
 */

//void update_player(unsigned int pebbleID, GA_stats_t* g, char* playername, char* teamname, int latitude, int longitude);
void update_player(unsigned int pebbleID, GA_stats_t* g, char* playername, char* teamname, lat_long_t *lat_long);
void update_GA(GA_stats_t* g, unsigned int gameId, char* team, int numClaimed, int numPlayers);
