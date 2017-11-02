/*                                                                                                                                                           
 * gamestats.c     Eleph     May, 2017                                                                                                                        
 *                                                                                                                                                            
 * Defines a struct that contains the statistics for a given game
 * Has getters for the various fields                                                                                                                         
 *                                                                                                                                                            
 * See gamestats.h for more information                                                                                                                         
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
#include "../common/krags.h"


// static functions and function headers needed for iterate helper methods
static void clue_delete(void * arg);
static void team_delete(void *arg);
static void krag_delete(void *arg);
static void ID_delete(void *arg);
static void player_delete(void *arg);
static void itemcount(void *arg, const char* key, void *item);
void team_print(void* arg, const char* key, void* item);
void player_print(void* arg, const char* key, void* item);
static void copybag(void *arg, void *item);


//              struct definitions and initializor methods                                                                                                  
/***********************************************************************/


/*********************************/
// struct definition for game stats

typedef struct game_stats {
  unsigned int gameId; //the game Id
  int num_krags; // the total number of krags in the game
  int num_agents; // number of agents in the game
  int num_teams; // number of teams in the game
  time_t time_since_start; // time elapsed since start
  set_t* teams; // a set: keys are team names, and team_t structs 
  bag_t* master_bag_of_clues; // a bag of clues for giving teams clues
  hashtable_t* krag_ht;  // a hashtable: keys are kragIDs, items are krag_t structs
  char* secret; // the secret
  hashtable_t* ID_ht; // a hashtable: keys are player IDs (pebbleId or GuideId), items are team_and_name_t structs
} game_stats_t;

/*********************************/
// method to initialize game stats struct

game_stats_t* game_stats_new(void){
  game_stats_t* g = malloc(sizeof(game_stats_t));
  // if there is an issue mallocing
  if (g == NULL){
    return NULL;
  }

  g->gameId = 0;
  g->num_krags = -1;
  g->num_agents = 0;
  g->num_teams = -1;
  // save the time at the very beginnining of the game to use to calculate elapsed time
  time_t start;
  time (&start);
  g->time_since_start = start;
  g->teams = set_new();
  g->master_bag_of_clues = NULL;
  g->krag_ht = NULL;
  g->secret = NULL;
  g->ID_ht = hashtable_new(10);
  return g;
}

/*********************************/
// struct definition for team stats 

typedef struct team {
  char* name;
  unsigned int guideId;
  char* guide_name;
  time_t last_contact;
  int num_claimed_krags;
  char** claimed_krags;
  char* secret;
  set_t* players; // key pebbleID item player_t
  bag_t* bag_of_clues;
} team_t;

/*********************************/
// method to initialize a team struct to hold team stats

team_t* team_new(void){
  team_t* t = malloc(sizeof(team_t));

  if (t == NULL){
    return NULL;
  }
  t->name = NULL;
  t->guideId = 0;
  t->guide_name = NULL;
  t->last_contact = -1;
  t->claimed_krags = NULL;
  t->num_claimed_krags = 0;
  t->secret = NULL; 
  t->players = set_new();
  t->bag_of_clues = bag_new();
  return t;
}

/*********************************/
// struct definition for player (FA) stats

typedef struct player {
  char* name;
  char* team_name;
  float latitude;
  float longitude;
  time_t last_contact;
} player_t;

/*********************************/
// method to initialize a player struct to hold player statistics

player_t* player_new(void){
  player_t* p = malloc(sizeof(player_t));
  if (p == NULL){
    return NULL;
  }
  p->name = NULL;
  p->team_name = NULL;
  p->latitude = 200;
  p->longitude = 200;
  p->last_contact = -1;
  return p;
}

/*******************************/
// struct containing names and teams

typedef struct team_and_name {
  char* team;
  char* name;
} team_and_name_t;

team_and_name_t* team_and_name_new(char *team, char *name) {
  team_and_name_t *tn  = malloc(sizeof(team_and_name_t));
  tn->team = team;
  tn->name = name;
  return tn;
}

void team_and_name_delete(team_and_name_t *tn) {
  free(tn);
}

/********************************/
// method to delete the game stats struct                                                                                                                                  

