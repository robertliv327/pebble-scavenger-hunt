#ifdef NOPEBBLE 

/*                                                                                                                                                                  
 * krags.h     Eleph     May, 2017                                                                                                                                  
 *                                                                                                                                                                  
 * Module storing krag_t struct members and functions                                                                                                               
 *                                                                                                                                                                  
 *                                                                                                                                                                  
 * Functionality:
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


/**************** struct definitions/handlers  ****************/
/*                                                                                                                           
 * define and create or delete  a struct to store krag info
 *
 */
typedef struct krag_info krag_info_t;
krag_info_t* krag_info_new(void);
void krag_info_delete(krag_info_t *k);

/**************** getters for GS struct  ****************/
/*                                                                     
 * return the value of a field in the struct if its not NULL           
 * return NULL equivalent otherwise           
 *
 */
float krag_get_latitude(krag_info_t* k);
float krag_get_longitude(krag_info_t* k);
char* krag_get_clue(krag_info_t* k);
unsigned int krag_get_kragId(krag_info_t* k);
int krag_get_index(krag_info_t* k);



/**************** set up krag data structures ****************/
/* 
 * read_krags_from_file: read krags from a filename and store info in a hashtable with ID key and krag struct item
 * create_bag_of_clues: create a bag of clues from a krag hashtable using iterate helper addtobag 
 *
 */
int read_krags_from_file(hashtable_t* krag_ht, char* filename);
void create_bag_of_clues(bag_t* bag, hashtable_t* krag_ht);
void addtobag(void* arg, const char* key, void* item);



/************** Clue functionality ***********************/
/* 
 * Functions to help get clues and IDs from the bag of clues
 *
 */
void cluefinder(void* arg, const char* key, void* item);
unsigned int get_kragId_from_clue(char* clue, hashtable_t* krag_ht);

// struct defintions to help with method get_krag_from_clue
typedef struct kragstruct_clue kragstruct_clue_t;
kragstruct_clue_t* kragstruct_clue_new(krag_info_t* kragstruct, char* clue);

#endif 
