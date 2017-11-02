/*  
 *  repick_team_name_window.c      Eleph     May, 2017
 *  
 *  This window is called if the player must repick their team name
 *  This results when a user leaves the game and tries to re-enter under a
 *  different team. It simply displays a message telling the user what has 
 *  happened, then pops this window and pushes the choose_team_window upon a click
 *
 */

#include "repick_team_name_window.h"
#include "windows/choose_team_window.h"

static Window *s_main_window;
static TextLayer *s_repick_team_layer;
static char *text_to_print;

// overrides back button - now will pop this window and push the choose team window
void team_back_single_click_handler(ClickRecognizerRef recognizer, void *context) {
    window_stack_pop(true);
    choose_team_window_push();
} 

void team_config_provider(Window *window) {
 // single click / repeat-on-hold config:
  window_single_click_subscribe(BUTTON_ID_BACK, team_back_single_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);


  // Create the TextLayer with specific bounds
  s_repick_team_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 30), bounds.size.w, 140));

  text_layer_set_background_color(s_repick_team_layer, GColorClear);
  text_layer_set_text_color(s_repick_team_layer, GColorBlack);
  text_layer_set_text(s_repick_team_layer, text_to_print);
  text_layer_set_font(s_repick_team_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_repick_team_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_repick_team_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_repick_team_layer);

  window_destroy(window);
  s_main_window = NULL;
}

// called from outside of this function; it creates the window, 
// sets up load and unload, and pushes it onto the stack
void repick_team_name_window_push(char *text) {
  if(!s_main_window) {
    text_to_print = text;
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    window_set_click_config_provider(s_main_window, (ClickConfigProvider) team_config_provider);
  }
  window_stack_push(s_main_window, true);
}