void game_stats_delete(game_stats_t *g) {
  if (g != NULL) {
    bag_delete(g->master_bag_of_clues, clue_delete);
    hashtable_delete(g->krag_ht, krag_delete);
    free(g->secret);
    hashtable_delete(g->ID_ht, ID_delete);
    set_delete(g->teams, team_delete);
    free(g);
  }
}

/********************************/
// method to delete a clue

static void clue_delete(void * arg) {
  free(arg);
}

/********************************/
// method to delete a krag

static void krag_delete(void *arg) {
  krag_info_t *k = arg;
  krag_info_delete(k);
}

/********************************/
// method to delete an ID

static void ID_delete(void *arg) {
  team_and_name_t *tn = arg;
  team_and_name_delete(tn);
}

/********************************/
// method to delete a team

static void team_delete(void *arg) {
  team_t *team = arg;
  free(team->name);
  free(team->guide_name);
  for (int i = 0; i < team->num_claimed_krags; i++) {
    free(team->claimed_krags[i]);
  }
  free(team->claimed_krags);
  free(team->secret);
  set_delete(team->players, player_delete);
  bag_delete(team->bag_of_clues, clue_delete);
  free(arg);
}

/********************************/
// method to delete a player

static void player_delete(void *arg) {
  player_t * player = arg;
  free(player->name);
  free(player->team_name);
  free(arg);
}


//               getters/setters for variables in GAME STATS STRUCT
/***********************************************************************/

/*********************************/
// method to get the ID of a game

unsigned int stats_get_gameID(game_stats_t* s){
  if (s != NULL){
    return s->gameId;
  }
  return 0;
}

/*********************************/
// method to set the ID of a game

void stats_set_gameID(game_stats_t* s, unsigned int gameID){
  s->gameId = gameID;
}

/*********************************/
// method to get the number of krags in the game

int stats_get_numkrags(game_stats_t* s){
  if (s!= NULL){
    return s->num_krags;
  }
  return -1;
}

/*********************************/
// method to get the number of agents in the game

int stats_get_numagents(game_stats_t* s){
  if (s!= NULL){
    return s->num_agents;
  }
  return 0;
}

/*********************************/
// method to get the number of teams in the game

int stats_get_numteams(game_stats_t* s){
  if (s!= NULL){
    return s->num_teams;
  }
  return -1;
}

/*********************************/
// method to get the time since the game started

int stats_get_timesincestart(game_stats_t* s){
  if (s!= NULL){
    time_t end;
    time (&end);
    int dif = (int) difftime(end, s->time_since_start);
    // s->time_since_start = dif;
    return dif; 
  }
  return -1;
}

/*********************************/
// method to get the full secret

 char* stats_get_secret(game_stats_t* s){
  if (s != NULL){
    return s->secret;
  }
  return NULL;
}

/*********************************/
// method to get the bag of all clues

bag_t* stats_get_master_bag(game_stats_t* s){
  if (s != NULL){
    return s->master_bag_of_clues;
  }
  return NULL;
}

/*********************************/
// method to get the krag hashtable

hashtable_t* stats_get_krag_ht(game_stats_t* s){
  if (s != NULL){
    return s->krag_ht;
  }
  return NULL;
}


/*********************************/
// method to get a team's stats struct                                                                                                                                           

team_t* stats_get_team(char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
    return t;
  }
  }
  return NULL;
}


/********************************/
// method to return a game's set of team structs

set_t* stats_get_teamset(game_stats_t* s){
  if (s != NULL) {
    return s->teams;
  }
  return NULL;
}

/*********************************/
// method to set the krag hashtable                                                                                                     

void set_main_hashtable(game_stats_t *gs, hashtable_t *ht){
  gs->krag_ht=ht;

}


/*********************************/
// method to set the bag of clues                                                                                                       
void set_main_bag_of_clues(game_stats_t * gs, bag_t* bag){
  gs->master_bag_of_clues = bag;

}

/*********************************/
// method to set the secret                                                                                                             
void set_main_secret(game_stats_t *gs, char* secret)
{
  gs->secret= secret;
}

