#ifndef UNICAN_H_
#define UNICAN_H_
#include "unican_user.h"
#include "unican_errcodes.h"

#define CAN_MAX_DLC 8 //maximum value of data length code
#define CAN_MIN_DLC 2 //minimum value of data length code

typedef struct tag_can_message
{
  uint32 can_identifier; // 11 or 29bit CAN identifier
  uint8 can_rtr; // Remote transmission request bit
  uint8 can_extbit; // Identifier extension bit. 0x00 indicate 11 bit message ID
  uint8 can_dlc; //Data length code. Number of bytes of data (0–8 bytes)
  volatile uint8 data[8]; //Data field
}  __attribute__ ((packed)) can_message;

typedef struct tag_unican_message
{
  uint16 unican_msg_id; //MSG_ID of unican message
  uint16 unican_address_from; // address of sender in sattelite network
  uint16 unican_address_to; // address of receiver in sattelite network
  uint16 unican_length; //length of data 
		
  volatile uint8* data; //pointer to data field
}  __attribute__ ((packed)) unican_message;

typedef struct tag_unican_buffer
{

}
void can_HW_send_message (can_message* msg); //HW implementation

uint16 can_receive_message (can_message* msg);
void can_send_message (can_message* msg); 

void unican_proceed_message (unican_message* msg);
void unican_send_message (unican_message* msg);

void unican_create_msg(unican_message* msg);
void unican_drop_msg(unican_message* msg);

#endif /* UNICAN_H_ */
