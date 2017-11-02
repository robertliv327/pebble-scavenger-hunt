/**
 *  hint_window.h      Eleph     May, 2017
 *
 *  This window displays a shortened version of the seven most recent hints.
 *  If there have not been seven or more received hints, there will be 
 *  placeholders that simply say "N/A". If the user clicks on the shortened
 *  version of the hint, a new window will pop up with the full version.
 *
 */

#pragma once

#include <pebble.h>
#include "../FAstats.h"

#define HINT_WINDOW_NUM_ROWS    7
#define HINT_WINDOW_CELL_HEIGHT 25
#define HINT_WINDOW_MENU_HEIGHT \
    HINT_WINDOW_NUM_ROWS * HINT_WINDOW_CELL_HEIGHT
// #define HINT_WINDOW_HINT_TEXT   "Your list items"

// call this function to push this window
void hint_window_push(); 