/*********************************/
// method to set the secret                                                                                                             
void set_gameId_gs(game_stats_t *gs, unsigned int game_id){

  gs->gameId = game_id;
}

/*********************************/
// method to set the secret                                                                                                             
void set_num_krags_gs(game_stats_t *gs, int num_krags){
  gs->num_krags = num_krags;
}


//              getters/setters for variables in TEAM STATS STRUCT                                                         
/***********************************************************************/

/*********************************/
// method to get a team's name
char * team_get_name(team_t *team) {
  if (team != NULL) {
    return team->name;
  }
  return NULL;
}

/*********************************/
// method to get a team's guideId

unsigned int team_get_guideId(char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
    return t->guideId;
  }
  }
  return 0;
}

/*********************************/
// method to get a team's guide name

char* team_get_guide_name(char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
    return t->guide_name;
  }
  }
  return NULL;
}

/*********************************/
// method to get a team's GA's last contact time

time_t team_get_GA_last_contact(char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      return t->last_contact;
  }
  }
  return -1;
}

/*********************************/
// method to get a team's array of claimed krags

char** team_get_claimed_krags_array(char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      return t->claimed_krags;
  }
  }
  return NULL;
}

/*********************************/
// method to get a team's number of claimed krags

int team_get_num_claimed_krags(char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      return t->num_claimed_krags;
    }
  }
  return -1;
}

/*********************************/
// method to get a team's secret

char* team_get_secret(char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      return t->secret;
    }
  }
  return NULL;
}

/*********************************/
// method to set a teams secret

void team_set_secret(char* teamname, game_stats_t* s, char* secret){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      t->secret = secret;
    }
  }
}
/*********************************/
// method to get a team's bag of clues

bag_t* team_get_bag_of_clues(char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      return t->bag_of_clues;
    }
  }
  return NULL;
}

/*********************************/
// method to get a player's stats struct

player_t* team_get_player(unsigned int pebbleID, char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      char pebID[17];
      sprintf(pebID, "%x", pebbleID);
      player_t* p = set_find(t->players, pebID);
      if (p != NULL){
	return p;
      }
    }
  }
  return NULL;
}

/*********************************/
// method to get a set of players

set_t* team_get_players(char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      return t->players;
    }
  }
  return NULL;
}

/*********************************/
// method to increment the number of krags a team has claimed

void team_increment_num_krags(char* teamname, game_stats_t* s){
   if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      t->num_claimed_krags += 1;
    }
   }
}

/**********************************/
// method to get the number of players in a team struct

int get_num_players(char * teamname, game_stats_t *s) {
  if (s->teams != NULL){
    set_t *players = team_get_players(teamname, s);
    if (players != NULL) {
      int nitems = 0;
      set_iterate(players, &nitems, itemcount);
      return nitems;
    }
  }
  return -1;
}

/**********************************/
// helper for iterate in get_num_players

static void
itemcount(void *arg, const char* key, void *item)
{
  int *nitems = arg;

  if (nitems != NULL && key != NULL && item != NULL)
    (*nitems)++;
}


//                       getters for variables in PLAYER STATS STRUCT                                                                 
/***********************************************************************/


/*********************************/
// method to get a player's name

char* player_get_name(unsigned int pebbleID, char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      char pebID[17];
      sprintf(pebID, "%x", pebbleID);
      player_t* p = set_find(t->players, pebID);
      if (p != NULL){
        return p->name;
      }
    }
  }
  return NULL;
}

/*********************************/
// method to get a player's team's name

char* player_get_team_name(player_t *player) {
  return player->team_name;
}

/*********************************/
// method to get a player's latitude

float player_get_latitude(unsigned int pebbleID, char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      char pebID[17];
      sprintf(pebID, "%x", pebbleID);
      player_t* p = set_find(t->players, pebID);
      if (p != NULL){
        return p->latitude;
      }
    }
  }
  return 200.0;
}

/*********************************/
// method to get a player's longitude

float player_get_longitude(unsigned int pebbleID, char* teamname, game_stats_t* s){
  if (s->teams != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      char pebID[17];
      sprintf(pebID, "%x", pebbleID);
      player_t* p = set_find(t->players, pebID);
      if (p != NULL){
        return p->longitude;
      }
    }
  }
  return 200;
}

