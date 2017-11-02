/*
 * message_handler.c     Eleph     May, 2017
 *
 * Handles the code that has to do with communication with the proxy/smartphone
 * This includes sending messages, requesting info, managing inbox/outbox, etc.
 * 
 */ 

#include <pebble.h>
#include "key_assembly.h"
#include "../../common/message.h"
#include "FAstats.h"
#include "windows/display_text_window.h"
#include "opCode_handler.h"


/**************** function prototypes ****************/ 
// tick handler function
void tick_handler(struct tm *tick_time, TimeUnits units_changed);

// AppMessage functions
void inbox_received_callback(DictionaryIterator *iterator, void *context);
void outbox_sent_callback(DictionaryIterator *iterator, void *context);
void inbox_dropped_callback(AppMessageResult reason, void *context);
void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context);

// my message functions
void request_pebbleId();
void request_location();
static void send_message(char *message);

// functions to pack and send messages to send_message()
void build_FA_LOCATION();
void build_FA_CLAIM();
void build_FA_LOG(char *text);

static void add_location_to_message(char* message_string);

// global to hold values returned in inbox
char *location_global;

// global that gets set to true as soon as we get an AppKeyJSReady...
// used as a flag in tick handler to wait to request pebbleID
bool app_ready = false;

/**************** tick_handler() ****************/
void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Tick.");
    static int seconds = 15;

    // will do as soon as we get a AppKeyJSReady
    if (app_ready){
        // request pebId right at start of game
        request_pebbleId();
        app_ready = false;
    }

    // send out initial FA_LOCATION once user has chosen team and player (but dont send more if it's still processing)
    if (FA_get_waiting_for_response() && FA_get_user_not_joined_game() && (FA_get_team() != NULL) && (FA_get_player() != NULL)){
        request_location();
        // dont want to get back into this 'if statement' if it's waiting for inbox
        FA_set_waiting_for_response(false);
    }

    // send out a FA_LOCATION every 15 seconds
    if (seconds == 0) {
        // don't request a location if you haven't filled these in...
        if ((FA_get_gameId() != NULL) && (FA_get_player() != NULL) && (FA_get_team() != NULL)){
          APP_LOG(APP_LOG_LEVEL_INFO, "15 seconds went off - location request");
          request_location();
        } 
        seconds = 15;
    } else {
        seconds--;
    }
}

/**************** inbox_received_callback() ****************/
void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    /* Possible message types defined in key_assembly.h
    enum {
      AppKeyJSReady = 0,      // The JS environment on the companion smartphone is up and running!
      AppKeySendMsg,          // Send a message over Bluetooth to the companion smartphone and then on to the Game Server
      AppKeyRecvMsg,          // A message from the Game Server is available (arrived over Bluetooth)
      AppKeyLocation,         // Request your GPS location from the companion smartphone
      AppKeyPebbleId,         // Request your unique pebble ID from the companion smartphone
      AppKeySendError         // Error: companion app can't connect to the Proxy (and ultimately the Game Server).
    };
    */

    /* 1. Check to see if JS environment ready message received. */
    Tuple *ready_tuple = dict_find(iterator, AppKeyJSReady);
    if(ready_tuple) {
        // Log the value sent as part of the received message.
        char *ready_str = ready_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyJSReady: %s\n", ready_str);
        app_ready = true;
    }

    /* 2. Check to see if an opcode message was received received. */
    Tuple *msg_tuple = dict_find(iterator, AppKeyRecvMsg);
    if(msg_tuple) {
        // pack message into a struct and send it to do_opcode
        char *msg = msg_tuple->value->cstring;
        message_t *m = message_new();
        message_parse(m, msg);
        do_opCode(m);
        message_delete(m);
        // Log the value sent as part of the received message.
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyrecvMsg: %s\n", msg);
    }

    /* 3. Check to see if a location message received. */
    Tuple *loc_tuple = dict_find(iterator, AppKeyLocation);
    if(loc_tuple) {
        // Log the value sent as part of the received message.
        char *location = loc_tuple->value->cstring;
        location_global = malloc(30*sizeof(char));
        strcpy(location_global, location);
        // see if this location request was to send an FA_CLAIM or not...
        if (FA_get_krag_claim()){
            // send FA_CLAIM
            build_FA_CLAIM();
            FA_set_krag_claim(false);
        } else {
            // send FA_LOCATION
            build_FA_LOCATION();
        }
        // free location_global for next malloc...
        free(location_global);
        // after sending FA_LOCATION, make sure statusReq will be 0 unless otherwise set to 1
        FA_set_statusReq(0);
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyLocation: %s\n", location_global);
    }

    /* 4. Check to see if a PebbleId message received. */
    Tuple *id_tuple = dict_find(iterator, AppKeyPebbleId);
    if(id_tuple) {
        // plug in the pebbleId into FA variable
        char *pebbleId = malloc(strlen(id_tuple->value->cstring) + 1);
        strcpy(pebbleId, id_tuple->value->cstring);
        FA_set_pebbleId(pebbleId);
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyPebbleId: %s\n", pebbleId);
        // send an FA log saying that the player has officially joined the game
        char *text = malloc(13 * sizeof(char*));
        strcpy(text, "Joined Game!");
        build_FA_LOG(text);
    }

    /* 5. Check to see if an error message was received. */
    Tuple *error_tuple = dict_find(iterator, AppKeyLocation);
    if(error_tuple) {
        // Log the value sent as part of the received message.
        char *error = error_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeySendError: %s\n", error);
    }
}

