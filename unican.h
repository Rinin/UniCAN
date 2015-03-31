#ifndef UNICAN_H_
#define UNICAN_H_
#include "unican_user.h"
#include "unican_errcodes.h"
#include "unican_msg_id.h"

#ifndef NULL
#define NULL (void*)0
#endif /* NULL */

#define CAN_MAX_DLC 8 //maximum value of data length code
#define CAN_MIN_DLC 2 //minimum value of data length code
#define UNICAN_HEADER_SIZE 8
typedef struct tag_unican_message unican_message;

typedef struct tag_unican_status
{
  uint8 is_online;
  uint32 free_space; //Free space reserved for unican messages //TODO:init it
  uint32 messages_count;
  uint16 free_buffers_count;
  unican_message* first;
  unican_message* last;
  
}  __attribute__ ((packed)) unican_status;

typedef struct tag_can_message
{
  uint32 can_identifier; // 11 or 29bit CAN identifier
  uint8 can_rtr; // Remote transmission request bit
  uint8 can_extbit; // Identifier extension bit. 0x00 indicate 11 bit message ID
  uint8 can_dlc; //Data length code. Number of bytes of data (0–8 bytes)
  volatile uint8 data[8]; //Data field
}  __attribute__ ((packed)) can_message;

struct tag_unican_message
{
  uint16 unican_msg_id; //MSG_ID of unican message
  uint16 unican_address_from; // address of sender in sattelite network
  uint16 unican_address_to; // address of receiver in sattelite network
  uint16 unican_length; //length of data 		
  uint8* data; //pointer to data field
  unican_message* next_msg; //pointer to next_message
}  __attribute__ ((packed));

typedef struct tag_unican_buffer
{
  unican_message* umsg; //MSG accamulated in buffer
  uint16 crc; // CRC of message, 0 for short messages
  uint16 position; //Current position in buffer
}  __attribute__ ((packed)) unican_buffer;

uint16 unican_init (void);

void can_HW_send_message (can_message* msg); //HW implementation

uint16 can_receive_message (can_message* msg);
inline void can_send_message (can_message* msg); 

void unican_proceed_message (unican_message* msg);
void unican_send_message (unican_message* msg);

void unican_drop_message(unican_message* msg);

#endif /* UNICAN_H_ */
