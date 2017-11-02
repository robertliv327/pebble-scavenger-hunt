/*                                                                                                                                            
 * Refer to message_handler.h for more details                                                                                                 
 * for guide agent                                                                                                                                           
 *                                                                                                                                            
 *                                                                                                                                            
 *                                                                                                                                            
 */

#include "message_handler.h"

typedef struct socket_struct {
  int comm_sock;
  struct sockaddr_in *sAddress;
} socket_struct_t;





static const int IDSIZE = 9;






/**************** socket_struct_new ****************/
/*                                                                             \                                                              
                                                                                                                                              
 * create new socket_struct                                                    \                                                              
                                                                                                                                              
 * return pointer to struct or                                                 \                                                              
                                                                                                                                              
 * NULL if error                                                               \                                                              
                                                                                                                                              

 *                                                                             \                                                              
                                                                                                  */
socket_struct_t* socket_struct_new(void){
 socket_struct_t* sock_struct = malloc(sizeof(socket_struct_t));

  if (sock_struct == NULL){
    return NULL;
  }

  sock_struct->comm_sock = -99;
  sock_struct->sAddress = NULL;

  return sock_struct;
}

int get_comm_socket(socket_struct_t *sock_struct)
{
  return (sock_struct->comm_sock);
}

struct sockaddr_in * get_address(socket_struct_t *sock_struct)
{
  return (sock_struct->sAddress);
}
void set_address(socket_struct_t *sock_struct, struct sockaddr_in *sAddress)
{
  sock_struct->sAddress = sAddress;
}

void set_comm_socket(socket_struct_t *sock_struct, int comm_sock)
{
  sock_struct->comm_sock =comm_sock;
}


/*                                                                                                                                            
 * sends a message to the client(iD)                                                                                                          
 *                                                                                                                                            
 *                                                                                                                                            
 *                                                                                                                                            
 */


/*
void send_message_gs(message_t *message, int iD, socket_struct_t *sock_struct)
{
  //for testing                                                                                                                               
  unsigned int gameID= get_gameId(message);
  char * opCode= get_opCode(message);

  unsigned int ID;
  if ((get_pebbleId(message)) != 0 ){
    ID=get_pebbleId(message);
    fprintf(stdout, "message to pebble %u\n", ID);
  }
  else {
      ID= get_guideId(message);
      fprintf(stdout, "message to guide agent %u\n", ID);
  }

  fprintf(stdout, "%s, %u\n\n", opCode, gameID);
    //end testing                                                                                                                             


  char *rep= get_respCode(message);
  fprintf(stdout, "response code: %s\n", rep);

    //convert player id into a char*                                                                                                          
    char playerID[IDSIZE];
  //itoa(iD, playerID, 16);                                                                                                                   
  sprintf(playerID, "%x", iD);
  //puts(playerID);                                                                                                                           


  char* dg= message_to_string(message);

  struct sockaddr_in address;            // sender of this message                                                                            
  struct sockaddr *addressp;// = (struct sockaddr *) &address;                                                                                
  socklen_t senderlen = sizeof(address);
  //get the address of the sender from the hashtable                                                                                          
  fprintf(stdout, "about to get address of sender\n");
  addressp= hashtable_find(sock_struct->ht,playerID);


    if (addressp != NULL){
      //send to client                                                                                                                        
      fprintf(stdout, "address of not null");

      //change from cd opCode strlen(opCode) to message sizeof(message)                                                                       
      if (sendto(sock_struct->comm_sock, dg, strlen(dg), 0,
                 addressp, senderlen) < 0) {
        perror("sending in datagram socket");
        exit(6);
      }

      else{
        fprintf(stdout, "datagram sent");
      }
    }

}
*/



