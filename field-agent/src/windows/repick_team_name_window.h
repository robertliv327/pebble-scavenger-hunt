/*  
 *  repick_team_name_window.h      Eleph     May, 2017
 *  
 *  This window is called if the player must repick their team name
 *  This results when a user leaves the game and tries to re-enter under a
 *  different team. It simply displays a message telling the user what has 
 *  happened, then pops this window and pushes the choose_team_window upon a click
 *
 */

#pragma once

#include <pebble.h>

// call this function to push this window 
void repick_team_name_window_push(char *text);