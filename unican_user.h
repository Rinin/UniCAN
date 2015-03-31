#ifndef UNICAN_USER_H_
#define UNICAN_USER_H_
#include <stdlib.h> //We are using malloc() and free() so we need them HW/compiler depended


#define UNICAN_RX_BUFFERS_COUNT 5 //Count of Rx buffers for long messages
                                  //recomended to keep this value equal to 
                                  //number of devices, which could send long messages.
#define UNICAN_ALLOWED_SPACE 0x100000 //1MB
typedef unsigned char uint8;
typedef unsigned int  uint16;
typedef unsigned long uint32;
typedef char int8;
typedef int  int16;
typedef long int32;

void can_HW_receive_message (void);//HW implementation, may be changed.
void unican_RX_event (uint16 msg_id, uint16 length);


#endif /* UNICAN_USER_H_ */
