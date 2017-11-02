/**
 *  display_text_window.c      Eleph     May, 2017
 *
 *  This window displays a message that is passed in as a parameter to the push function.
 *
 */

#include "display_text_window.h"

static Window *s_main_window;
static TextLayer *s_text_layer;
static char *text_to_print;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_text_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 30), bounds.size.w, 140));
 
  text_layer_set_background_color(s_text_layer, GColorClear);
  text_layer_set_text_color(s_text_layer, GColorBlack);
  text_layer_set_text(s_text_layer, text_to_print);
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_text_layer);

  window_destroy(window);
  s_main_window = NULL;
}

// called from outside of this function; it creates the window, 
// sets up load and unload, and pushes it onto the stack
void display_text_window_push(char *text) {
  if(!s_main_window) {
    text_to_print = text;
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}
