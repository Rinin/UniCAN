#ifndef UNICAN_USER_H_
#define UNICAN_USER_H_

#define UNICAN_RX_BUFFERS_COUNT 5 //Count of Rx buffers for long messages
                                  //recomended to keep this value equal to 
                                  //number of devices, which could send long messages.

void can_HW_receive_message (void);//HW implementation, may be changed.

typedef unsigned char uint8;
typedef unsigned int  uint16;
typedef unsigned long uint32;
typedef char int8;
typedef int  int16;
typedef long int32;

#endif /* UNICAN_USER_H_ */