/*********************************/
// method to get the time at which a player last contacted the server

time_t player_get_last_contact(int pebbleID, char* teamname, game_stats_t* s){
  if (s->teams != NULL){
     team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      char pebID[17];
      sprintf(pebID, "%x", pebbleID);
      player_t* p = set_find(t->players, pebID);
      if (p != NULL){
	return p->last_contact;
      }
    }
   }
  return -1;
}

/*********************************/
// method to set last contact time for a player

void player_set_last_contact(int pebbleID, char* teamname, game_stats_t* s){
  if (s->teams != NULL){
     team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      char pebID[17];
      sprintf(pebID, "%x", pebbleID);
      player_t* p = set_find(t->players, pebID);
      if (p != NULL){
	time_t curr;
        time (&curr);
        p->last_contact = curr;
      }
    }
  }
}
	


//                methods to add and update players/teams
/***********************************************************************/


/*********************************/
// method to add a team to a game

void register_team(char* teamname, game_stats_t* s){
  if (s->teams != NULL && set_find(s->teams, teamname) == NULL){
    s->num_agents += 1;
    team_t* t = team_new();
    t->name = teamname;
    char* fullsecret = s->secret;
    int fullsecretlength = strlen(fullsecret)+1;

    char* secret = malloc(fullsecretlength);
    memset(secret, '\0', fullsecretlength);
    int i;
    for (i = 0; i < fullsecretlength - 1; i++){
      secret[i] = '_';
    }

    secret[i+1] = '\0';
    t->secret = secret;    
    int numkrags = s->num_krags;
    char** claimed_krags;
    claimed_krags = malloc(numkrags * sizeof(char*) * 5);
    t->claimed_krags = claimed_krags;

    bag_iterate(s->master_bag_of_clues, t->bag_of_clues, copybag);
    set_insert(s->teams, teamname, t);
  }
}


/*********************************/
// helper iterate method used in register team to copy master clue bag to a team's bag

static void 
copybag(void *arg, void *item)
{
  bag_t* teamsbag = arg;
  char* item2 = item;
  char* clue = malloc(sizeof(char)*(strlen(item2)+1));
  strcpy(clue, item2);
  bag_insert(teamsbag, clue);
}

/*********************************/
// method to update a player's stats

void update_player(unsigned int pebbleID, char* teamname, char* playername, float latitude, float longitude, game_stats_t* s){
  if (s != NULL){
    if (s->teams != NULL){
      team_t* t = set_find(s->teams, teamname);
      if (t != NULL){
	char pebID[17];
	sprintf(pebID, "%x", pebbleID);
	player_t* p = set_find(t->players, pebID);
	if (p != NULL){
	  p->latitude = latitude;
	  p->longitude = longitude;	
	 time_t curr;
        time (&curr);
        p->last_contact = curr;
	}
	else {
	  player_t* p = player_new();
	  p->name = playername;
	  p->team_name = teamname;
	  p->latitude = latitude;
	  p->longitude = longitude;
	  time_t curr;
	  time (&curr);
	  p->last_contact = curr;
	  char pebID[17];
	  sprintf(pebID, "%x", pebbleID);
	  set_insert(t->players, pebID, p);
	  team_and_name_t *tn = team_and_name_new(teamname, playername);
	  char *Idtext = malloc(9);
	  sprintf(Idtext, "%x", pebbleID);
	  hashtable_insert(s->ID_ht, Idtext, tn);
	}     
      }
    }
  }
}

/*********************************/
// method to add a guide agent to a team

void register_guide(unsigned int guideId, char* guide_name, char* teamname, game_stats_t* s){
  if (s != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      t->guideId = guideId;
      t->guide_name = guide_name;
      time_t curr;
      time (&curr);
      t->last_contact = curr;
      team_and_name_t *tn = team_and_name_new(teamname, guide_name);
      char *Idtext = malloc(9);
      sprintf(Idtext, "%x", guideId);
      hashtable_insert(s->ID_ht, Idtext, tn);
    }
  }   
}

/*********************************/
// method to update a guide agent's stats in a team

