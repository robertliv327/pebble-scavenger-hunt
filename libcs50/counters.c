#ifdef NOPEBBLE 

/* 
 * counters.c - CS50 'counters' module
 *
 * see counters.h for more information.
 *
 * Robert Livaudais, April 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "counters.h"
#include "memory.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct ctrsnode {
  int key;		      // the node's key
  int count;				// the key's count value
  struct ctrsnode *next;	      // link to next node
} ctrsnode_t;

/**************** global types ****************/
typedef struct ctrs {
  struct ctrsnode *head;	      // head of the list of counters
} counters_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static ctrsnode_t *ctrsnode_new(const int key);

/**************** counters_new() ****************/
counters_t *
counters_new(void)
{
	counters_t *ctrs = count_malloc(sizeof(counters_t));

	if (ctrs == NULL){
		return NULL;	// error allocating memory
	} else {
		// initialize contents of counters structure
		ctrs->head = NULL;
		return ctrs;
	}
}

/**************** counters_add() ****************/
void 
counters_add(counters_t *ctrs, const int key)
{
	// ignore if ctrs is NULL or key is negative.
	if (ctrs != NULL && key >= 0){
		// search to see if key exists
		for (ctrsnode_t *node = ctrs->head ; node != NULL ; node = node->next){
			// if find the key, increment count
			if (key == node->key){
				node->count ++;
				return;
			}
		}
		// didn't find key, so make a new node with the key and count = 1
		ctrsnode_t *new = ctrsnode_new(key);	// ctrsnode_new() will handle setting key and setting count to 1
		if (new == NULL)
			return;
		else {
			new->next = ctrs->head;
    		ctrs->head = new;
    	}
	}
	return;
}

/**************** ctrsnode_new() ****************/
/* Allocate and initialize a ctrsnode */
static ctrsnode_t *
ctrsnode_new(const int key)
{
	ctrsnode_t *node = count_malloc(sizeof(ctrsnode_t));

	if (node == NULL){
		// error allocating memory for node; return error
		return NULL;
	} else {
		node->key = key;
		node->count = 1;
		node->next = NULL;
		return node;
	}
}

/**************** counters_get() ****************/
int 
counters_get(counters_t *ctrs, const int key)
{
	// if ctrs is null, return 0
	if (ctrs == NULL){
		return 0;
	} else {
		// otherwise, search for key
		for (ctrsnode_t *node = ctrs->head ; node != NULL ; node = node->next){
			// if find the key, return its count value
			if (key == node->key){
				return node->count;
			}
		}
	}
	// key not found, so return 0
	return 0;
}

/**************** counters_set() ****************/
void 
counters_set(counters_t *ctrs, const int key, int count)
{
	if (ctrs != NULL && key >= 0 && count >= 0){
		for (ctrsnode_t *node = ctrs->head ; node != NULL ; node = node->next){
			// if find the key, set the current value of counter associated with the given key
			if (key == node->key){
				node->count = count;
				return;
			}
		}
		// didn't find key, so create new counter with that key and initialize the counter to the given value
		ctrsnode_t *new = ctrsnode_new(key);	// ctrsnode_new() will handle setting key
		// if creation of node didn't work, just return
		if (new == NULL)
			return;
		else {
			new->next = ctrs->head;
    		ctrs->head = new;
    		new->count = count;		// have its count value be the count parameter
    	}
	}
	return;
}

/**************** counters_print() ****************/
void 
counters_print(counters_t *ctrs, FILE *fp)
{
	if (fp != NULL) {
    	if (ctrs != NULL) {
      		fputs("{\n", fp);	// formatting...
     		for (ctrsnode_t *node = ctrs->head ; node != NULL ; node = node->next){
     			// print the node's info
     			printf("key '%d' has count '%d'\n", node->key, node->count);
     		}
      		fputc('}', fp);		// formatting...
    	} else {
      		fputs("(null)", fp);	// if ctrs is null, print (null)
    	}
  	}
}

/**************** counters_iterate() ****************/
void 
counters_iterate(counters_t *ctrs, void *arg, void (*itemfunc)(void *arg, const int key, int count))
{
	if (ctrs != NULL && itemfunc != NULL) {
    	// call itemfunc with arg, on each item
     	for (ctrsnode_t *node = ctrs->head ; node != NULL ; node = node->next){
      		(*itemfunc)(arg, node->key, node->count); 
    	}
  	}
}

/**************** counters_delete() ****************/
void 
counters_delete(counters_t *ctrs)
{
	if (ctrs != NULL){
		for (ctrsnode_t *node = ctrs->head ; node != NULL ; ){
			ctrsnode_t *next = node->next;	    // remember what comes next
      		count_free(node);			    // free the node
      		node = next;			    // and move on to next
		}
	}
	count_free(ctrs);
}

#endif 
