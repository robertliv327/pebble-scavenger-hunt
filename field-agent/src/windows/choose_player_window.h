/**
 *  choose_player_window.h      Eleph     May, 2017
 *
 *  This window displays a list of player names for the user to choose from.
 *
 */

#pragma once

#include <pebble.h>
#include "../FAstats.h"

#define CHOOSE_PLAYER_WINDOW_NUM_ROWS    4
#define CHOOSE_PLAYER_WINDOW_CELL_HEIGHT 25
#define CHOOSE_PLAYER_WINDOW_HINT_TEXT "Choose a player name"
#define CHOOSE_PLAYER_WINDOW_MENU_HEIGHT \
    CHOOSE_PLAYER_WINDOW_NUM_ROWS * CHOOSE_PLAYER_WINDOW_CELL_HEIGHT
// #define CHOOSE_PLAYER_WINDOW_CHOOSE_PLAYER_TEXT   "Your list items"

// call this function to push this window
void choose_player_window_push();
