/**
 *  choose_team_window.c      Eleph     May, 2017
 *
 *  This window displays a list of team names for the user to choose from.
 *
 */

#include "choose_team_window.h"
#include "../FAstats.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static TextLayer *s_choose_team_layer;

/*************** DISABLE BACK BUTTON **************/
// code modified from https://gist.github.com/sarfata/10574031
// we do this so that the user does not go to a window we don't want them on...

// when back button is pressed, dont go back - just log
void team_back_button_handler(ClickRecognizerRef recognizer, void *context) {

} 

// We need to save a reference to the ClickConfigProvider originally set by the menu layer
ClickConfigProvider team_previous_ccp;

// This is the new ClickConfigProvider we will set, it just calls the old one and then subscribe
// for back button events.
void team_new_ccp(void *context) {
  team_previous_ccp(context);
  window_single_click_subscribe(BUTTON_ID_BACK, team_back_button_handler);
}

// Call this from your init function to do the hack
void team_force_back_button(Window *window, MenuLayer *menu_layer) {
  team_previous_ccp = window_get_click_config_provider(window);
  window_set_click_config_provider_with_context(window, team_new_ccp, menu_layer);
}
/*************************************************/

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return CHOOSE_TEAM_WINDOW_NUM_ROWS;
}

// display the possible team names
static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  static char s_buff[16];
  if ((int)cell_index->row == 0){
      snprintf(s_buff, sizeof(s_buff), "alien");
  }
  if ((int)cell_index->row == 1){
      snprintf(s_buff, sizeof(s_buff), "beetle");
  }
  if ((int)cell_index->row == 2){
      snprintf(s_buff, sizeof(s_buff), "carpool");
  }
  if ((int)cell_index->row == 3){
      snprintf(s_buff, sizeof(s_buff), "dog");
  }
  // snprintf(s_buff, sizeof(s_buff), "Item %d", (int)cell_index->row);
  menu_cell_basic_draw(ctx, cell_layer, s_buff, NULL, NULL);
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ?
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    CHOOSE_TEAM_WINDOW_CELL_HEIGHT);
}

// set the chosen team name to FA stats variable
static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  char *team = malloc(8 * sizeof(char*));

  if ((int)cell_index->row == 0){
    strcpy(team, "alien");
  }
  if ((int)cell_index->row == 1){
    strcpy(team, "beetle");
  }
  if ((int)cell_index->row == 2){
    strcpy(team, "carpool");
  }
  if ((int)cell_index->row == 3){
    strcpy(team, "dog");
  }

  // plug the team into FA stats under team name...
  FA_set_team(team);

  APP_LOG(APP_LOG_LEVEL_INFO, "Joined game with team: %s", team);

  window_stack_pop(true);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, CHOOSE_TEAM_WINDOW_MENU_HEIGHT));
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  // disable the back button for this screen
  team_force_back_button(window, s_menu_layer);

  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
      .select_click = select_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

  const GEdgeInsets message_insets = {.top = 140};
  s_choose_team_layer = text_layer_create(grect_inset(bounds, message_insets));
  text_layer_set_text_alignment(s_choose_team_layer, GTextAlignmentCenter);
  text_layer_set_text(s_choose_team_layer, CHOOSE_TEAM_WINDOW_HINT_TEXT);
  layer_add_child(window_layer, text_layer_get_layer(s_choose_team_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  text_layer_destroy(s_choose_team_layer);

  window_destroy(window);
  s_main_window = NULL;
}

// called from outside of this function; it creates the window, 
// sets up load and unload, and pushes it onto the stack
void choose_team_window_push() {
  if(!s_main_window) {
    // set up window
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}