/*  
 *  show_full_hint_window.h      Eleph     May, 2017
 *  
 *  This window displays the full version of the hint that the
 *  user chose to view.
 *
 */

#pragma once

#include <pebble.h> 

// call this function to push this window
void display_text_window_push(char *hint);