/*  
 *  show_full_hint_window.h      Eleph     May, 2017
 *  
 *  This window displays the full version of the hint that the
 *  user chose to view.
 *
 */

#include "show_full_hint_window.h"

static Window *s_main_window;
static TextLayer *s_hint_layer;
static char *hint_to_print;
 
// loads the window
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_hint_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 30), bounds.size.w, 140));

  text_layer_set_background_color(s_hint_layer, GColorClear);
  text_layer_set_text_color(s_hint_layer, GColorBlack);
  text_layer_set_text(s_hint_layer, hint_to_print);
  text_layer_set_font(s_hint_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_hint_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_hint_layer));
}

// unloads the window
static void window_unload(Window *window) {
  text_layer_destroy(s_hint_layer);

  window_destroy(window);
  s_main_window = NULL;
}

// called from outside of this function; it creates the window, 
// sets up load and unload, and pushes it onto the stack
void show_full_hint_window_push(char *hint) {
  if(!s_main_window) {
    hint_to_print = hint;
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}
