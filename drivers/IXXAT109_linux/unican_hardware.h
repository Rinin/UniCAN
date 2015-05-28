#ifndef UNICAN_HARDWARE_H_
#define UNICAN_HARDWARE_H_
#include "EciLinux_i386/inc/ECI109.h"

#define UINT16LEFT(val) \
  ((val >> 8) & 0x00FF)
  
#define UINT16RIGHT(val) \
  (val & 0x00FF)
  
#define CAN_STANDART_HEADER 0
#define CAN_EXTENDED_HEADER 1

typedef struct tag_can_message
{
  uint32 can_identifier;                     // 11 or 29bit CAN identifier
  uint8 can_rtr;                             // Remote transmission request bit
  uint8 can_extbit;                          // Identifier extension bit. 0x00 indicate 11 bit message ID
  uint8 can_dlc;                             // Data length code. Number of bytes of data (0–8 bytes)
  volatile uint8 data[8];                    // Data field
}  __attribute__ ((packed)) can_message;

void can_send_message (can_message* msg);

void can_HW_init (void);                     //HW implementation
void can_HW_receive_message (void);          //HW implementation, may be changed.
void can_HW_send_message (can_message* msg); //HW implementation
void can_HW_close(void);                     //HW implementation

//////////////////////////////////////////////////////////////////////////
// static constants, types, macros, variables

/** ECI Demo send timeout in [ms] @ingroup EciDemo */
#define ECIDEMO_TX_TIMEOUT 500

/** ECI Demo TX message count for CAN @ingroup EciDemo */
#define ECIDEMO_TX_MSGCOUNT_CAN (0x800 * 10)

/** ECI Demo receive timeout in [ms] @ingroup EciDemo */
#define ECIDEMO_RX_TIMEOUT 500

/** ECI Demo total receive timeout in [ms] @ingroup EciDemo */
#define ECIDEMO_RX_TOTALTIMEOUT 30000

/**
  If defined ECI Demo will run in polling mode instead of using an
 event triggered mode. This mode can be used, if it is not possible to
  assign an unique IRQ to the device to use. The device driver than
  will not use an IRQ based message reception and transmission.

  @ingroup EciDemo
*/
//#define ECIDEMO_HWUSEPOLLINGMODE

/** ECI Demo error check macro @ingroup EciDemo */
#define ECIDEMO_CHECKERROR(FuncName) \
{\
  if(ECI_OK == hResult)\
  {\
    OS_Printf(#FuncName "...succeeded.\n"); \
  }\
  else\
  {\
    OS_Printf( #FuncName "...failed with errorcode: 0x%08X. %s\n", \
               hResult, \
               ECI109_GetErrorString(hResult)); \
  }\
}



#endif /* UNICAN_HARDWARE_H_ */
