/**
 *  Taken from pebble development guides
 *  not directly used by our project, but needed to be included for compiling other windows
 */

#pragma once

#include <pebble.h>

#define PROGRESS_BAR_WINDOW_SIZE GSize(144, 1) // System default
#define PROGRESS_BAR_WINDOW_DELTA 33

// call this function to push this window
void progress_bar_window_push();
