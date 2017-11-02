/*  
 *	status_window.h      Eleph     May, 2017
 *	
 *	This window displays the game status to the user upon request.
 *  This includes the team name, the player's name,
 *  the number of claimed krags, and the number of total krags.
 *
 */

#pragma once

#include <pebble.h>
#include "../FAstats.h"

#define STATUS_WINDOW_NUM_ROWS    4
#define STATUS_WINDOW_CELL_HEIGHT 30
#define STATUS_WINDOW_MENU_HEIGHT \
    STATUS_WINDOW_NUM_ROWS * STATUS_WINDOW_CELL_HEIGHT

// call this function to push this window
void status_window_push(); 
