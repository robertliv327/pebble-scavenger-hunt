/* 
 * opCode_handler.h     Eleph     May, 2017
 *
 * Handles the processing of each opCode for Field Agent. 
 * Called when a message is received by Field Agent
 * Does this processing by using a function table to match the incoming message struct to a function.
 * Then calls that funciton to perform correct response to message.
 * 
 * Functionality:
 * 	- When Field Agent receives a message and has it packaged into a message struct, Field Agent will call 
 * 	  do_opCode(), passing in the message struct - this will perform the relevant response to the message.
 *
 */

#ifndef __opCode_handler_H
#define __opCode_handler_H

#include <pebble.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../common/message.h"
#include "FAstats.h"


/**************** file−local global variables ****************/ 
// none

/**************** local structs/types ****************/ 
// none

/**************** do_opCode() ****************/
/*
 *  Is called when Field Agent receives a message. 
 *  It takes in a message struct, which will have an opCode field. 
 *  Function matches what's in the message struct's opCode field to a function via the function table.
 *  Then calls the matched function to correctly respond to the message according to the 
 *  pseudocode laid out in the requirements spec.
 */
void do_opCode(message_t *message);

#endif // __opCode_handler_H