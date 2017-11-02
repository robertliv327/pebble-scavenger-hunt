/*                                                                                                                                                                     
 * GAstats.c     Eleph     May, 2017                                                                                                                                 
 *                                                                                                                                                                     
 * Defines a struct that contains the statistics for a given guide agent                                                                                                      
 * Has getters for the various fields                                                                                                                                  
 *                                                                                                                                                                     
 * See guideagent.h for more information                                                                                                                                
 *                                                                                                                                                                     
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/set.h"
#include "../libcs50/counters.h"

/**************** struct definitions/initializers  ****************/
/*                                                                                                                                                                              
 * define and create a GA struct to store stats local to GA
 * define and create a player struct to store stats for the player that the GA needs  
 *                            
 */

typedef struct lat_long {
  float latitude;
  float longitude;
} lat_long_t;

lat_long_t* lat_long_new(float lat, float lon){
  lat_long_t* l = malloc(sizeof(lat_long_t));
  if (l!= NULL){
    l->latitude = lat;
    l->longitude = lon;
    return l;
  }
  return NULL;
}

typedef struct clue_bool {
  char* clue;
  bool solved;
} clue_bool_t;

clue_bool_t* clue_bool_new(char* clue, bool solved){
  clue_bool_t* c = malloc(sizeof(clue_bool_t));

  if (c!= NULL){
    c->clue = clue;
    c->solved = solved;
    return c;
  }
  return NULL;
}

void set_bool_to_true(clue_bool_t* c){
  if ( c != NULL){
    c->solved = true;
  }
}

bool get_solved_bool(clue_bool_t* c){
if ( c != NULL){
  return c->solved;
 }
 return false;
}

char* get_unsolved_clue(clue_bool_t* c){
if ( c != NULL){
  return c->clue;
 }
 return NULL;
}

// instead of having a delete for lat long t just remember to free the struct itself!!!
			     
typedef struct GA_stats {
  unsigned int gameId;//?
  char* teamname;//?
  int numClaimed;
  int numKrags;
  int numPlayers;
  bag_t* known_clues;
  set_t* players;
  char* secret;
  bag_t* krags_found;
  set_t* kragId_to_clue;
} GA_stats_t;
  
  
GA_stats_t* GA_stats_new(void){
  GA_stats_t* g = malloc(sizeof(GA_stats_t));

  if (g != NULL){
    g->gameId = 0;
    g->teamname = NULL;
    g->numClaimed = -1;
    g->numKrags = -1;
    g->known_clues = bag_new();
    g->players = set_new();
    g->secret = NULL;
    g->krags_found = bag_new();
    g->kragId_to_clue = set_new();
    return g;
  }
  return NULL;
}

static void simpledelete(void *item){
  if (item){
    free(item);
  }
}

void GA_stats_delete(GA_stats_t* g){
  if (g != NULL){
    bag_delete(g->known_clues, simpledelete);
    set_delete(g->players, simpledelete);
    bag_delete(g->krags_found, simpledelete);
    set_delete(g->kragId_to_clue, simpledelete);
    free(g->teamname);
    free(g->secret);
    free(g);
  }
}

typedef struct player {
  char* name;
  char* team_name;
  float latitude;
  float longitude;
  time_t last_contact_time;
} player_t;
  
void player_delete(player_t* p){
  if (p != NULL){
    free(p->name);
    free(p->team_name);
    free(p);
  }
}


/*********************************/
// method to initialize a player struct to hold player statistics

player_t* player_new(void){
  player_t* p = malloc(sizeof(player_t));

  if (p != NULL){
    p->name = NULL;
    p->team_name = NULL;
    p->latitude = 200.0;
    p->longitude = 200.0;
    p->last_contact_time = -1;
    return p;
  }
  return NULL;
}

 
/**************** getters and setters for GA struct  ****************/
/*                                                                                                                               
 * return the value of a field in the struct if it’s not NULL                                                                     
 * return NULL equivalent otherwise                                                                                              
 OR                                             
 * sets value of a field if the struct is not NULL                        
 *                                                                                                                                                               
 */


unsigned int GA_get_gameId(GA_stats_t* g){
  if (g != NULL){
    return g->gameId;
  }
  return 0;
}

void GA_set_gameID(GA_stats_t* g, unsigned int gameID){
  if (g!= NULL){
    g->gameId = gameID;
  }
}

char* GA_get_teamname(GA_stats_t* g){
  if (g != NULL){
    return g->teamname;
  }
  return NULL;
}

void GA_set_teamname(GA_stats_t* g, char* teamname){
  if (g != NULL){
    g->teamname = teamname;
  }
}

int GA_get_numClaimed(GA_stats_t* g){
  if (g != NULL){
    return g->numClaimed;
  }
  return -1;
}

int GA_get_numPlayers(GA_stats_t* g){
  if (g != NULL){
    return g->numPlayers;
  }
  return -1;
}


void GA_set_numClaimed(GA_stats_t* g, int numClaimed){
  if (g != NULL){
    g->numClaimed = numClaimed;
  }
}

