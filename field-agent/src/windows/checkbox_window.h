/**
 *  Taken from pebble development guides
 *  not directly used by our project, but needed to be included for compiling other windows
 */

#pragma once

#include <pebble.h>

#define CHECKBOX_WINDOW_NUM_ROWS    4
#define CHECKBOX_WINDOW_BOX_SIZE    12
#define CHECKBOX_WINDOW_CELL_HEIGHT 44

// call this function to push this window
void checkbox_window_push();