void update_guide(char* teamname, game_stats_t* s){
  team_t* t = set_find(s->teams, teamname);
  if (s->teams != NULL && t != NULL){
    time_t curr;
    time (&curr);
    t->last_contact = curr;
  }
}

/*********************************/
// struct to help with set iterate in method to check if a player is on a team

typedef struct bool_and_name {
  bool x;
  char* playername;
} bool_and_name_t;

/*********************************/
// initializor for bool_and_name

static bool_and_name_t* bool_and_name_new(bool x, char* playername){
  bool_and_name_t* b = malloc(sizeof(bool_and_name_t));
  if (b!= NULL){
    b->x = x;
    b->playername = playername;
    return b;
  }
  return NULL;
}

/*********************************/
// iterate helper to compare player names

static void checkplayername(void* arg, const char* key, void* item){
  bool_and_name_t* b = arg;
  player_t* playeritem = item;
  if (strcmp(playeritem->name, b->playername) == 0){
    b->x = true;
  }
}

/*********************************/
// method to check if a player is on a team

bool is_player_on_team(char* playername, char* teamname, game_stats_t* s){
  bool x = false;
  if (s != NULL){
    team_t* t = set_find(s->teams, teamname);
    if (t != NULL){
      set_t* playerset = t->players;
      bool_and_name_t* b = bool_and_name_new(x, playername);
      set_iterate(playerset, b, checkplayername);
      free(b); // do we need to free b's fields too.. 
    }
  }
  return x;
}

/*********************************/
// Checks the ID of a player against its name
int player_check_Id(game_stats_t *gs, unsigned int Id, char* team, char* name) {
  char Idtext[9];
  sprintf(Idtext, "%x", Id);
  team_and_name_t *tn = hashtable_find(gs->ID_ht, Idtext);
  if (tn != NULL) {
    if ((strcmp(team, tn->team) != 0) && (strcmp(name, tn->name) != 0)){
      return 3;
    } else if (strcmp(team, tn->team) != 0) {
      return 1;
    } else if (strcmp(name, tn->name) != 0) {
      return 2;
    } 
  }
  return 0;
}


//                methods to print game stats                                                                             
/***********************************************************************/

/*******************************/
// method to print the game stats
void game_print(game_stats_t * gs)
{
  unsigned int gID= stats_get_gameID( gs);
  int nK= stats_get_numkrags(gs);
  int nA= stats_get_numagents(gs);
  int nT= stats_get_numteams(gs);
  int time= stats_get_timesincestart(gs);
  char* secret= stats_get_secret(gs);
  fprintf(stdout, "\n\n ******************GAME STATUS*******************\n");
  fprintf(stdout, "-gameID= %u\n -numKrags= %d\n -numAgents= %d\n -numTeams= %d\n -timeSinceStart= %d\n -secret= %s\n", gID, nK, nA, nT, time, secret);
  set_iterate(gs->teams, &gs->num_krags, team_print);
  
}

/*********************************/
// method to print team stats

void team_print(void* arg, const char* key, void* item) {
  team_t *team = item;
  int *numKrags = arg;
  printf("***********/n -TEAM: %s\n", key);
  printf("---Guide ID | Guide name: %x | %s\n", team->guideId, team->guide_name);
  printf("---Time since last contact: %li\n", team->last_contact);
  printf("---Claimed %i/%i krags\n", team->num_claimed_krags, *numKrags);

   printf("---Claimed Krags: ");
   if (team->claimed_krags != NULL) {
     for (int i=0; i < team->num_claimed_krags; i++) {
       printf("%s ", team->claimed_krags[i]);
     }
   }
   printf("\n");
   printf("---Secret: %s\n", team->secret);
   printf("---PLAYERS: \n");
   set_iterate(team->players, NULL, player_print);
}

/*********************************/
// method to print player stats

void player_print(void* arg, const char* key, void* item) {
  player_t* player = item;
  printf("---PLAYER: %s\n", key);
  printf("-----Name | team: %s %s\n", player->name, player->team_name);
  printf("-----Location: %f, %f\n", player->latitude, player->longitude);
  printf("-----Last contact time: %li\n", player->last_contact);
}

