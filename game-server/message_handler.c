/* message.c 
 *
 * Eleph, May 2017
 * file for socket-related functions and structs
 * Refer to message_handler.h for more details
 *
 *
 *
 *
 */

#include "message_handler.h"
#include <time.h>


// Socket struct: for storing socket information 
typedef struct socket_struct {
  int comm_sock; // socket file descriptor
  hashtable_t * ht; //hashtable: Keys are player IDs, items are sock_addr structs
  struct sockaddr_in *lastsender; // the last client to send a message
} socket_struct_t;





// Static functions and declarations
static void hashprint(FILE *fp, const char *key, void *item);
static const int IDSIZE = 9;






/**************** socket_struct_new ****************/
/*                                                                                                                                                            
 * create new socket_struct                                                                                                                                   
 * return pointer to struct or                                                                                                                               
 * NULL if error                                                                                                                                             
 *                                                                                                                                                           
 */
socket_struct_t* socket_struct_new(void){
 socket_struct_t* sock_struct = malloc(sizeof(socket_struct_t));

  if (sock_struct == NULL){
    return NULL;
  }

  sock_struct->comm_sock = -99;
  sock_struct->ht = NULL;
  sock_struct->lastsender = NULL;
  
  return sock_struct;
}

int get_comm_socket(socket_struct_t *sock_struct)
{
  return (sock_struct->comm_sock);
}

hashtable_t* get_address_ht(socket_struct_t *sock_struct)
{
  return (sock_struct->ht);
}
void set_address_ht(socket_struct_t *sock_struct, hashtable_t *ht)
{
  sock_struct->ht = ht;
}

void set_comm_socket(socket_struct_t *sock_struct, int comm_sock)
{
  sock_struct->comm_sock =comm_sock;
}

struct sockaddr_in * get_last_sender(socket_struct_t *sock_struct)
{
  return (sock_struct->lastsender);
}

void set_last_sender( socket_struct_t *sock_struct, struct sockaddr_in *lastsender)
{
  sock_struct->lastsender = lastsender;
}


/*************** send_message ________________/                                                                                                         
 * sends a message to the client(iD)                                                                       
 * takes a message struct, converts it into a string, and sends it out                                                                                                        
 * MEMORY CONTRACT: send_message frees the message -- this is to keep free-ing out of opCode_handler when possible.                                                                                                        
 * Every pointer in the message must be malloc'ed for or you will get memory errors!                                                                                                         
 *
 */
void send_message(message_t *message, unsigned int iD, socket_struct_t *sock_struct)
{
  
  fprintf(stdout, "sending message to %i\n", iD);

  //convert player id into a char*
  char playerID[IDSIZE];
  sprintf(playerID, "%x", iD);
  //convert the message into a char*
  char* dg= message_to_string(message);
  printf("SENDING MESSAGE: %s\n", dg);


  
  struct sockaddr_in* address;            // sender of this message                                         
  struct sockaddr *addressp;// = (struct sockaddr *) &address;                                             
  socklen_t senderlen = sizeof(*addressp);

  //get the address of the sender from the hashtable                                                                  
  fprintf(stdout, "about to get address of sender\n");

  // Return to sender if id == * 
  if (iD == '*'){
    address = sock_struct->lastsender;
    //fprintf(stdout, "sending to different IP address");
  }
  
  else{
    address= hashtable_find(sock_struct->ht,playerID);
  }
  
  addressp= (struct sockaddr *) address;

 fprintf(stdout, "printing hashtable of address keys:\n");
 hashtable_print(sock_struct->ht, stdout, hashprint);
  
  
  if (addressp != NULL){
    
    
    if (sendto(sock_struct->comm_sock, dg, strlen(dg), 0, addressp, senderlen) < 0) {
      perror("sending in datagram socket");
      exit(6);
    } else {
      fprintf(stdout, "datagram sent\n");
      print_log(dg, address, "TO");
    }
  }
  printf("NULL ADDRESS\n");
  //clean up
  free(dg);
  free(message);	 
}


static void
hashprint(FILE *fp, const char *key, void *item)
{
  int *num = item;
  if (num == NULL || key == NULL)
    fprintf(fp, "[(null)]");
  else
    fprintf(fp, "key: %s item: %p",
            key, item);
}




 /**************** print_log *************/
 /* Prints a message to ../logs/gameserver.log 
  * Appends to the end of the log file -- if you need to 
  * reset the file, you must do so separately!
  */
void print_log(char * message, struct sockaddr_in *address, char* tofrom) {
  //  unsigned long addressnum = address->sin_addr.s_addr;
  //unsigned short addressport = address->sin_port;
  char timestamp[27];
  time_t clk = time(NULL);
  sprintf(timestamp, "(%s", ctime(&clk));
  timestamp[25] = ')';
  FILE *fp = fopen("../logs/gameserver.log", "a");
  fprintf(fp, "%s %s %s@%05d: %s\n", timestamp, tofrom, inet_ntoa(address->sin_addr), ntohs(address->sin_port), message);
  fclose(fp);
}












