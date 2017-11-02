#ifdef NOPEBBLE 

/* 
 * set.c - CS50 'set' module
 *
 * see set.h for more information.
 *
 * Robert Livaudais, April 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "set.h"
#include "memory.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct setnode {
  void *item;		      // pointer to item for this node
  char *key;				// the node's key
  struct setnode *next;	      // link to next node
} setnode_t;

/**************** global types ****************/
typedef struct set {
  struct setnode *head;	      // head of the list of items in set
} set_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see set.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static setnode_t *setnode_new(const char *key, void *item);

/**************** set_new() ****************/
set_t *
set_new(void)
{
	set_t *set = count_malloc(sizeof(set_t));

	if (set == NULL) {
		return NULL; // error allocating set
	} else {
		// initialize contents of set structure
		set->head = NULL;
		return set;
	}
}

/**************** set_insert() ****************/
bool 
set_insert(set_t *set, const char *key, void *item)
{
	// if any parameters are null, return false
	if (set == NULL || key == NULL || item == NULL){
		return false;
	}
	// make sure key doesn't exist in set already
	for (setnode_t *node = set->head; node != NULL; node = node->next) {
		if (strcmp(key, node->key) == 0){
			// if key already exists, return false
			return false;
		}
	}
	// if we get to this step, key doesn't exist in set, so create new node
	setnode_t *new = setnode_new(key, item);	// setnode_new() will handle copying key...
	if (new == NULL)
		return false;
	else {
		new->next = set->head;
    	set->head = new;
	}
	return true;	// successfully inserted, so return true
}

/**************** setnode_new() ****************/
/* Allocate and initialize a setnode */
static setnode_t *
setnode_new(const char *key, void *item)
{
	setnode_t *node = count_malloc(sizeof(setnode_t));

	if (node == NULL) {
		// error allocating memory for node; return error
		return NULL;
	} else {
		node->key = count_malloc(strlen(key)+1);
    	if (node->key == NULL) {
      		// error allocating memory for key; 
      		// cleanup and return error
      		count_free(node);
      		return NULL;
	    } else {
	      	strcpy(node->key, key);
			node->item = item;
			node->next = NULL;
			return node;
		}
	}
}

/**************** set_find() ****************/
void *
set_find(set_t *set, const char *key)
{	
	// if set or key is null, return null
	if (set == NULL || key == NULL){
		return NULL;
	}
	// search for key
	for (setnode_t *node = set->head; node != NULL; node = node->next) {
		if (strcmp(key, node->key) == 0){
			// if key is found, return item...
			return node->item;
		}
	}
	return NULL;	// key not found
}

/**************** set_print() ****************/
void 
set_print(set_t *set, FILE *fp, void (*itemprint)(FILE *fp, const char *key, void *item) )
{
	if (fp != NULL) {
    	if (set != NULL) {
      		fputs("{ ", fp);	// formatting...
      		for (setnode_t *node = set->head; node != NULL; node = node->next) {
      			// print this node
      			if (itemprint != NULL) {  // print the node's item 
      	  			(*itemprint)(fp, node->key, node->item);
      	  			fputc(',', fp);
      			}
      		}
      		fputs(" }", fp);	// formatting...
    	} else {
      		fputs("(null)", fp);	// if set is null
    	}
  	}
}

/**************** set_iterate() ****************/
void 
set_iterate(set_t *set, void *arg, void (*itemfunc)(void *arg, const char *key, void *item) )
{
	if (set != NULL && itemfunc != NULL) {
    	// call itemfunc with arg, on each item
    	for (setnode_t *node = set->head; node != NULL; node = node->next) {
      		(*itemfunc)(arg, node->key, node->item); 
    	}
  	}
}

/**************** set_delete() ****************/
void 
set_delete(set_t *set, void (*itemdelete)(void *item) )
{
	if (set != NULL){
		// call itemdelete on each item
		for (setnode_t *node = set->head; node != NULL; ) {
      		if (itemdelete != NULL) {
      			(*itemdelete)(node->item); 
      		}
      		setnode_t *next = node->next;	    // remember what comes next
      		count_free(node->key);			// free the key
      		count_free(node);			    // free the node
      		node = next;			    // and move on to next
    	}
	}
	count_free(set);
}

#endif 
