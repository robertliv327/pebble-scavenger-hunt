#include <pebble.h> 

#ifndef __MESSAGE_HANDLER_H
#define __MESSAGE_HANDLER_H

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

// functions to pack and send messages to send_message()
void build_FA_LOCATION();
void build_FA_CLAIM();
void build_FA_LOG(char *text);

#endif 