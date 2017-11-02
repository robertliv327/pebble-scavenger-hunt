






#ifndef __message_handler_GA_H
#define __message_handler_GA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>           // strlen                                                                                                                          
#include <strings.h>          // bcopy, bzero                                                                                                                    
#include <arpa/inet.h>        // socket-related calls                                                                                                            
#include <sys/select.h>       // select-related stuff                                                                                                            
#include <ctype.h>

#include "hashtable.h"
#include "message.h"

// function headers for message_handler.c

typedef struct socket_struct socket_struct_t;
socket_struct_t* socket_struct_new(void);
int get_comm_socket(socket_struct_t *sock_struct);
struct sockaddr_in * get_address(socket_struct_t *sock_struct);
void set_address(socket_struct_t *sock_struct, struct sockaddr_in *sAddress);
void set_comm_socket(socket_struct_t *sock_struct, int comm_sock);
void send_message_gs(message_t *message, int iD, socket_struct_t *sock_struct);


#endif
