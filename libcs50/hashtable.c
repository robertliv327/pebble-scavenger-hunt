#ifdef NOPEBBLE 

/* 
 * hashtable.c - CS50 'hashtable' module
 *
 * see hashtable.h for more information.
 *
 * Robert Livaudais, April 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hashtable.h"
#include "memory.h"
#include "set.h"
#include "jhash.h"

/**************** file-local global variables ****************/
/* none */

/**************** global types ****************/
typedef struct ht {
  struct set **array;	      // array of pointers to sets
  int num_slots;
} hashtable_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see hashtable.h for comments about exported functions */

/**************** hashtable_new() ****************/
hashtable_t *
hashtable_new(const int num_slots)
{
	// return NULL if invalid num_slots
	if (num_slots < 1){
		return NULL;
	}

	hashtable_t *ht = count_malloc(sizeof(hashtable_t));

	if (ht == NULL) {
		return NULL; // error allocating hashtable
	} else {
		// initialize contents of hashtable structure
		ht->array = calloc(num_slots, sizeof(struct set*));
		// if calloc encountered error and didn't allocate memory, return NULL for error
		if (ht->array == NULL){
			return NULL;
		}
		ht->num_slots = num_slots;
		return ht;
	}
}

/**************** hashtable_insert() ****************/
bool 
hashtable_insert(hashtable_t *ht, const char *key, void *item)
{
	// if any parameters are null, return false
	if (ht == NULL || key == NULL || item == NULL){
		return false;
	}
	// perform hash function
	unsigned long bucket = JenkinsHash(key, ht->num_slots);

	// if we haven't used this bucket yet, we need to instantiate the set
	if (ht->array[bucket] == NULL){
		ht->array[bucket] = set_new();
	}
	// will be true if key doesn't exist in hashtable and successfully inserted ; false otherwise
	return set_insert(ht->array[bucket], key, item);	// key is copied for use by hashtable within set...
}

/**************** hashtable_find() ****************/
void *
hashtable_find(hashtable_t *ht, const char *key)
{
	// if ht or key is null, return null
	if (ht == NULL || key == NULL){
		return NULL;
	}
	// search for key
	unsigned long bucket = JenkinsHash(key, ht->num_slots);		// perform hash function

	// will return item if key is found in set, NULL otherwise
	return set_find(ht->array[bucket], key);
}

/**************** hashtable_print() ****************/
void 
hashtable_print(hashtable_t *ht, FILE *fp, void (*itemprint)(FILE *fp, const char *key, void *item))
{
	if (fp != NULL) {
    	if (ht != NULL) {
      		fputs("{\n", fp);	// formatting...
      		// loop through array of set pointers
      		for (int i = 0; i < ht->num_slots; i++){
      			// print the set using itemprint
      			if (ht->array[i] != NULL){
      				fprintf(fp, "Bucket %d: ", i);
      				set_print(ht->array[i], fp, itemprint);
      				fputc('\n', fp);
      			}
      		}
      		fputs("}\n", fp);	// formatting...
    	} else {
      		fputs("(null)", fp);	// if set is null
    	}
  	}
}

/**************** hashtable_iterate() ****************/
void 
hashtable_iterate(hashtable_t *ht, void *arg, void (*itemfunc)(void *arg, const char *key, void *item) )
{
	if (ht != NULL && itemfunc != NULL) {
  		for (int i = 0; i < ht->num_slots; i++){
  			// if set isn't null, call itemiterate on it
  			if (ht->array[i] != NULL){
  				// call set_iterate, passing in itemiterate
  				set_iterate(ht->array[i], arg, itemfunc);
  			}
  		}
	}
}

/**************** hashtable_delete() ****************/
void 
hashtable_delete(hashtable_t *ht, void (*itemdelete)(void *item) )
{
	if (ht != NULL){
		for (int i = 0; i < ht->num_slots; i++){
			// delete each set in array
			set_delete(ht->array[i], itemdelete);
		}
		// free the array
		count_free(ht->array);
	}
	// free the ht
	count_free(ht);
}

#endif 
