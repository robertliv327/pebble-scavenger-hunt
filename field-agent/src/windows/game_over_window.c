/**
 *  game_over_window.c      Eleph     May, 2017
 *
 *  This window displays a game over message along with the final stats.
 *  These stats include gameId, number of claimed krags, number of total krags, 
 *  the number of players on the team, the full secret, and the team name
 *
 */

#include "game_over_window.h"
#include "../FAstats.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static TextLayer *s_game_over_layer;


/*************** DISABLE BACK BUTTON **************/
// code modified from https://gist.github.com/sarfata/10574031

// when back button is pressed, dont go back - just log
void game_over_back_button_handler(ClickRecognizerRef recognizer, void *context) {

} 

// We need to save a reference to the ClickConfigProvider originally set by the menu layer
ClickConfigProvider g_previous_ccp;

// This is the new ClickConfigProvider we will set, it just calls the old one and then subscribe
// for back button events.
void game_over_new_ccp(void *context) {
  g_previous_ccp(context);
  window_single_click_subscribe(BUTTON_ID_BACK, game_over_back_button_handler);
}

// Call this from your init function to do the hack
void game_over_force_back_button(Window *window, MenuLayer *menu_layer) {
  g_previous_ccp = window_get_click_config_provider(window);
  window_set_click_config_provider_with_context(window, game_over_new_ccp, menu_layer);
}
/*************************************************/

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return GAME_OVER_WINDOW_NUM_ROWS;
}

// prints out the menu
static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  static char s_buff[16];
  if ((int)cell_index->row == 0){
      snprintf(s_buff, sizeof(s_buff), "GAME OVER!!!");
  }
  if ((int)cell_index->row == 1){
      snprintf(s_buff, sizeof(s_buff), "Game ID: %s", FA_get_gameId());
  }
  if ((int)cell_index->row == 2){
      snprintf(s_buff, sizeof(s_buff), "NumClaimed: %d", FA_get_numClaimed());
  }
  if ((int)cell_index->row == 3){
      snprintf(s_buff, sizeof(s_buff), "NumPlayers: %d", FA_get_numPlayers());
  }
  if ((int)cell_index->row == 4){
      snprintf(s_buff, sizeof(s_buff), "Secret: %s", FA_get_secret());
  }
  if ((int)cell_index->row == 5){
      snprintf(s_buff, sizeof(s_buff), "Team: %s", FA_get_team());
  }
  // snprintf(s_buff, sizeof(s_buff), "Item %d", (int)cell_index->row);
  menu_cell_basic_draw(ctx, cell_layer, s_buff, NULL, NULL);
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ?
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    GAME_OVER_WINDOW_CELL_HEIGHT);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, GAME_OVER_WINDOW_MENU_HEIGHT));
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  // disable the back button for this screen
  game_over_force_back_button(window, s_menu_layer);

  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

  const GEdgeInsets message_insets = {.top = 180};
  s_game_over_layer = text_layer_create(grect_inset(bounds, message_insets));
  text_layer_set_text_alignment(s_game_over_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_game_over_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  text_layer_destroy(s_game_over_layer);

  window_destroy(window);
  s_main_window = NULL;
}

// called from outside of this function; it creates the window, 
// sets up load and unload, and pushes it onto the stack
void game_over_window_push() {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}
