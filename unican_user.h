#pragma once

#include <stdlib.h> //malloc
#include <stdint.h>
#include "string.h"
#include "stddef.h"
#include "unican_msg_id.h"
#include "PMSM_settings.h"
#include "IO_structs.h"

typedef struct UNICAN_DATA_FIELD_DESCRIPTOR_TAG{
	uint32_t address_offset;
	uint32_t size;
}UNICAN_DATA_FIELD_DESCRIPTOR;

//extern const UNICAN_DATA_FIELD_DESCRIPTOR UNICAN_MSG_ID_SETTINGS_MAP[UNICAN_SET_NUM_SET_CODES];
extern volatile uint32_t UNICAN_CONTROL_ARRAY[UNICAN_NUM_ARRAY_CODES];

void unican_RX_event (uint16_t msg_id, uint16_t length);
void unican_error (uint16_t errcode);
void unican_send_regular_telemetry();
void unican_settings_array_write(PMSM_SETTINGS *PMSM_configuration);
void unican_settings_array_read(PMSM_SETTINGS *PMSM_configuration);
void unican_dump_settings();

//Преобразование MESSAGE_ID в индекс массива с настройками.
#define unican_MSG_ID_to_ARRAY_index(UNICAN_MSG_ID)\
		((UNICAN_MSG_ID) - UNICAN_FIRST_ARRAY_CODE)

//функции преобразования индексов для группы телеметрии.
#define unican_MSG_ID_to_TELEMETRY_index(UNICAN_MSG_ID)\
		((UNICAN_MSG_ID) - UNICAN_TELEMETRY_FIRST_TELEMETRY_CODE)

#define unican_TELEMETRY_index_to_MSG_ID(UNICAN_TELEMETRY_INDEX)\
		((UNICAN_TELEMETRY_INDEX) + UNICAN_TELEMETRY_FIRST_TELEMETRY_CODE)

#define unican_TELEMETRY_index_to_ARRAY_index(UNICAN_TELEMETRY_INDEX)\
		((UNICAN_TELEMETRY_INDEX) + UNICAN_TELEMETRY_FIRST_TELEMETRY_CODE - UNICAN_FIRST_ARRAY_CODE)

#define unican_ARRAY_index_to_TELEMETRY_index(UNICAN_ARRAY_INDEX)\
		((UNICAN_ARRAY_INDEX) - (UNICAN_TELEMETRY_FIRST_TELEMETRY_CODE - UNICAN_FIRST_ARRAY_CODE))




//функции преобразования индексов для группы включателей/выключателей телеметрии.
#define unican_MSG_ID_to_ENABLE_index(UNICAN_MSG_ID)\
		((UNICAN_MSG_ID) - UNICAN_ENABLE_FIRST_ENABLE_CODE)

#define unican_ENABLE_index_to_MSG_ID(UNICAN_ENABLE_INDEX)\
		((UNICAN_ENABLE_INDEX) + UNICAN_ENABLE_FIRST_ENABLE_CODE)

#define unican_ENABLE_index_to_ARRAY_index(UNICAN_ENABLE_INDEX)\
		((UNICAN_ENABLE_INDEX) + UNICAN_ENABLE_FIRST_ENABLE_CODE - UNICAN_FIRST_ARRAY_CODE)

#define unican_ARRAY_index_to_ENABLE_index(UNICAN_ARRAY_INDEX)\
		((UNICAN_ARRAY_INDEX) - (UNICAN_ENABLE_FIRST_ENABLE_CODE - UNICAN_FIRST_ARRAY_CODE))




//функции преобразования индексов для группы настроек.
#define unican_MSG_ID_to_SETTINGS_index(UNICAN_MSG_ID)\
		((UNICAN_MSG_ID) - UNICAN_SET_FIRST_SET_CODE)

#define unican_SETTINGS_index_to_MSG_ID(UNICAN_SETTINGS_INDEX)\
		((UNICAN_SETTINGS_INDEX) + UNICAN_SET_FIRST_SET_CODE)

#define unican_SETTINGS_index_to_ARRAY_index(UNICAN_SETTINGS_INDEX)\
		((UNICAN_SETTINGS_INDEX) + UNICAN_SETTINGS_FIRST_SETTINGS_CODE - UNICAN_FIRST_ARRAY_CODE)

#define unican_ARRAY_index_to_SETTINGS_index(UNICAN_ARRAY_INDEX)\
		((UNICAN_ARRAY_INDEX) - (UNICAN_SETTINGS_FIRST_SETTINGS_CODE - UNICAN_FIRST_ARRAY_CODE))


//сервисные функции для чтения/записи непосредственно по unican_msg_id.
static inline __attribute__ ((always_inline))
void unican_set_array_by_MSG_ID(uint16_t MSG_ID, void *value, uint16_t size){
	memcpy((void *)&UNICAN_CONTROL_ARRAY[unican_MSG_ID_to_ARRAY_index(MSG_ID)], value, size);
}

static inline __attribute__ ((always_inline))
void unican_get_array_by_MSG_ID(uint16_t MSG_ID, void *value, uint16_t size){
	memcpy(value, (void *)&UNICAN_CONTROL_ARRAY[unican_MSG_ID_to_ARRAY_index(MSG_ID)], size);
}

static inline __attribute__ ((always_inline))
PMSM_COMMAND_DESCRIPTOR unican_get_PMSM_COMMAND_by_MSG_ID(uint16_t MSG_ID){

	PMSM_COMMAND_DESCRIPTOR return_value;
	switch(MSG_ID & 0xFFFF){
	case UNICAN_COMMAND_W_SIMPLE:{
		return_value = PMSM_COMMAND_ANGULAR_RATE;
	}break;
	case UNICAN_COMMAND_W_KALMAN:{
		return_value = PMSM_COMMAND_ANGULAR_RATE_WITH_KALMAN;
	}break;
	case UNICAN_COMMAND_TORQUE_SIMPLE:{
		return_value = PMSM_COMMAND_TORQUE;
	}break;
	case UNICAN_COMMAND_TORQUE_KALMAN:{
		return_value = PMSM_COMMAND_TORQUE_WITH_KALMAN;
	}break;
	case UNICAN_COMMAND_CALIB_HALL:{
		return_value = PMSM_COMMAND_CALIB_HALL;
	}break;
	case UNICAN_COMMAND_CALIB_CURRENT:{
		return_value = PMSM_COMMAND_CALIB_CURRENT;
	}break;
	default:{
		/*Остальные коды команд являются служебными
			и обрабатываться никак не должны.*/
		return_value = PMSM_COMMAND_BAD_COMMAND;
	}break;
	}

	return return_value;
}
