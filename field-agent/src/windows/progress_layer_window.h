/**
 *  Taken from pebble development guides
 *  not directly used by our project, but needed to be included for compiling other windows
 */

#pragma once

#include <pebble.h>

#include "../layers/progress_layer.h"

#define PROGRESS_LAYER_WINDOW_DELTA 33
#define PROGRESS_LAYER_WINDOW_WIDTH 80

// call this function to push this window
void progress_layer_window_push(); 
