#pragma once

//Count of Rx buffers for long messages
//recomended to keep this value equal to
//number of devices, which could send long messages.
#define UNICAN_RX_BUFFERS_COUNT 5

//Amount of memory reserved for received UniCAN
//messages which is still not processed
#define UNICAN_ALLOWED_SPACE 0x200

extern volatile int UNICAN_ID;
