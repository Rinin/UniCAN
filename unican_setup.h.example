#ifndef UNICAN_SETUP_H_
#define UNICAN_SETUP_H_

//Apropriate sizes according to compiller
typedef unsigned char uint8;
typedef unsigned short int  uint16;
typedef unsigned long uint32;
typedef char int8;
typedef short int  int16;
typedef long int32;

#define DRIVERS_SOURCE drivers/          //Path to directory with hardware 
                                         //drivers

#define HARDWARE                         //Name of used Hardware driver, driver should
                                         //consist HARDWARE folder under DRIVER_SOURCE
                                         
//#define WRITE_INCLUDE (p, n) {"\""##p##n##"/unican_hardware.h\""}
//#define EVALUATE_INCLUDE (p, n) WRITE_INCLUDE (p, n)
//#define HARDWARE_INCLUDE EVALUATE_INCLUDE(DRIVERS_SOURCE, HARDWARE)
#define HARDWARE_INCLUDE "drivers/IXXAT109_linux/unican_hardware.h"

#define UNICAN_RX_BUFFERS_COUNT 5        //Count of Rx buffers for long messages
                                         //recomended to keep this value equal to 
                                         //number of devices, which could send long messages.
                                  
#define UNICAN_ALLOWED_SPACE 0x100000    //Amount of memory reserved for received UniCAN
                                         //messages which is still not processed



#endif /* UNICAN_SETUP_H_ */
