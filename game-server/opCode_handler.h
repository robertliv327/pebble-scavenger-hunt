/*
 * opCode_handler.h     Eleph     May, 2017
 *
 * Handles the processing of each opCode for Gameserver. 
 * Called when a message is received by Gameserver
 * Does this processing by using a function table to match the incoming message struct to a function.
 * Then calls that funciton to perform correct response to message.
 * 
 * Functionality:
 * 	- When gameserver receives a message and has it packaged into a message struct, Gameserver will call 
 * 	  do_opCode(), passing in the message struct - this will perform the relevant response to the message.
 *
 */

#ifndef __OPCODE_HANDLER_H
#define __OPCODE_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "message.h"
#include "gamestats.h"


/**************** file−local global variables ****************/ 
// none

/**************** local structs/types ****************/ 
// none

/**************** do_opCode() ****************/
/*
 *  Is called when Gameserver receives a message. 
 *  It takes in a message struct, which will have an opCode field. 
 *  Also takes in a game_stats struct for accessing the server's game stats and 
 *  a socket_struct for sending messages via send_message.
 *  Function matches what's in the message struct's opCode field to a function via the function table.
 *  Then calls the matched function to correctly respond to the message according to the 
 *  pseudocode laid out in the requirements spec.
 */
void do_opCode(message_t *message, game_stats_t *stats, socket_struct_t *sock);







/**************** handle_game_over *****************/

void handle_game_over(game_stats_t *game_stats, socket_struct_t *sock_struct);
#endif // __OPCODE_HANDLER_H
