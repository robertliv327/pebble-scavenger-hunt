/**
 *  display_text_window.h      Eleph     May, 2017
 *
 *  This window displays a message that is passed in as a parameter to the push function.
 *
 */

#pragma once

#include <pebble.h>

// call this function to push this window
void display_text_window_push(char *text);
