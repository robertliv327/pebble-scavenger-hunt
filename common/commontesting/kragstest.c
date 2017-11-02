#ifdef NOPEBBLE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../krags.h"
#include "../../libcs50/bag.h"
#include "../../libcs50/hashtable.h"
#include "../../libcs50/set.h"
#include "../../libcs50/counters.h"

void ht_delete(void *item);
static void hashprint(FILE *fp, const char *key, void *item);
void krag_info_print(void *arg, const char * key, void * item);

int main(int argv, char* argc[]) {
  hashtable_t* ht = hashtable_new(5);
  int numkrags = read_krags_from_file(ht, "../fakekragfile.txt");
  printf("\n number of things: %i\n", numkrags);
  printf("hashtable: \n");
  hashtable_iterate(ht, NULL, krag_info_print);
  hashtable_delete(ht, ht_delete);
  return 0;
}

//write a hastable deleteeeeee
void ht_delete(void *item) {
  krag_info_t* k = item;
  krag_info_delete(k);
}

static void
hashprint(FILE *fp, const char *key, void *item)
{
  int *num = item;
  if (num == NULL || key == NULL)
    fprintf(fp, "[(null)]");
  else
    fprintf(fp, "--key %s--",
            key);
}




void krag_info_print(void *arg, const char * key, void * item) {
  krag_info_t * krag = item;
  printf("\nkey: %s\n", key);
  printf("kragId: %x\n", krag_get_kragId(krag));
  printf("lat: %f\n", krag_get_latitude(krag));
  printf("long: %f\n", krag_get_longitude(krag));
  printf("index: %i\n", krag_get_index(krag));
  printf("clue: %s\n", krag_get_clue(krag));
}

#endif 