/*
 * opCode_handler.h     Eleph     May, 2017
 *
 * Handles the processing of each opCode for Guide Agent. 
 * Called when a message is received by Guide Agent
 * Does this processing by using a function table to match the incoming message struct to a function.
 * Then calls that funciton to perform correct response to message.
 * 
 * Functionality:
 * 	- When Guide Agent receives a message and has it packaged into a message struct, Guide Agent will call 
 * 	  do_opCode(), passing in the message struct - this will perform the relevant response to the message.
 *
 */

#ifndef __opCode_handler_H
#define __opCode_handler_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "message.h"
#include "GAstats.h"


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
void do_opCode(GA_stats_t* g, message_t *message);

#endif // __opCode_handler_H
