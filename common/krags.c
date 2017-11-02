#ifdef NOPEBBLE 

/*                                                                                                                                                           
 * krags.c     Eleph     May, 2017                                                                                                                         
 *                                                                                                                                                           
 * Module storing krag_t struct members and functions
 * 
 *                                                                                                                                                           
 * See krags.h for more information                                                                                                                        
 *                                                                                                                                                           
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/set.h"
#include "../libcs50/counters.h"
#include "../libcs50/file.h"

/*
Puts krags in hashtable as keys where items are struct holding location and clue
(reason for this is so that we can get random clues and also have krags be linked to clues… if a team finds a krag that they didn’t have the clue for, we can use the hashtable to see if the clue has been given out - if it hasn’t find it in bag and deactivate it)
*/

/****************************************/
// a struct that stores info for one single krag, including a clue                                                                                              

typedef struct krag_info {
  float latitude;
  float longitude;
  char* clue;
  unsigned int kragId;
  int index; // read them in and assign them each a number
} krag_info_t;



// Local Function Declarations
static krag_info_t* create_krag_struct_from_line(char* line, int i);
static void simpleiterate(void *arg, const char *key, void *item);
/****************************************/
// method to initialize the krag struct                                                                                                                          
krag_info_t* krag_info_new(float lat, float lon, char* clue, unsigned int ID, int index){
  krag_info_t* k = malloc(sizeof(krag_info_t));
  if (k != NULL){
    k->latitude = lat;
    k->longitude = lon;
    k->clue = clue;
    k->kragId = ID;
    k->index = index;
    return k;
  }
  return NULL;
}


// method to delete the krag struct
void krag_info_delete(krag_info_t *k) {
  if (k != NULL){
    free(k->clue);
    free(k);
  }
}

//                 getters for krag struct
/*******************************************************************/

/****************************************/
// method to get a krag's latitude

float krag_get_latitude(krag_info_t* k){
  if (k != NULL){
    return k->latitude;
  }
  return 200.0;
}

/****************************************/
// method to get a krag's longitude                                                                                                                                  
float krag_get_longitude(krag_info_t* k){
  if (k != NULL){
    return k->longitude;
  }
  return 200.0;
}

/****************************************/
// method to get a krag's clue

char* krag_get_clue(krag_info_t* k){
  if (k != NULL){
    return k->clue;
  }
  return NULL;
}

/****************************************/
// method to get a krag's kragId

unsigned int krag_get_kragId(krag_info_t* k){
  if (k != NULL){
    return k->kragId;
  }
  return 0;
}

/****************************************/
// method to get a krag's index

int krag_get_index(krag_info_t* k){
  if (k != NULL){
    return k->index;
  }
  return -1;
}


/****************************************/
 // method to create a hashtable of krag structs out of a file containing a list of krags
// key of hashtable is kragId, item is a krag_info_t* struct

// latitude=|longitude=|kragId=|clue=

int read_krags_from_file(hashtable_t* krag_ht, char* filename){
  FILE* fp = fopen(filename, "r");
  //
  char* line = readlinep(fp);
  int i = 0;
  while (line != NULL){
    krag_info_t* krag_struct = create_krag_struct_from_line(line, i);
    char kragid[17];
    sprintf(kragid, "%x", krag_struct->kragId);
    hashtable_insert(krag_ht, kragid, krag_struct);
    i++;
    free(line);
    line = readlinep(fp);    
  }
  int nitems = 0;
  hashtable_iterate(krag_ht, &nitems, simpleiterate);
  fclose(fp);
  return nitems;
}

static void simpleiterate(void *arg, const char *key, void *item){
  int *nitems = arg;
  if (nitems != NULL && key != NULL && item != NULL)
    (*nitems)++;
}

/****************************************/
// Static helper method for read_krags_from_file
// Reads the krag from a line and stores it in a krag_info_t struct
static krag_info_t* create_krag_struct_from_line(char* line, int i){
  int index = i;
  krag_info_t *k = krag_info_new(200, 200, NULL, 0, i);
  int line_length = strlen(line);
  char *line_copy = malloc(line_length+1);
  strcpy(line_copy, line);
  for (char * rest = line_copy; *rest != '\0';  ) {
    char* field;
    char* field_ID = rest;
    for (rest = field_ID; *rest != '\0' && *rest != '='; rest++) {}
    if (*rest != '\0') {
      *rest++ = '\0';
    }
    field = rest;
    for (rest = field; *rest != '\0' && *rest != '|'; rest++) {}
    if (*rest != '\0') {
      *rest++ = '\0';
    }
    if (strcmp(field_ID, "latitude") == 0) {
      k->latitude = atof(field);
    } else if (strcmp(field_ID, "longitude") == 0) {
      k->longitude = atof(field);
    } else if (strcmp(field_ID, "kragId") == 0) {
      k->kragId = strtoul(field, NULL, 16);
    } else if (strcmp(field_ID, "clue") == 0) {
      k->clue = malloc(strlen(field)+1);
      strcpy(k->clue, field);
    }
  }   
  free(line_copy);
  return k;


}

/****************************************/
// hashtable iterate helper method to add each clue from the krag item in the hashtable into the bag of clues 

void addtobag(void* arg, const char* key, void* item){
  bag_t* bag = arg;
  krag_info_t* k = item;
  if (bag != NULL){
  bag_insert(bag, k->clue);
  }
}

/****************************************/
// method to create a (master) bag storing clues from a hashtable of krags                                                                                  
void create_bag_of_clues(bag_t* bag, hashtable_t* krag_ht){
  hashtable_iterate(krag_ht, bag, addtobag);
 }


/****************************************/
// struct definition to store a krag struct AND a clue, to be used in hashtable iterate

typedef struct kragstruct_clue {
  char* clue;
  unsigned int id;
} kragstruct_clue_t;

kragstruct_clue_t* kragstruct_clue_new(char* clue, unsigned int id){
  kragstruct_clue_t* k = malloc(sizeof(kragstruct_clue_t));
  if (k != NULL){
    k->clue = clue;
    k->id = id;
    return k;
  }
  return NULL;
}

/****************************************/
// hashtable iterate helper method to check what krag a clue is linked to

void cluefinder(void* arg, const char* key, void* item){
  kragstruct_clue_t* s = arg;
  krag_info_t* k = item;

  // if the clue from the hashtable item is equal to the clue from the kragstruct which stores the clue that is a parameter.....
  if (strcmp(krag_get_clue(k), s->clue) == 0){
    s->id = krag_get_kragId(k);
  }
}

// go thru each krag in hashtable, if clue == clue, return krag info struct

unsigned int get_kragId_from_clue(char* clue, hashtable_t* krag_ht){
  kragstruct_clue_t* s = kragstruct_clue_new(clue, 0);
  hashtable_iterate(krag_ht, s, cluefinder);
  return s->id;
}


#endif 

