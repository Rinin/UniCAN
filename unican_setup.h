#pragma once
#include <stdint.h>

#include "IO_structs.h"

//Count of Rx buffers for long messages
//recomended to keep this value equal to
//number of devices, which could send long messages.
#define UNICAN_RX_BUFFERS_COUNT 5

//Amount of memory reserved for received UniCAN
//messages which is still not processed
#define UNICAN_ALLOWED_SPACE 0x200

static const uint8_t 	UNICAN_DEFAULT_SELF_ID = 0x11;
static const uint8_t 	UNICAN_DEFAULT_DESTINATION_ID = 0x05;

typedef struct unican_settings_tag{
	uint16_t 	UNICAN_SELF_ID;
	uint16_t 	UNICAN_DESTINATION_ID;
	float 		Telemetry_period;
} unican_settings;

void unican_set_new_settings(unican_settings *new_unican_settings);
void unican_get_current_settings_pointer(unican_settings **current_unican_settings_pointer);