int GA_get_numKrags(GA_stats_t* g){
  if (g != NULL){
    return g->numKrags;
  }
  return -1;
}

void GA_set_numKrags(GA_stats_t* g, int numKrags){
  if (g != NULL){
    g->numKrags = numKrags;
  }
}

bag_t* GA_get_known_clues(GA_stats_t* g){
  if (g != NULL){
    return g->known_clues;
  }
  return NULL;
}

bag_t* GA_get_krags_found(GA_stats_t* g){
  if (g != NULL){
    return g->krags_found;
  }
  return NULL;
}

set_t* GA_get_kragId_to_clue(GA_stats_t* g){
  if (g != NULL){
    return g->kragId_to_clue;
  }
  return NULL;
}

void GA_set_known_clues(GA_stats_t* g, bag_t* bag){
   if (g != NULL){
    g->known_clues = bag;
  }
}

set_t* GA_get_players( GA_stats_t* g){
  if (g != NULL){
    return g->players;
  }
  return NULL;
}


char* GA_get_secret(GA_stats_t* g){
  if (g != NULL){
    return g->secret;
  }
  return NULL;
}

void GA_set_secret(GA_stats_t* g, char* secret){
  if (g != NULL){
    g->secret = secret;
  }
}

char* player_get_name(unsigned int pebbleID, GA_stats_t* g){
  if (g != NULL){
    char pebID[17];
    sprintf(pebID, "%x", pebbleID);
    player_t* p = set_find(g->players, pebID);
    return p->name;
  }
  return NULL;
}

char* player_get_name_from_struct(player_t* p){
  if (p!= NULL){
    return p->name;
  }
  return NULL;
}

char* player_get_team_name(unsigned int pebbleID, GA_stats_t* g){
  if (g != NULL){
    char pebID[17];
    sprintf(pebID, "%x", pebbleID);
    player_t* p = set_find(g->players, pebID);
    return p->team_name;
  }
  return NULL;
}

void set_team_name(char * team_name, GA_stats_t* g){
  if (g != NULL){
    g->teamname = team_name;
  }
}


float player_get_latitude(unsigned int pebbleID, GA_stats_t* g){
  if (g != NULL){
    char pebID[17];
    sprintf(pebID, "%x", pebbleID);
    player_t* p = set_find(g->players, pebID);
    return p->latitude;
  }
  return 200.0;
}

float player_get_longitude(unsigned int pebbleID, GA_stats_t* g){
  if (g != NULL){
    char pebID[17];
    sprintf(pebID, "%x", pebbleID);
    player_t* p = set_find(g->players, pebID);
    return p->longitude;
  }
  return 200.0;
}

time_t player_get_FA_last_contact_time(unsigned int pebbleID, GA_stats_t* g){
  if (g != NULL){
    char pebID[17];
    sprintf(pebID, "%x", pebbleID);
    player_t* p = set_find(g->players, pebID);
    return p->last_contact_time;
  }
  return -1;
}

void player_set_FA_last_contact_time(unsigned int pebbleID, GA_stats_t* g){
  if (g != NULL){
    char pebID[17];
    sprintf(pebID, "%x", pebbleID);
    player_t* p = set_find(g->players, pebID);
    /*    time_t curr;
    time (&curr);
    p->last_contact_time = curr; 
    */
    p->last_contact_time = 20;
  }
}


/**************** update_GA ****************/
/*                                                                                                                                                           
 * updates a GA's stats 
 *                                                                                                                                                           
 */
void update_GA(GA_stats_t* g, unsigned int gameId, char* team, int numClaimed, int numPlayers){
  if (g != NULL){
    g->gameId = gameId;
    g->teamname = team;
    g->numClaimed = numClaimed;
    g->numPlayers = numPlayers;
  }
}
  

/**************** update_player ****************/
/*      
 * updates a player's stats including its last contact time 
 *  
 */
void update_player(unsigned int pebbleID, GA_stats_t* g, char* playername, char* teamname, lat_long_t *lat_long){
   if (g != NULL){
     if( g->players != NULL) {
     char pebID[17];
    sprintf(pebID, "%x", pebbleID);
    player_t* p = set_find(g->players, pebID);
    
    if (p != NULL){
      p->latitude = lat_long->latitude;
      p->longitude = lat_long->longitude;
      p->last_contact_time = 20;  /*      time_t curr;
          time (&curr);
          p->last_contact_time = curr;
	  // was prev setting it to 20
	  */
    }

    else {
      player_t* p2 = player_new();
      p2->name = playername;
      p2->team_name = teamname;
      p2->latitude = lat_long->latitude;
      p2->longitude = lat_long->longitude;
      p2->last_contact_time = 20;
      //time_t curr;
      //    time (&curr);
      //   p2->last_contact_time = curr;
	  //p2->last_contact_time = 20;
      char* idtext = malloc(9);
      sprintf(idtext, "%x", pebbleID);
      set_insert(g->players, idtext, p2); 
    }
    
     }
   }
}
