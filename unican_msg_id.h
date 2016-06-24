#ifndef UNICAN_MESSAGE_ID_H_
#define UNICAN_MESSAGE_ID_H_


#define UNICAN_AVS_TELEMETRY_X 0x0B10
#define UNICAN_AVS_TELEMETRY_Y 0x0B11
#define UNICAN_AVS_TELEMETRY_Z 0x0B12

#define UNICAN_MACHINE_SWITCH_TO_STATE_IDLE 0xF000
#define UNICAN_MACHINE_SWITCH_TO_STATE_1 0xF001
#define UNICAN_MACHINE_SWITCH_TO_STATE_2 0xF002

//Управление скоростью (об/мин)
#define UNICAN_WHEEL_CMD_ANGULAR_RATE				0x0A01
//Управление моментом (Нм)
#define UNICAN_WHEEL_CMD_TORQUE						0x0A10
//Выключить выдачу регулярной телеметрии
#define UNICAN_WHEEL_DISABLE_REGULAR_TELEMETRY		0x0A22
//Запросить выдачу телеметрии Маховика
#define UNICAN_WHEEL_GET_REGULAR_TELEMETRY			0x0A31
//Запросить выдачу общей телеметрии
#define UNICAN_WHEEL_GET_GENERYC_TELEMETRY			0x0A41
//Выключить питание маховика (инвертора)
#define UNICAN_WHEEL_OUTPUT_DISABLE					0x0A51
//Включить питание маховика (инвертора)
#define UNICAN_WHEEL_OUTPUT_ENABLE					0x0A52
//Изменить DestinationID (изменяется до перезагрузки, если не передать команду "Установить по умолчанию\n") <- FLASH
#define UNICAN_WHEEL_SET_DEFAULT_DESTINATION_ID		0x0A61
//Изменить SourseID  (изменяется до перезагрузки, если не передать команду "Установить по умолчанию\n")		<- FLASH
#define UNICAN_WHEEL_SET_UNICAN_ID 					0x0A62
//Включить выдачу регулярной телеметрии
#define UNICAN_WHEEL_ENABLE_REGULAR_TELEMETRY		0x0A63
//Установить Пропорциональный коэффициент																	<- FLASH
#define UNICAN_WHEEL_SET_PID_PROP_KOEF				0x0A64
//Установить Интегральный коэффициент																		<- FLASH
#define UNICAN_WHEEL_SET_PID_INTEGR_KOEF			0x0A65
//Установить Дифференциальный коэффициент																	<- FLASH
#define UNICAN_WHEEL_SET_PID_DIFF_KOEFF				0x0A66
//Записать новые настройки во FLASH																			<- FLASH

#define UNICAN_WHEEL_PROGRAMM_SETTINGS				0x0A60

#define UNICAN_START_LONG_MESSAGE 0xFFFE

#endif /* UNICAN_MESSAGE_ID_H_ */