// outbox_sent_callback
void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message successfully sent.\n");
}

// inbox_dropped_callback
void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message dropped from inbox.\n");
}

// outbox_failed_callback
void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message failed to send.\n");
}

// request_pebbleId
void request_pebbleId() {
    /* 1. Declare dictionary iterator */
    DictionaryIterator *out_iter;

    /* 2. Prepare the outbox */
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    /* 3. If outbox was prepared, send request. Otherwise, log error. */
    if (result == APP_MSG_OK) {
        int value = 1;

        /* construct and send message, outgoing value ignored */
        dict_write_int(out_iter, AppKeyPebbleId, &value, sizeof(value), true);

        result = app_message_outbox_send();

        if (result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending pebbleId request from outbox.\n");
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing outbox for pebbleId request.\n");
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Requested pebbleId.\n");
}

// request_location
void request_location() {
    /* 1. Declare dictionary iterator */
    DictionaryIterator *out_iter;

    /* 2. Prepare the outbox */
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    /* 3. If outbox was prepared, send request. Otherwise, log error. */
    if (result == APP_MSG_OK) {
        int value = 1;

        /* construct and send message, outgoing value ignored */
        dict_write_int(out_iter, AppKeyLocation, &value, sizeof(value), true);

        result = app_message_outbox_send();

        if (result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending locationrequest from outbox.\n");
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing outbox for location request.\n");
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Requested location.\n");
}

// send_message
static void send_message(char *message) {
    /* 1. Declare dictionary iterator */
    DictionaryIterator *out_iter;

    /* 2. Prepare the outbox */
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    /* 3. If outbox was prepared, send message. Otherwise, log error. */
    if (result == APP_MSG_OK) {
      
        /* Construct and send the message */
        dict_write_cstring(out_iter, AppKeySendMsg, message);
      
        result = app_message_outbox_send();
      
        if(result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending message from outbox.\n");
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing outbox for send_message.\n");
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Sent message.\n");
}

/**************** build_FA_LOCATION() ****************/
/*
 *  Called when we want to send an FA_LOCATION. 
 *  Packs up a message struct, converts message struct to string,
 *  then sends string to send_message() to be sent away to GS
 */
void build_FA_LOCATION(){

    // opCode=FA_LOCATION|gameId=|pebbleId=|team=|player=|latitude=|longitude=|statusReq=
    // make a new message struct
    message_t *m = message_new();
    // pack it with the right info
    char *op = malloc(12 * sizeof(char));
    strcpy(op, "FA_LOCATION");
    set_opCode(m, op);

    // if statement to see if we should set gameId to 0 or not
    if (FA_get_user_not_joined_game()){
        char *gameId = malloc(2*sizeof(char));
        strcpy(gameId, "0");
        set_gameId(m, gameId);
    } else {
        char *gameId = malloc(strlen(FA_get_gameId()) + 1);
        strcpy(gameId, FA_get_gameId());
        set_gameId(m, gameId);
    }

    // set flag to false so that we don't send another 0 in the future... 
    // (unless they need to pick another team/player) name - handled in opCode_handler.c
    FA_set_user_not_joined_game(false);

    char *pebbleId = malloc(strlen(FA_get_pebbleId()) + 1);
    strcpy(pebbleId, FA_get_pebbleId());
    set_pebbleId(m, pebbleId);

    char *team = malloc(strlen(FA_get_team()) + 1);
    strcpy(team, FA_get_team());
    set_team(m, team);

    char *player = malloc(strlen(FA_get_player()) + 1);
    strcpy(player, FA_get_player());
    set_player(m, player);

    set_statusReq(m, FA_get_statusReq());

    // convert it to a string
    char *message = message_to_string(m);
    // add the location to the end of the string
    add_location_to_message(message);
    // free the message
    message_delete(m);
    // send_message with the string!
    send_message(message);

    APP_LOG(APP_LOG_LEVEL_INFO, "Sent FA_LOCATION\n");
}

/**************** build_FA_CLAIM() ****************/
/*
 *  Called when we want to send an FA_CLAIM. 
 *  Packs up a message struct, converts message struct to string,
 *  then sends string to send_message() to be sent away to GS
 */
void build_FA_CLAIM(){
    // opCode=FA_CLAIM|gameId=|pebbleId=|team=|player=|latitude=|longitude=|kragId=
    // make a new message struct
    message_t *m = message_new();
    // pack it with the right info
    char *op = malloc(12 * sizeof(char));
    strcpy(op, "FA_CLAIM");
    set_opCode(m, op);

    char *gameId = malloc(strlen(FA_get_gameId()) + 1);
    strcpy(gameId, FA_get_gameId());
    set_gameId(m, gameId);

    char *pebbleId = malloc(strlen(FA_get_pebbleId()) + 1);
    strcpy(pebbleId, FA_get_pebbleId());
    set_pebbleId(m, pebbleId);

    char *team = malloc(strlen(FA_get_team()) + 1);
    strcpy(team, FA_get_team());
    set_team(m, team);

    char *player = malloc(strlen(FA_get_player()) + 1);
    strcpy(player, FA_get_player());
    set_player(m, player);

    char *kragId = malloc(strlen(FA_get_krag_to_send()) + 1);
    strcpy(kragId, FA_get_krag_to_send());
    set_kragId(m, kragId);

    // convert it to a string
    char *message = message_to_string(m);
    // add the location to the end of the string...
    add_location_to_message(message);
    // free the message
    message_delete(m);
    // send_message with the string!
    send_message(message);

    APP_LOG(APP_LOG_LEVEL_INFO, "Sent FA_CLAIM\n");
}

/**************** build_FA_LOG() ****************/
/*
 *  Called when we want to send an FA_LOG. 
 *  Packs up a message struct, converts message struct to string,
 *  then sends string to send_message() to be sent away to GS
 */
void build_FA_LOG(char *text){
    // opCode=FA_LOG|pebbleId=|text=
    // make a new message struct
    message_t *m = message_new();
    // pack it with the right info
    char *op = malloc(12 * sizeof(char));
    strcpy(op, "FA_LOG");
    set_opCode(m, op);

    char *pebbleId = malloc(strlen(FA_get_pebbleId()) + 1);
    strcpy(pebbleId, FA_get_pebbleId());
    set_pebbleId(m, pebbleId);

    set_text(m, text);

    // convert it to a string
    char *message = message_to_string(m);
    // free the message
    message_delete(m);
    // send_message with the string!
    send_message(message);

    APP_LOG(APP_LOG_LEVEL_INFO, "Sent FA_LOG\n");
}


/*************** add_location_to_message() ********/
/*
 * Concatenates the location string to the end of a to-string'ed message
 * turns "lat|long" into "|latitude=lat|longitude=long" then contatenates
 */
static void add_location_to_message(char* message_string) {
  int location_size = strlen(location_global);
  char *location_copy = malloc(location_size +1);
  strcpy(location_copy, location_global);
  char * rest = location_copy;
    char*latitude = rest;
    char*longitude;
    // empty loop to get to the pipe... will save it all in latitude
    for (rest = latitude; *rest != '\0' && *rest != '|'; rest++) {}
    if (*rest != '\0') {
      *rest++ = '\0';
    }
    // assign rest of the string to longitude
    longitude = rest;
    // concatenate it all together
    strcat(message_string, "|latitude=");
    strcat(message_string, latitude);
    strcat(message_string, "|longitude=");
    strcat(message_string, longitude);
    free(location_copy);
       
}

			     
