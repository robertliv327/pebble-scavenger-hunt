#ifdef NOPEBBLE 

/* 
 * bag.c - CS50 'bag' module
 *
 * see bag.h for more information.
 *
 * David Kotz, April 2016, 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bag.h"
#include "memory.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct bagnode {
  void *item;		      // pointer to data for this item
  struct bagnode *next;	      // link to next node
} bagnode_t;

/**************** global types ****************/
typedef struct bag {
  struct bagnode *head;	      // head of the list of items in bag
} bag_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see bag.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static bagnode_t *bagnode_new(void *item);

/**************** bag_new() ****************/
bag_t *
bag_new(void)
{
  bag_t *bag = count_malloc(sizeof(bag_t));

  if (bag == NULL) {
    return NULL; // error allocating bag
  } else {
    // initialize contents of bag structure
    bag->head = NULL;
    return bag;
  }
}

/**************** bag_insert() ****************/
void
bag_insert(bag_t *bag, void *item)
{
  if (bag != NULL && item != NULL) {
    // allocate a new node to be added to the list
    bagnode_t *new = bagnode_new(item);
    if (new != NULL) {
      // add it to the head of the list
      new->next = bag->head;
      bag->head = new;
    }
  }

#ifdef MEMTEST
  count_report(stdout, "After bag_insert");
#endif
}


/**************** bagnode_new ****************/
/* Allocate and initialize a bagnode */
static bagnode_t * // not visible outside this file
bagnode_new(void *item)
{
  bagnode_t *node = count_malloc(sizeof(bagnode_t));

  if (node == NULL) {
    // error allocating memory for node; return error
    return NULL;
  } else {
    node->item = item;
    node->next = NULL;
    return node;
  }
}

/**************** bag_extract() ****************/
void *
bag_extract(bag_t *bag)
{
  if (bag == NULL) {
    return NULL; // bad bag
  } else if (bag->head == NULL) {
    return NULL; // bag is empty
  } else {
    bagnode_t *out = bag->head; // the node to take out
    void *item = out->item;	// the item to return
    bag->head = out->next;	// hop over the node to remove
    count_free(out);
    return item;
  }
}

/**************** bag_print() ****************/
void
bag_print(bag_t *bag, FILE *fp, void (*itemprint)(FILE *fp, void *item) )
{
  if (fp != NULL) {
    if (bag != NULL) {
      fputc('{', fp);
      for (bagnode_t *node = bag->head; node != NULL; node = node->next) {
	// print this node
	if (itemprint != NULL) {  // print the node's item 
	  (*itemprint)(fp, node->item); 
	  fputc(',', fp);
	}
      }
      fputc('}', fp);
    } else {
      fputs("(null)", fp);
    }
  }
}

/**************** bag_iterate() ****************/
void
bag_iterate(bag_t *bag, void *arg, void (*itemfunc)(void *arg, void *item) )
{
  if (bag != NULL && itemfunc != NULL) {
    // call itemfunc with arg, on each item
    for (bagnode_t *node = bag->head; node != NULL; node = node->next) {
      (*itemfunc)(arg, node->item); 
    }
  }
}

/**************** bag_delete() ****************/
void 
bag_delete(bag_t *bag, void (*itemdelete)(void *item) )
{
  if (bag != NULL) {
    for (bagnode_t *node = bag->head; node != NULL; ) {
      if (itemdelete != NULL) {		    // if possible...
        (*itemdelete)(node->item);	    // delete node's item
      }
      bagnode_t *next = node->next;	    // remember what comes next
      count_free(node);			    // free the node
      node = next;			    // and move on to next
    }

    count_free(bag);
  }

#ifdef MEMTEST
  count_report(stdout, "End of bag_delete");
#endif
}

#endif 
