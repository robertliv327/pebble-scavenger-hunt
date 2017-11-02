/**
 *  choose_team_window.h      Eleph     May, 2017
 *
 *  This window displays a list of team names for the user to choose from.
 *
 */

#pragma once

#include <pebble.h>
#include "../FAstats.h"

#define CHOOSE_TEAM_WINDOW_NUM_ROWS    4
#define CHOOSE_TEAM_WINDOW_CELL_HEIGHT 25
#define CHOOSE_TEAM_WINDOW_HINT_TEXT "Choose a team name"
#define CHOOSE_TEAM_WINDOW_MENU_HEIGHT \
    CHOOSE_TEAM_WINDOW_NUM_ROWS * CHOOSE_TEAM_WINDOW_CELL_HEIGHT
// #define CHOOSE_TEAM_WINDOW_CHOOSE_TEAM_TEXT   "Your list items"

// call this function to push this window
void choose_team_window_push();
