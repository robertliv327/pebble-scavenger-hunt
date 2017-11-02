/**
 *  hint_window.c      Eleph     May, 2017
 *
 *  This window displays a shortened version of the seven most recent hints.
 *  If there have not been seven or more received hints, there will be 
 *  placeholders that simply say "N/A". If the user clicks on the shortened
 *  version of the hint, a new window will pop up with the full version.
 *
 */

#include "hint_window.h"
#include "display_text_window.h"
#include "../FAstats.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;
static TextLayer *s_hint_layer;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return HINT_WINDOW_NUM_ROWS;
} 

// prints out the menu
static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  static char s_buff[20];

  // shorten the hint to make it fit on the line...
  static char shortened[20];
  for (int i = 0; i < 20; i++){
    if (i > 12){
      shortened[i] = '>';
    } else {
      char **hint_array = FA_get_hints();
      shortened[i] = hint_array[(int)cell_index->row][i];
    }
  }
  //hints[(int)cell_index->row];

  snprintf(s_buff, sizeof(s_buff), "%s", shortened);

  // draw it on the screen... 
  menu_cell_basic_draw(ctx, cell_layer, s_buff, NULL, NULL);
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ?
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    HINT_WINDOW_CELL_HEIGHT);
}

// will push a window that displays the full text of the chosen hint
static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {

  APP_LOG(APP_LOG_LEVEL_INFO, "Player chose to look at hint %d\n", (int)cell_index->row);

  char **hint_array = FA_get_hints();
  display_text_window_push(hint_array[(int)cell_index->row]);

}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(GRect(bounds.origin.x, bounds.origin.y, bounds.size.w, HINT_WINDOW_MENU_HEIGHT));
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
      .select_click = select_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

  const GEdgeInsets message_insets = {.top = 180};
  s_hint_layer = text_layer_create(grect_inset(bounds, message_insets));
  text_layer_set_text_alignment(s_hint_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_hint_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  text_layer_destroy(s_hint_layer);

  window_destroy(window);
  s_main_window = NULL;
}

// called from outside of this function; it creates the window, 
// sets up load and unload, and pushes it onto the stack
void hint_window_push() {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}