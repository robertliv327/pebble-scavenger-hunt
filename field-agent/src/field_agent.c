/*
 *  field_agent.c     Eleph     May, 2017
 *
 *  The main functionality for running the game on the Pebble
 *  Sets up the main window and calls for other windows to be pushed appropriately
 *  Reaches out to message_handler.c for messages/proxy-related code
 *  
 */
 
#include <pebble.h>

// Local includes
#include "key_assembly.h"
#include "../../common/message.h"
#include "FAstats.h"
#include "opCode_handler.h"
#include "message_handler.h"

// Window includes
#include "windows/checkbox_window.h"
#include "windows/status_window.h"
#include "windows/hint_window.h"
#include "windows/pin_window.h"
#include "windows/display_text_window.h"
#include "windows/choose_team_window.h"
#include "windows/choose_player_window.h"
#include "windows/game_over_window.h"
#include "windows/repick_player_name_window.h"
#include "windows/repick_team_name_window.h"
//#include "windows/text_animation_window.h"
//#include "windows/progress_bar_window.h"
//#include "windows/progress_layer_window.h"

// GLOBALS
#define NUM_WINDOWS 4
static Window *s_main_window;
static MenuLayer *s_menu_layer;


/*************************************************************************/
/*************************************************************************/


/*************** DISABLE BACK BUTTON **************/
// code modified from https://gist.github.com/sarfata/10574031
// I do this so that the user can't accidentally quit the game

// when back button is pressed, dont go back
void back_button_handler(ClickRecognizerRef recognizer, void *context) {

}

// We need to save a reference to the ClickConfigProvider originally set by the menu layer
ClickConfigProvider previous_ccp;

// This is the new ClickConfigProvider we will set, it just calls the old one and then subscribe
// for back button events.
void new_ccp(void *context) {
  previous_ccp(context);
  window_single_click_subscribe(BUTTON_ID_BACK, back_button_handler);
}

// Call this from your init function to do the hack
void force_back_button(Window *window, MenuLayer *menu_layer) {
  previous_ccp = window_get_click_config_provider(window);
  window_set_click_config_provider_with_context(window, new_ccp, menu_layer);
}
/*************************************************/


static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return NUM_WINDOWS;
}

// draw the menu items
static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, "Input Krag", NULL, NULL);
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "View Hints", NULL, NULL);
      break;
    case 2:
      menu_cell_basic_draw(ctx, cell_layer, "View Game Stats", NULL, NULL);
      break;
    case 3:
      menu_cell_basic_draw(ctx, cell_layer, "Request Updates", NULL, NULL);
      break;
    default:
      break;
  }
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ?
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    CHECKBOX_WINDOW_CELL_HEIGHT);
}

static void pin_complete_callback(PIN pin, void *context) {
   APP_LOG(APP_LOG_LEVEL_INFO, "Krag inputed was %x %x %x %x", pin.digits[0], pin.digits[1], pin.digits[2], pin.digits[3]);

   static char krag_str[5];
   snprintf(krag_str, sizeof(krag_str), "%x%x%x%x", pin.digits[0], pin.digits[1], pin.digits[2], pin.digits[3]);
   char *krag = malloc(5*sizeof(char));
   strcpy(krag, krag_str);

   // free previous krag if we stored one
   if (FA_get_krag_to_send() != NULL){
      free(FA_get_krag_to_send());
   }
   // plug in new krag
   FA_set_krag_to_send(krag);

   // let the "AppKeyLocation" code know that this request location was to load up an FA_CLAIM
   // build_FA_CLAIM() will be called from there by using this flag
   FA_set_krag_claim(true);
   // dont send request if GAME_STATUS has not been received yet
   if (FA_get_gameId() != NULL){
          request_location();
      } else {
          display_text_window_push("Please try again. Server has not yet responded with GAME_STATUS...");
      }

   pin_window_pop((PinWindow*)context, true);
}

// take action depending on which menu item was selected
static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    case 0:{
        PinWindow *pin_window = pin_window_create((PinWindowCallbacks) {
          .pin_complete = pin_complete_callback
        });
        pin_window_push(pin_window, true);
      }
      break;
    case 1:
      hint_window_push();
      break;
    case 2:
      status_window_push();
      break;
    case 3:
      // we want a status req
      FA_set_statusReq(1);
      // then request location and send FA_LOCATION... but only if we've received out GAME_STATUS
      if (FA_get_gameId() != NULL){
          request_location();
          display_text_window_push("Your updated game stats are now available from the main menu.");
      } else {
          display_text_window_push("Please try again. Server has not yet responded with GAME_STATUS...");
      }
      break;
    default:
      break;
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  // disable the back button for this screen
  force_back_button(window, s_menu_layer);

#if defined(PBL_COLOR)
  menu_layer_set_normal_colors(s_menu_layer, GColorBlack, GColorWhite);
  menu_layer_set_highlight_colors(s_menu_layer, GColorRed, GColorWhite);
#endif
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
      .select_click = select_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
}

// init! sets up the windows, stats, etc.
static void init() {
  /* 1. Create main Window element. */
  s_main_window = window_create();

  /* 2. Set handlers to manage the elements inside the window. */
  window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });

  // initialize stats
  FA_stats_new();
  FA_set_hints(); // malloc empty spaces for each slot in hints array

  /* 4. Show the Window on the watch, with animated=true. */
  window_stack_push(s_main_window, true);
  choose_team_window_push();
  choose_player_window_push();

  /* 6. Set the handlers for AppMessage inbox/outbox events. Set these    *
   *    handlers BEFORE calling open, otherwise you might miss a message. */
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);

  /* 7. open the app message communication protocol. Request as much space *
   *    as possible, because our messages can be quite large at times.     */
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  /* 3. Register our tick_handler function with TickTimerService. */
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

// take down the windows, timer, stats
static void deinit() {
    /* 1. Destroy the window. */
    window_destroy(s_main_window);

    /* 2. Unsubscribe from sensors. */
    tick_timer_service_unsubscribe();

    // delete the mallocced stats
    FA_stats_delete();
}

// main! just calls to init, sets up the app event loop, and deinits at the end
int main() {
  init();
  app_event_loop();
  deinit();
}
