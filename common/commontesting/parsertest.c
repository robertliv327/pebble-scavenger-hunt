#ifdef NOPEBBLE 

//Parsertest.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../message.h"


int main(int argv, char* argc[]) {
  char* string1 = "opCode=FA_LOCATION|gameId=0|pebbleId=8080477D|team=aqua|player=Alice|latitude=43.706552|longitude=-72.287418|statusReq=0";
  message_t *message = message_new();
  message_parse(message, string1);
  
  printf("opCode: %s\n", get_opCode(message));
  printf("respCode: %s\n", get_respCode(message));
  printf("gameId: %x\n", get_gameId(message));
  message_delete(message);
}

#endif 