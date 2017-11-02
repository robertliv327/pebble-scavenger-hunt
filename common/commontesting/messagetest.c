#ifdef NOPEBBLE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../message.h"

int main(int argv, char* argc[]) {
  message_t* m = message_new();
  set_lastContact(m, 100);
  int l = get_lastContact(m);
  printf("last contact is %d\n", l);
  char *oc = malloc(12);
  strcpy(oc, "HI FRIENDS");
  set_opCode(m, oc);
  char* o = get_opCode(m);
  printf("opcodde is %s\n", o);
  set_guideId(m, 10001);
  unsigned int g = get_guideId(m);
  printf("guideId is %x\n", g);
  char * printed_message = message_to_string(m);
  printf("%s\n", printed_message);
  free(printed_message);
  message_delete(m);
  return 0;
}

#endif 