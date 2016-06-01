#ifndef UNICAN_H_
#define UNICAN_H_

#include "unican_setup.h"
#include "unican_user.h"
#include "unican_errcodes.h"
#include "unican_msg_id.h"
#include "unican_hardware.h" //HARDWARE_INCLUDE
#include <stdint.h>

#define SERV
#undef SERV

#ifndef NULL
#define NULL (void*)0
#endif /* NULL */

#define CAN_MAX_DLC 8 //maximum value of data length code
#define CAN_MIN_DLC 2 //minimum value of data length code
#define UNICAN_HEADER_SIZE 6
#define UNICAN_ENABLED  1
#define UNICAN_DISABLED 0
//typedef struct tag_unican_message unican_message;

typedef struct tag_unican_message
{
  uint16 unican_msg_id; //MSG_ID of unican message
  uint16 unican_address_from; // address of sender in sattelite network
  uint16 unican_address_to; // address of receiver in sattelite network
  uint16 unican_length; //length of data
  uint8* data; //pointer to data field
}  __attribute__ ((packed)) unican_message;

typedef struct tag_unican_node unican_node;

struct tag_unican_node
{
  unican_message* value; //pointer to holded message
  unican_node* next_node; //pointer to next_message
}  __attribute__ ((packed));

typedef struct tag_unican_status
{
  uint8 is_online;
  uint32 free_space; 
  uint32 nodes_count;
  uint16 free_buffers_count;
  unican_node* first;
  unican_node* last;
  
}  __attribute__ ((packed)) unican_status;

typedef struct tag_unican_buffer
{
  unican_node* node; //node with MSG accamulated in buffer
//  uint16 crc; // CRC of message, 0 for short messages
  uint16 position; //Current position in buffer
}  __attribute__ ((packed)) unican_buffer;


void unican_init (void);
void unican_close (void);

void can_receive_message(can_message* msg);
void unican_take_message (void);
void unican_RX_message (unican_message* msg);
void unican_send_message (unican_message* msg);

void unican_drop_node(unican_node* node);

#endif /* UNICAN_H_ */
