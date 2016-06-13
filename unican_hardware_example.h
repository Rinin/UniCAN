#pragma once

#include "stm32f4xx_rcc.h"

#define UINT16LEFT(val) \
  (((val) >> 8) & 0x00FF)
  
#define UINT16RIGHT(val) \
  ((val) & 0x00FF)
  
#define CAN_STANDART_HEADER CAN_Id_Standard
#define CAN_EXTENDED_HEADER CAN_Id_Extended

typedef struct
{
	CanTxMsg TxMsg[256];
	uint8_t NowCanBufPoint;
	uint8_t LastCanBufPoint;
}Typdef_CAN_HW_TX_buf;

typedef struct tag_can_message
{
  uint32_t can_identifier;                     // 11 or 29bit CAN identifier
  uint8_t can_rtr;                             // Remote transmission request bit
  uint8_t can_extbit;                          // Identifier extension bit. 0x00 indicate 11 bit message ID
  uint8_t can_dlc;                             // Data length code. Number of bytes of data (0–8 bytes)
  volatile uint8_t data[8];                    // Data field
}  __attribute__ ((packed)) can_message;

void can_send_message (can_message* msg);
void can_HW_init (void);                     //HW implementation
void can_HW_receive_message (void);          //HW implementation, may be changed.
void can_HW_send_message (can_message* msg); //HW implementation
void can_HW_close(void);                     //HW implementation



