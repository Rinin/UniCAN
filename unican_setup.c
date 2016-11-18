#include "unican_setup.h"
#include "IO_tools.h"
#include "unican_hardware_example.h"
#include "string.h"
#include "stdlib.h"

volatile unican_settings current_unican_settings = {
		.UNICAN_SELF_ID = 0x11,
		.UNICAN_DESTINATION_ID = 0x05,
		.Telemetry_period = 0.1f
};

void unican_set_new_settings(unican_settings *new_unican_settings){
	DEBUG_PRINT("%s\n", __FUNCTION__);

	memcpy((void *)&current_unican_settings, (void *)new_unican_settings, sizeof(unican_settings));
	can_filter_init(current_unican_settings.UNICAN_SELF_ID, CAN_FilterScale_16bit);

	DEBUG_PRINT("SELF_ID: 0x%02X\n", current_unican_settings.UNICAN_SELF_ID);
	DEBUG_PRINT("DEST_ID: 0x%02X\n", current_unican_settings.UNICAN_DESTINATION_ID);
	DEBUG_PRINT("Telemetry period: %f\n", current_unican_settings.Telemetry_period);
}

void unican_get_current_settings_pointer(unican_settings **current_unican_settings_pointer){
	*current_unican_settings_pointer = (unican_settings *)&current_unican_settings;
}
