/*  
 *  status_window.c      Eleph     May, 2017
 *  
 *  This window displays the game status to the user upon request.
 *  This includes the team name, the player's name,
 *  the number of claimed krags, and the number of total krags.
 *
 */ 

#include "status_window.h"
#include "../FAstats.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static TextLayer *s_status_layer;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return STATUS_WINDOW_NUM_ROWS;
}

// Displays the menu - one row for each item included in the game stats
static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  static char s_buff[16];
  if ((int)cell_index->row == 0){
      snprintf(s_buff, sizeof(s_buff), "Team: %s", FA_get_team());
  }
  if ((int)cell_index->row == 1){
      snprintf(s_buff, sizeof(s_buff), "Player: %s", FA_get_player());
  }
  if ((int)cell_index->row == 2){
      snprintf(s_buff, sizeof(s_buff), "# Claimed: %d", FA_get_numClaimed());
  }
  if ((int)cell_index->row == 3){
      snprintf(s_buff, sizeof(s_buff), "# Total: %d", FA_get_numKrags());
  }
  // snprintf(s_buff, sizeof(s_buff), "Item %d", (int)cell_index->row);
  menu_cell_basic_draw(ctx, cell_layer, s_buff, NULL, NULL);
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ?
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    STATUS_WINDOW_CELL_HEIGHT);
}

// sets up the window
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, STATUS_WINDOW_MENU_HEIGHT));
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

  const GEdgeInsets message_insets = {.top = 180};
  s_status_layer = text_layer_create(grect_inset(bounds, message_insets));
  text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_status_layer));
}


// unloads the window
static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  text_layer_destroy(s_status_layer);

  window_destroy(window);
  s_main_window = NULL;
}

// called from outside of this function; it creates the window, 
// sets up load and unload, and pushes it onto the stack
void status_window_push() {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}
