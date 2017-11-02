/*
 * defines struct for holding comm_socket and hashtable
 * 
 *
 *
 */


#ifndef __message_handler_H
#define __message_handler_H 

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

typedef struct socket_struct socket_struct_t;


/**************** socket_struct_new ****************/
/*                                                                              
* create new socket_struct                                                     
* return pointer to struct or                                                 
* NULL if error                                                                
*                                                                              
*/
socket_struct_t* socket_struct_new(void);


/**************** socket_struct_new ****************/
/*                                                                             
 * create new socket_struct                                                    
 * return pointer to struct or                                                 
 * NULL if error                                                               
 *                                                                             
 */


//getters and setters for socket_struct_t
int get_comm_socket(socket_struct_t *sock_struct);
hashtable_t *get_address_ht(socket_struct_t *sock_struct);
struct sockaddr_in * get_last_sender(socket_struct_t *sock_struct);
void set_address_ht(socket_struct_t *sock_struct, hashtable_t *ht);
void set_comm_socket(socket_struct_t *sock_struct, int comm_sock);
void set_last_sender( socket_struct_t *sock_struct, struct sockaddr_in *lastsender);


// Sends a message given a message_t struct.
// Memory contract: frees the message being sent
void send_message(message_t *message, unsigned int iD, socket_struct_t *sock_struct);



 /**************** print_log *************/
 /* Prints a message to ../logs/gameserver.log                                                                                  * Appends to the end of the log file -- if you need to                                                                          * reset the file, you must do so separately!                                                                                    */

void print_log(char * message, struct sockaddr_in *address, char* tofrom);




#endif 



