#pragma once

#include <pebble.h>
#include "../FAstats.h"

#define GAME_OVER_WINDOW_NUM_ROWS    6
#define GAME_OVER_WINDOW_CELL_HEIGHT 30
#define GAME_OVER_WINDOW_MENU_HEIGHT \
    GAME_OVER_WINDOW_NUM_ROWS * GAME_OVER_WINDOW_CELL_HEIGHT
// #define GAME_OVER_WINDOW_HINT_TEXT   "Your list items"

// call this function to push this window
void game_over_window_push(); 